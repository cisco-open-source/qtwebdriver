#ifndef WEBDRIVER_QT_WEB_VIEW_UTIL_H_
#define WEBDRIVER_QT_WEB_VIEW_UTIL_H_

#include <string>

namespace webdriver {

class QWebViewUtil {
public:
    static bool isUrlSupported(const std::string& url);

private:
	QWebViewUtil() {};
    ~QWebViewUtil(){}
};


}  // namespace webdriver

#endif  // WEBDRIVER_QT_WEB_VIEW_UTIL_H_
