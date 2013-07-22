#ifndef WEBDRIVER_QT_WEB_VIEW_UTIL_H_
#define WEBDRIVER_QT_WEB_VIEW_UTIL_H_

#include <string>

#include <QtCore/QDebug>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWebKitWidgets/QWebView>
#else
#include <QtWebKit/QtWebKit>
#endif

namespace webdriver {

class Session;
class ViewId;
class Error;

class QWebViewUtil {
public:
    static bool isUrlSupported(QWebView* pWebView, const std::string& url, Error **error);
    static bool isUrlSupported(const std::string& url, Error **error);
    static QWebView* getWebView(Session* session, const ViewId& viewId);

private:
    QWebViewUtil() {}
    ~QWebViewUtil(){}
};


}  // namespace webdriver

#endif  // WEBDRIVER_QT_WEB_VIEW_UTIL_H_
