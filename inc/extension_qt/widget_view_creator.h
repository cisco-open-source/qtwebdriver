#ifndef WEBDRIVER_QT_WIDGET_VIEW_CREATOR_H_
#define WEBDRIVER_QT_WIDGET_VIEW_CREATOR_H_

#include <string>

#include "webdriver_view_factory.h"
#include "webdriver_logging.h"

namespace webdriver {

/// base class for custom view's creators
class QWidgetViewCreator : public ViewCreator {
public:
    QWidgetViewCreator();
    virtual ~QWidgetViewCreator(){}

    virtual bool CreateViewByClassName(const Logger& logger, const std::string& className, ViewHandle** view) const;

    virtual bool CreateViewForUrl(const Logger& logger, const std::string& url, ViewHandle** view) const;    

private:

    DISALLOW_COPY_AND_ASSIGN(QWidgetViewCreator);
};


}  // namespace webdriver

#endif  // WEBDRIVER_QT_WIDGET_VIEW_CREATOR_H_
