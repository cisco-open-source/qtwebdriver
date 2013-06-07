#ifndef WEBDRIVER_QT_WIDGET_VIEW_UTIL_H_
#define WEBDRIVER_QT_WIDGET_VIEW_UTIL_H_

#include <string>

namespace webdriver {

class QWidgetViewUtil {
public:
    static bool isUrlSupported(const std::string& url);

private:
	QWidgetViewUtil() {};
    ~QWidgetViewUtil(){}
};


}  // namespace webdriver

#endif  // WEBDRIVER_QT_WIDGET_VIEW_UTIL_H_
