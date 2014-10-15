/****************************************************************************
**
** Copyright © 1992-2014 Cisco and/or its affiliates. All rights reserved.
** All rights reserved.
** 
** $CISCO_BEGIN_LICENSE:LGPL$
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** $CISCO_END_LICENSE$
**
****************************************************************************/

#include "web_view_visualizer.h"

#include <stdexcept>
#include <QtCore/QEventLoop>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtWebKit/QWebElement>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWebKitWidgets/QWebFrame>
#else
#include <QtWebKit/QWebFrame>
#endif

#include "base/stringprintf.h"
#include "extension_qt/web_view_executor.h"
#include "webdriver_session.h"
#include "webdriver_util.h"
#include "common_util.h"

namespace webdriver {

QWebViewVisualizerSourceCommand::QWebViewVisualizerSourceCommand(yasper::ptr<QWebkitProxy> webkitProxy, Session* session, QWebView* view)
    : webkitProxy_(webkitProxy), session_(session), view_(view)
{}

void QWebViewVisualizerSourceCommand::Execute(std::string* source, Error** error) {
    QWebElement documentElement = webkitProxy_->GetFrame(session_->current_frame())->documentElement().clone();

    AssemblePage(documentElement);

    *source = documentElement.toOuterXml().toStdString();

    session_->logger().Log(kInfoLogLevel, "[QWebViewVisualizerSourceCommand] result:");
    session_->logger().Log(kInfoLogLevel, *source);
}

void QWebViewVisualizerSourceCommand::UnescapeXml(QString& input) {
    input.replace("&amp;", "&");
}

void QWebViewVisualizerSourceCommand::AssemblePage(QWebElement element) const {
    if (element.tagName().toLower() == QString("img")) {
        AssembleImg(element);
    }
    if (element.tagName().toLower() == QString("link")) {
        AssembleLink(element);
    }
    if (element.tagName().toLower() == QString("style")) {
        AssembleStyle(element);
    }

    if (!element.attribute("style").isEmpty()) {
        QString style = AssembleStyle(element.attribute("style"));
        element.setAttribute("style", style);
    }

    RemoveScripts(element);

    // Recursively walk DOM tree
    for (QWebElement child = element.firstChild(); !child.isNull(); child = child.nextSibling()) {
        AssemblePage(child);
    }
}

void QWebViewVisualizerSourceCommand::AssembleLink(QWebElement element) const {
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

    if (type.isEmpty() && !contentType.isEmpty()) {
        type = contentType.split(';')[0];
    }

    if (!file.isEmpty()) {
        QString style = "<style";
        if (!type.isEmpty()) {
            style += " type=\"" + type + "\"";
        }
        style += ">";
        style += file.data();
        style += "</style>";
        element.setOuterXml(style);
    } else {
        element.removeFromDocument();
    }
}

// Convert <img> tag 'src' attribute to base64
void QWebViewVisualizerSourceCommand::AssembleImg(QWebElement element) const {
    QString url = element.attribute("src");
    if (!url.startsWith(DATA_PROTOCOL)) {
        QString value = DownloadAndEncode(url);
        element.setAttribute("src", value);
    }

    if (!element.attribute("srcset").isNull())
        element.removeAttribute("srcset");
}

void QWebViewVisualizerSourceCommand::AssembleStyle(QWebElement element) const {
    QString type = element.attribute("type");
    if (type != "" &&
        type != "text/css") {
        return;
    }

    QString value = element.toInnerXml();
    value = AssembleStyle(value);
    element.setInnerXml(value);
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
void QWebViewVisualizerSourceCommand::RemoveScripts(QWebElement element) const {
    for (QWebElement child = element.firstChild(); !child.isNull(); child = child.nextSibling()) {
        if (child.tagName().toLower() == "script") {
            child.setInnerXml("");
            child.removeAttribute("src");
        }
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

QWebViewVisualizerShowPointCommand::QWebViewVisualizerShowPointCommand(yasper::ptr<QWebkitProxy> webkitProxy, Session* session, QWebView* view)
    : webkitProxy_(webkitProxy), session_(session), view_(view)
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
