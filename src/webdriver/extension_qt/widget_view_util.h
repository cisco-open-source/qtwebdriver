#ifndef WEBDRIVER_QT_WIDGET_VIEW_UTIL_H_
#define WEBDRIVER_QT_WIDGET_VIEW_UTIL_H_

#include <string>

#include <QtCore/QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets/QWidget>
#else
#include <QtGui/QWidget>
#endif

#include "common_util.h"
#include "webdriver_view_id.h"

namespace webdriver {

class Session;	

class QWidgetViewUtil {
public:
    static bool isUrlSupported(const std::string& url);
    static std::string extractClassName(const std::string& url);
    static std::string makeUrlByClassName(const std::string& className);
    static QWidget* getView(Session* session, const ViewId& viewId);

private:
	static const char url_protocol[];
    QWidgetViewUtil() {}
    ~QWidgetViewUtil() {}
};

class QWidgetXmlSerializer : public QViewXmlSerializer<QObject> {
public:
    QWidgetXmlSerializer(QIODevice* buff)
        : QViewXmlSerializer<QObject>(buff)
    {}

    void setViewId(ViewId viewId) {
        viewId_ = viewId;
    }

    void setSupportedClasses(const QStringList& classes) {
        supportedClasses_ = classes;
    }

private:
    virtual void addWidget(QObject* widget);
    QString getElementName(const QObject* object) const;

    ViewId viewId_;
    QStringList supportedClasses_;
};

}  // namespace webdriver

#endif  // WEBDRIVER_QT_WIDGET_VIEW_UTIL_H_
