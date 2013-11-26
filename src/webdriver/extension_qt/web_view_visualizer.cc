#include "web_view_visualizer.h"

#include <buffio.h>
#include <tidy.h>
#include <stdexcept>
#include <QtCore/QEventLoop>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include "base/stringprintf.h"
#include "extension_qt/web_view_executor.h"
#include "webdriver_session.h"
#include "webdriver_util.h"
#include "common_util.h"

namespace webdriver {

QWebViewVisualizerSourceCommand::QWebViewVisualizerSourceCommand(QWebViewCmdExecutor* executor, Session* session, QWebView* view)
    : executor_(executor), session_(session), view_(view)
{}

void QWebViewVisualizerSourceCommand::Execute(std::string* source, Error** error) {
    // Convert DOM tree to valid XML.
    const char* kSource =
        "var elements = document.getElementsByTagName('*');\n"
        "for (var index = 0; index < elements.length; index++) {\n"
        "  var element = elements[index];\n"
        "  element.setAttribute('data-viz-id', index);\n"
        "}\n"
        "var root = document.documentElement.cloneNode(true);\n"
        "var elements = root.getElementsByTagName('script');\n"
        "for (var index = 0; index < elements.length; index++) {\n"
        "  var element = elements[index];\n"
        "  element.innerHTML = '';\n"
        "}\n"
        "\n"
        "return new XMLSerializer().serializeToString(root);";

    Value* unscoped_value = NULL;
    executor_->ExecuteScript(
                kSource,
                new ListValue(),
                &unscoped_value,
                error);
    if (*error) {
        (*error)->AddDetails("getSource execution failed");
        return;
    }

    scoped_ptr<Value> value(unscoped_value);
    const ValueParser* parser = CreateDirectValueParser(source);
    if (!parser->Parse(value.get())) {
        std::string error_msg = base::StringPrintf("getSource returned invalid value: %s",
            JsonStringify(value.get()).c_str());
        *error = new Error(kUnknownError, error_msg);
        return;
    }

    session_->logger().Log(kInfoLogLevel, "[QWebViewVisualizerSourceCommand] before transform:");
    session_->logger().Log(kInfoLogLevel, *source);
    *source = Tidy(*source, error);
    if (*error)
        return;

    QSharedPointer<pugi::xml_document> document = ParseXml(QString::fromStdString(*source), error);
    if (!document)
        return;

    pugi::xml_node documentElement = document->document_element();
    AssemblePage(documentElement);

    std::stringstream sourceStream;
    document->save(sourceStream);
    *source = sourceStream.str();

    session_->logger().Log(kInfoLogLevel, "[QWebViewVisualizerSourceCommand] after transform:");
    session_->logger().Log(kInfoLogLevel, *source);
}

QSharedPointer<pugi::xml_document> QWebViewVisualizerSourceCommand::ParseXml(const QString& input, Error** error) {
    if (*error) throw std::invalid_argument("error");

    QSharedPointer<pugi::xml_document> document(new pugi::xml_document());
    QByteArray buffer = input.toUtf8();
    pugi::xml_parse_result result = document->load_buffer(buffer.data(), buffer.size(), pugi::parse_default, pugi::encoding_utf8);

    if (!result)
    {
        std::string details = base::StringPrintf(
            "pugi::xml_document::load_buffer failed with error message '%s' at offset %d",
            result.description(), result.offset);
        session_->logger().Log(kSevereLogLevel, details);
        *error = new Error(kInternalServerError, details);
        return QSharedPointer<pugi::xml_document>();
    }

    return document;
}

std::string QWebViewVisualizerSourceCommand::Tidy(const std::string& input, Error** error) const {
    static const bool SHOW_DIAGNOSTICS = false;
    TidyBuffer outbuf;
    TidyBuffer errbuf;
    int retval = -1;

    TidyDoc doc = tidyCreate();
    tidyBufInit(&outbuf);
    tidyBufInit(&errbuf);

    tidyOptSetBool(doc, TidyXhtmlOut, yes);
    tidyOptSetBool(doc, TidyForceOutput, yes);
    tidySetErrorBuffer(doc, &errbuf);

    retval = tidyParseString(doc, input.c_str());
    if (retval >= 0) {
        retval = tidyCleanAndRepair(doc);
        if (retval >= 0) {
            if (SHOW_DIAGNOSTICS) {
                retval = tidyRunDiagnostics(doc);
                if (retval >= 0) {
                    std::string diagnostics(reinterpret_cast<const char*>(errbuf.bp), errbuf.size);
                    session_->logger().Log(kWarningLogLevel, diagnostics);
                } else {
                    std::string details = base::StringPrintf(
                        "tidyRunDiagnostics failed with retval %d", retval);
                    session_->logger().Log(kSevereLogLevel, details);
                    *error = new Error(kInternalServerError, details);
                }
            }

            retval = tidySaveBuffer(doc, &outbuf);
            if (retval < 0) {
                std::string details = base::StringPrintf(
                    "tidySaveBuffer failed with retval %d", retval);
                session_->logger().Log(kSevereLogLevel, details);
                *error = new Error(kInternalServerError, details);
            }
        } else {
            std::string details = base::StringPrintf(
                "tidyCleanAndRepair failed with retval %d", retval);
            session_->logger().Log(kSevereLogLevel, details);
            *error = new Error(kInternalServerError, details);
        }
    } else {
        std::string details = base::StringPrintf(
            "tidyParseString failed with retval %d", retval);
        session_->logger().Log(kSevereLogLevel, details);
        *error = new Error(kInternalServerError, details);
    }

    std::string output(reinterpret_cast<const char*>(outbuf.bp), outbuf.size);
    tidyBufFree(&outbuf);
    tidyBufFree(&errbuf);
    tidyRelease(doc);
    return output;
}

void QWebViewVisualizerSourceCommand::UnescapeXml(QString& input) {
    input.replace("&amp;", "&");
}

void QWebViewVisualizerSourceCommand::AssemblePage(pugi::xml_node element) const {
    if (std::string(element.name()) == std::string("img")) {
        AssembleImg(element);
    }
    if (std::string(element.name()) == std::string("link")) {
        AssembleLink(element);
    }
    if (std::string(element.name()) == std::string("style")) {
        AssembleStyle(element);
    }

    if (!element.attribute("style").empty()) {
        AssembleStyle(element.attribute("style"));
    }

    RemoveScripts(element);

    // Chrome does like empty tags like <textarea/>
    if (std::string(element.name()) != std::string("br") &&
        isEmpty(element.children())) {
        element.append_child(pugi::node_pcdata);
    }

    // Recursively walk DOM tree
    for (pugi::xml_node_iterator childIt = element.begin(); childIt != element.end(); childIt++) {
        if (childIt->type() == pugi::node_element)
            AssemblePage(*childIt);
    }
}

void QWebViewVisualizerSourceCommand::AssembleLink(pugi::xml_node element) const {
    QString rel = QString::fromUtf8(element.attribute("rel").value());
    QString type = QString::fromUtf8(element.attribute("type").value());
    if (rel != "stylesheet" && type != "text/css")
        return;

    QString href = QString::fromUtf8(element.attribute("href").value());
    UnescapeXml(href);
    QString url = AbsoluteUrl(href);
    QByteArray file;
    QString contentType;
    Download(url, &file, &contentType);

    pugi::xml_node parent = element.parent();
    parent.remove_child(element);

    if (!file.isEmpty()) {
        pugi::xml_node style = parent.append_child("style");
        if (!type.isEmpty()) {
            style.append_attribute("type").set_value(type.toUtf8().data());
        } else if (!contentType.isEmpty()) {
            QString type = contentType.split(';')[0];
            style.append_attribute("type").set_value(type.toUtf8().data());
        }
        style.append_child(pugi::node_pcdata).set_value(file.data());
    }
}

// Convert <img> tag 'src' attribute to base64
void QWebViewVisualizerSourceCommand::AssembleImg(pugi::xml_node element) const {
    QString url = QString::fromUtf8(element.attribute("src").value());
    if (!url.startsWith(DATA_PROTOCOL)) {
        QString value = DownloadAndEncode(url);
        element.attribute("src").set_value(value.toUtf8().data());
    }

    if (element.attribute("srcset"))
        element.remove_attribute("srcset");
}

void QWebViewVisualizerSourceCommand::AssembleStyle(pugi::xml_node element) const {
    pugi::xml_attribute type = element.attribute("type");
    if (!type.empty() &&
        std::string(type.value()) != "text/css") {
        return;
    }

    if (len(element.children()) == 1 &&
        element.first_child().type() == pugi::node_cdata) {
        pugi::xml_node cdata = element.first_child();
        element.remove_child(cdata);
        QString value = QString::fromUtf8(cdata.value());
        value = AssembleStyle(value);
        element.append_child(pugi::node_pcdata).set_value(value.toStdString().c_str());
    } else {
        for (pugi::xml_node_iterator childIt = element.begin(); childIt != element.end(); childIt++) {
            if (childIt->type() == pugi::node_pcdata) {
                QString value = QString::fromUtf8(childIt->value());
                value = AssembleStyle(value);
                childIt->set_value(value.toStdString().c_str());
            }
        }
    }
}

void QWebViewVisualizerSourceCommand::AssembleStyle(pugi::xml_attribute attribute) const {
    QString value = attribute.value();
    value = AssembleStyle(value);
    attribute.set_value(value.toUtf8().data());
}

QString QWebViewVisualizerSourceCommand::AssembleStyle(const QString& value) const {
    QRegExp regex(":\\s*url\\(([^\\)]+)\\)");
    QString result;

    int lastMatchEnd = 0;
    int newMatchStart;
    while ((newMatchStart = regex.indexIn(value, lastMatchEnd)) != -1) {
        result += value.mid(lastMatchEnd, newMatchStart - lastMatchEnd);

        QString url = regex.cap(1);
        url = StringUtil::trimmed(url, " \t'\"");

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
void QWebViewVisualizerSourceCommand::RemoveScripts(pugi::xml_node element) const {
    std::vector<pugi::xml_node> scripts;

    for (pugi::xml_node_iterator childIt = element.begin(); childIt != element.end(); childIt++) {
        if (childIt->type() == pugi::node_element &&
            std::string(childIt->name()) == "script") {
            scripts.push_back(*childIt);
        }
    }

    for (std::vector<pugi::xml_node>::const_iterator it = scripts.begin(); it != scripts.end(); it++) {
        element.remove_child(*it);
    }
}

QString QWebViewVisualizerSourceCommand::AbsoluteUrl(const QString& url) const {
    if (url.contains("://"))
        return url;

    QString absoluteUrl;
    if (url.startsWith("//")) {
        absoluteUrl += view_->url().scheme();
        absoluteUrl += ":";
        absoluteUrl += url;
    } else if (url.startsWith('/')) {
        absoluteUrl += view_->url().scheme();
        absoluteUrl += "://";
        absoluteUrl += view_->url().host();
        if (view_->url().port() != -1) {
            absoluteUrl += ":";
            absoluteUrl += QString::number(view_->url().port());
        }
        absoluteUrl += url;
    } else {
        absoluteUrl += view_->url().scheme();
        absoluteUrl += "://";
        absoluteUrl += view_->url().host();
        if (view_->url().port() != -1) {
            absoluteUrl += ":";
            absoluteUrl += QString::number(view_->url().port());
        }
        absoluteUrl += StringUtil::substringBeforeLast(view_->url().path(), "/");
        absoluteUrl += "/";
        absoluteUrl += url;
    }
    return absoluteUrl;
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

void QWebViewVisualizerSourceCommand::DownloadFinished() {
}

bool QWebViewVisualizerSourceCommand::isEmpty(const pugi::xml_object_range<pugi::xml_node_iterator>& range) {
    return range.begin() == range.end();
}

int QWebViewVisualizerSourceCommand::len(const pugi::xml_object_range<pugi::xml_node_iterator>& range) {
    return std::distance(range.begin(), range.end());
}

void QWebViewVisualizerSourceCommand::clearChildren(pugi::xml_node element) {
    std::vector<pugi::xml_node> children;
    for (pugi::xml_node_iterator it = element.children().begin(); it != element.children().end(); it++) {
        children.push_back(*it);
    }
    for (std::vector<pugi::xml_node>::iterator it = children.begin(); it != children.end(); it++) {
        element.remove_child(*it);
    }
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
    QPoint point = QCommonUtil::ConvertPointToQPoint(pos);
    mark->move(point);

    mark->show();
}

} //namespace webdriver
