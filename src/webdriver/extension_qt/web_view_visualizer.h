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

#ifndef WEB_VIEW_VISUALIZER_H
#define WEB_VIEW_VISUALIZER_H

#include <QtCore/QtGlobal>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWebKitWidgets/QWebView>
#else
#include <QtWebKit/QtWebKit>
#endif

#include "base/yasper.h"
#include "qwebkit_proxy.h"

namespace webdriver {

class Error;
class Session;

class QWebViewVisualizerSourceCommand : public QObject {
    Q_OBJECT

public:
    QWebViewVisualizerSourceCommand(yasper::ptr<QWebkitProxy> webkitProxy, Session* session, QWebView* view);

    void Execute(std::string* source, Error** error);

private:
    static void UnescapeXml(QString& input);

    void AssemblePage(QWebElement element) const;
    void AssembleLink(QWebElement element) const;
    void AssembleImg(QWebElement element) const;
    void AssembleStyle(QWebElement element) const;
    QString AssembleStyle(const QString& value) const;
    void RemoveScripts(QWebElement element) const;

    QString AbsoluteUrl(const QString& url) const;
    void Download(const QString& url, QByteArray* buffer, QString* contentType) const;
    QString DownloadAndEncode(const QString& url) const;

private Q_SLOTS:
    void DownloadFinished();

private:
    static const char DATA_PROTOCOL[];

    yasper::ptr<QWebkitProxy> webkitProxy_;
    Session* session_;
    QWebView* view_;
};

class QWebViewVisualizerShowPointCommand {
public:
    QWebViewVisualizerShowPointCommand(yasper::ptr<QWebkitProxy> webkitProxy, Session* session, QWebView* view);

    void Execute(Error** error);

private:
    yasper::ptr<QWebkitProxy> webkitProxy_;
    Session* session_;
    QWebView* view_;
};

}  // namespace webdriver

#endif // WEB_VIEW_VISUALIZER_H
