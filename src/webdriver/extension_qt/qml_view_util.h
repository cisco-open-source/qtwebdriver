#ifndef WEBDRIVER_QT_QML_VIEW_UTIL_H_
#define WEBDRIVER_QT_QML_VIEW_UTIL_H_

#include <string>

#include <QtCore/QDebug>

#include "common_util.h"

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
class QWindow;
class QQuickView;
#else
class QDeclarativeView;
#endif
class QDeclarativeItem;

namespace webdriver {

class Session;
class ViewId;
class Error;

class QQmlViewUtil {
public:
    static bool isUrlSupported(const std::string& url);
    static bool isContentTypeSupported(const std::string& mime);
    static void removeInternalSuffixes(QString& str);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    static QWindow* getQWindowView(Session* session, const ViewId& viewId);
    static QQuickView* getQMLView(Session* session, const ViewId& viewId);
#else    
    static QDeclarativeView* getQMLView(Session* session, const ViewId& viewId);
#endif
};

#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
class QQmlXmlSerializer : public QViewXmlSerializer<QDeclarativeItem>
{
public:
    QQmlXmlSerializer(QIODevice* buff)
        : QViewXmlSerializer<QDeclarativeItem>(buff)
    {}

protected:
    virtual void addWidget(QDeclarativeItem* item);
};
#endif

}  // namespace webdriver

#endif  // WEBDRIVER_QT_QML_VIEW_UTIL_H_
