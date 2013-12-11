#ifndef WEBDRIVER_QT_QML_VIEW_CREATOR_H_
#define WEBDRIVER_QT_QML_VIEW_CREATOR_H_

#include <string>

#include "webdriver_view_factory.h"

namespace webdriver {

class Session;

/// base class for custom view's creators
class QQmlViewCreator : public ViewCreator {
public:
    QQmlViewCreator();
    virtual ~QQmlViewCreator(){}

    virtual bool CreateViewByClassName(const Logger& logger, const std::string& className,
                                       const Point* position, const Size* size, ViewHandle** view) const;

    virtual bool CreateViewForUrl(const Logger& logger, const std::string& url,
                                  const Point* position, const Size* size, ViewHandle** view) const;

private:

    DISALLOW_COPY_AND_ASSIGN(QQmlViewCreator);
};


}  // namespace webdriver

#endif  // WEBDRIVER_QT_QML_VIEW_CREATOR_H_
