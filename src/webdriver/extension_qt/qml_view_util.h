#ifndef WEBDRIVER_QT_QML_VIEW_UTIL_H_
#define WEBDRIVER_QT_QML_VIEW_UTIL_H_

#include <string>

#include <QtCore/QDebug>

class QDeclarativeView;

namespace webdriver {

class Session;
class ViewId;
class Error;

class QQmlViewUtil {
public:
    static bool isUrlSupported(const std::string& url);
    static bool isContentTypeSupported(const std::string& mime);
    static QDeclarativeView* getQMLView(Session* session, const ViewId& viewId);

private:
    QQmlViewUtil() {}
    ~QQmlViewUtil(){}
};


}  // namespace webdriver

#endif  // WEBDRIVER_QT_QML_VIEW_UTIL_H_
