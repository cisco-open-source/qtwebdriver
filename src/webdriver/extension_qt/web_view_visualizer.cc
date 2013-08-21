#include "web_view_visualizer.h"

#include <stdexcept>
#include <QtCore/QEventLoop>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include "base/stringprintf.h"
#include "extension_qt/web_view_executor.h"
#include "webdriver_session.h"
#include "webdriver_util.h"

namespace webdriver {

QWebViewVisualizerSourceCommand::QWebViewVisualizerSourceCommand(QWebViewCmdExecutor* executor, Session* session, QWebView* view)
    : executor_(executor), session_(session), view_(view)
{}

void QWebViewVisualizerSourceCommand::Execute(std::string* source, Error** error) {
    // Convert DOM tree to valid XML.
    const char* kSource =
        "function() {\n"
        "  var elements = document.getElementsByTagName('*');\n"
        "  for (var elementIndex = 0; elementIndex < elements.length; elementIndex++) {\n"
        "    var element = elements[elementIndex];\n"
        "    if (element.tagName.toLowerCase() == 'script')\n"
        "      element.innerHTML = '';\n"
        "    if (element.hasAttribute('\"'))\n"
        "      element.removeAttribute('\"');\n"
        "\n"
        "    var childNodes = element.childNodes;\n"
        "    for (var childIndex = 0; childIndex < childNodes.length; childIndex++) {\n"
        "      var child = childNodes[childIndex];\n"
        "      if (child.nodeType == Node.TEXT_NODE) {\n"
        "        child.nodeValue = child.nodeValue.replace(/&/g, '&amp;');\n"
        "      }\n"
        "    }\n"
        "  }\n"
        "\n"
        "  var xhtml = document.implementation.createDocument();\n"
        "  xhtml = xhtml.importNode(document.documentElement, true);\n"
        "\n"
        "  return new XMLSerializer().serializeToString(xhtml);\n"
        "}";

    *error = executor_->ExecuteScriptAndParse(
                executor_->GetFrame(view_, session_->current_frame()),
                kSource,
                "getSource",
                new ListValue(),
                CreateDirectValueParser(source));

    session_->logger().Log(kInfoLogLevel, "[QWebViewVisualizerSourceCommand] before transform:");
    session_->logger().Log(kInfoLogLevel, *source);

    QSharedPointer<QDomDocument> document = ParseXml(QString::fromStdString(*source), error);
    if (!document)
        return;

    QDomElement documentElement = document->documentElement();
    AssemblePage(documentElement);

    *source = document->toString().toStdString();

    session_->logger().Log(kInfoLogLevel, "[QWebViewVisualizerSourceCommand] after transform:");
    session_->logger().Log(kInfoLogLevel, *source);
}

QSharedPointer<QDomDocument> QWebViewVisualizerSourceCommand::ParseXml(const QString& input, Error** error) {
    if (*error) throw std::invalid_argument("error");

    QString errorMsg;
    int errorLine = 0, errorColumn = 0;
    QSharedPointer<QDomDocument> document(new QDomDocument());
    bool retval = document->setContent(input, &errorMsg, &errorLine, &errorColumn);
    if (!retval) {
        std::string details = base::StringPrintf(
                    "ParseXml failed with error message '%s' at line %d column %d", errorMsg.toStdString().c_str(), errorLine, errorColumn);
        *error = new Error(kInternalServerError, details);
        return QSharedPointer<QDomDocument>();
    }

    return document;
}

void QWebViewVisualizerSourceCommand::UnescapeXml(QString& input) {
    input.replace("&amp;", "&");
}

void QWebViewVisualizerSourceCommand::AssemblePage(QDomElement element) const {
    if (element.tagName() == "img") {
        AssembleImg(element);
    }
    if (element.tagName() == "link") {
        AssembleLink(element);
    }
    if (element.tagName() == "style") {
        AssembleStyle(element);
    }

    if (element.hasAttribute("style")) {
        AssembleStyle(element.attributeNode("style"));
    }

    RemoveScripts(element);

    // Chrome does like empty tags like <textarea/>
    if (element.tagName() != "br" &&
        element.childNodes().length() == 0)
        element.appendChild(element.ownerDocument().createTextNode(" "));

    // Recursively walk DOM tree
    QDomNodeList children = element.childNodes();
    for (int childIndex = 0; childIndex < children.length(); childIndex++) {
        QDomNode child = children.at(childIndex);
        if (child.nodeType() == QDomNode::ElementNode) {
            QDomElement childElement = child.toElement();
            AssemblePage(childElement);
        }
    }
}

void QWebViewVisualizerSourceCommand::AssembleLink(QDomElement element) const {
    QString rel = element.attribute("rel");
    QString type = element.attribute("type");
    if (rel != "stylesheet" && type != "text/css")
        return;

    QString href = element.attribute("href");
    UnescapeXml(href);
    QString url = AbsoluteUrl(href);
    QByteArray file;
    QString contentType;
    Download(url, &file, &contentType);

    QDomElement style = element.ownerDocument().createElement("style");
    if (!type.isEmpty())
        style.setAttribute("type", type);
    else if (!contentType.isEmpty())
        style.setAttribute("type", contentType.split(';')[0]);

    if (!file.isEmpty()) {
        style.appendChild(style.ownerDocument().createTextNode(file));
        element.parentNode().replaceChild(style, element);
    } else {
        element.parentNode().removeChild(element);
    }
}

// Convert <img> tag 'src' attribute to base64
void QWebViewVisualizerSourceCommand::AssembleImg(QDomElement element) const {
    QString url = element.attribute("src");
    if (!url.startsWith(DATA_PROTOCOL)) {
        element.setAttribute("src", DownloadAndEncode(url));
    }

    if (element.hasAttribute("srcset"))
        element.removeAttribute("srcset");
}

void QWebViewVisualizerSourceCommand::AssembleStyle(QDomElement element) const {
    QString type = element.attribute("type");
    if (type.length() != 0 && type != "text/css")
        return;

    QString value = element.text();
    value = AssembleStyle(value);
    element.firstChild().toText().setData(value);
}

void QWebViewVisualizerSourceCommand::AssembleStyle(QDomAttr attribute) const {
    QString value = attribute.value();
    value = AssembleStyle(value);
    attribute.setValue(value);
}

QString QWebViewVisualizerSourceCommand::AssembleStyle(const QString& value) const {
    QRegExp regex(":\\s*url\\(([^\\)]+)\\)");
    QString result;

    int lastMatchEnd = 0;
    int newMatchStart;
    while ((newMatchStart = regex.indexIn(value, lastMatchEnd)) != -1) {
        result += value.mid(lastMatchEnd, newMatchStart - lastMatchEnd);

        QString url = regex.cap(1);
        url = trimmed(url, " \t'\"");

        if (!url.startsWith(DATA_PROTOCOL))
            url = DownloadAndEncode(url);

        result += ":url('" + url + "')";

        lastMatchEnd = newMatchStart + regex.matchedLength();
    }

    if (lastMatchEnd != value.length() - 1) {
        result += value.mid(lastMatchEnd, value.length() - lastMatchEnd);
    }
    return result;
}

// Remove <script> tags
void QWebViewVisualizerSourceCommand::RemoveScripts(QDomElement element) const {
    std::vector<QDomNode> scripts;

    QDomNodeList children = element.childNodes();
    for (int childIndex = 0; childIndex < children.length(); childIndex++) {
        QDomNode child = children.at(childIndex);
        if (child.nodeType() == QDomNode::ElementNode) {
            QDomElement childElement = child.toElement();
            if (childElement.tagName() == "script") {
                scripts.push_back(childElement);
            }
        }
    }

    for (std::vector<QDomNode>::const_iterator it = scripts.begin(); it != scripts.end(); it++) {
        element.removeChild(*it);
    }
}

QString QWebViewVisualizerSourceCommand::AbsoluteUrl(const QString& url) const {
    if (url.contains("://"))
        return url;

    if (url.startsWith("//"))
        return view_->url().scheme() + ":" + url;
    else if (url.startsWith('/'))
        return view_->url().scheme() + "://" + view_->url().host() + url;
    else
        return view_->url().scheme() + "://" + view_->url().host() + view_->url().path() + url;
}

void QWebViewVisualizerSourceCommand::Download(const QString& url, QByteArray* buffer, QString* contentType) const {
    QUrl absoluteUrl = QUrl::fromEncoded(AbsoluteUrl(url).toUtf8());
    QSharedPointer<QNetworkReply> reply(view_->page()->networkAccessManager()->get(QNetworkRequest(absoluteUrl)));

    QEventLoop loop;
    QObject::connect(reply.data(), SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    *buffer = reply->readAll();
    if (reply->error() != QNetworkReply::NoError)
        session_->logger().Log(kInfoLogLevel, "[QWebViewVisualizerSourceCommand::Download] " + reply->errorString().toStdString());
    if (contentType)
        *contentType = reply->header(QNetworkRequest::ContentTypeHeader).toString();
}

QString QWebViewVisualizerSourceCommand::DownloadAndEncode(const QString& url) const {
    QByteArray file;
    QString contentType = "image";
    Download(url, &file, &contentType);

    QString base64 = file.toBase64();
    return "data:" + contentType + ";base64," + base64;
}

QString QWebViewVisualizerSourceCommand::trimmed(const QString& str, const QString& symbols) {
    int start = 0;
    while (start < str.length() && symbols.contains(str.at(start))) {
        start++;
    }

    int end = str.length() - 1;
    while (end >= 0 && symbols.contains(str.at(end))) {
        end--;
    }

    return str.mid(start, end + 1 - start);
}

void QWebViewVisualizerSourceCommand::DownloadFinished() {
}

const char QWebViewVisualizerSourceCommand::DATA_PROTOCOL[] = "data:";

class QCursorMark : public QWidget
{
public:
    explicit QCursorMark(QWidget* parent)
        : QWidget(parent)
    {
        resize(2 * RADIUS, 2 * RADIUS);
    }

    virtual void paintEvent(QPaintEvent *event) {
        QPainter painter(this);
        painter.setPen(QPen(Qt::red));

        QBrush brush = painter.brush();
        brush.setColor(Qt::red);
        brush.setStyle(Qt::SolidPattern);
        painter.setBrush(brush);

        painter.drawEllipse(QPoint(RADIUS, RADIUS), RADIUS, RADIUS);
    }

private:
    static const int RADIUS = 5;
};

QWebViewVisualizerShowPointCommand::QWebViewVisualizerShowPointCommand(QWebViewCmdExecutor* executor, Session* session, QWebView* view)
    : executor_(executor), session_(session), view_(view)
{}

void QWebViewVisualizerShowPointCommand::Execute(Error** error) {
    QList<QCursorMark*> marks = view_->findChildren<QCursorMark*>();
    QCursorMark* mark;
    if (marks.empty()) {
        mark = new QCursorMark(view_);
    } else  {
        mark = marks.front();
    }

    Point pos = session_->get_mouse_position();
    QPoint point = executor_->ConvertPointToQPoint(pos);
    mark->move(point);

    mark->show();
}

} //namespace webdriver
