#ifndef WEBDRIVER_QT_WIDGET_VIEW_UTIL_H_
#define WEBDRIVER_QT_WIDGET_VIEW_UTIL_H_

#include <string>

#include <QtGui/QWidget>

namespace webdriver {

class Session;	
class ViewId;

class QWidgetViewUtil {
public:
    static bool isUrlSupported(const std::string& url);
    static std::string extractClassName(const std::string& url);
    static QWidget* getView(Session* session, const ViewId& viewId);

private:
	static const char url_protocol[];
	QWidgetViewUtil() {};
    ~QWidgetViewUtil(){}
};


}  // namespace webdriver

#endif  // WEBDRIVER_QT_WIDGET_VIEW_UTIL_H_
