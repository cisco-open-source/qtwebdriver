#ifndef WEBDRIVER_QT_WEB_VIEW_UTIL_H_
#define WEBDRIVER_QT_WEB_VIEW_UTIL_H_

#include <string>

#include <QtWebKit/QtWebKit>

namespace webdriver {

class Session;
class ViewId;


class QWebViewUtil {
public:
    static bool isUrlSupported(const std::string& url);
    static QWebView* getWebView(Session* session, const ViewId& viewId);

private:
	QWebViewUtil() {};
    ~QWebViewUtil(){}
};


}  // namespace webdriver

#endif  // WEBDRIVER_QT_WEB_VIEW_UTIL_H_
