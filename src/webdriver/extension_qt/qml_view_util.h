#ifndef WEBDRIVER_QT_QML_VIEW_UTIL_H_
#define WEBDRIVER_QT_QML_VIEW_UTIL_H_

#include <string>

#include <QtCore/QDebug>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
class QWindow;
class QQuickWindow;
#else
class QDeclarativeView;
#endif

namespace webdriver {

class Session;
class ViewId;
class Error;

class QQmlViewUtil {
public:
    static bool isUrlSupported(const std::string& url);
    static bool isContentTypeSupported(const std::string& mime);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    static QWindow* getQWindowView(Session* session, const ViewId& viewId);
    static QQuickWindow* getQMLView(Session* session, const ViewId& viewId);
#else    
    static QDeclarativeView* getQMLView(Session* session, const ViewId& viewId);
#endif    

private:
    QQmlViewUtil() {}
    ~QQmlViewUtil(){}
};


}  // namespace webdriver

#endif  // WEBDRIVER_QT_QML_VIEW_UTIL_H_
