#ifndef WEBDRIVER_QT_WEB_VIEW_CREATOR_H_
#define WEBDRIVER_QT_WEB_VIEW_CREATOR_H_

#include <string>

#include "webdriver_view_factory.h"

namespace webdriver {

class Session;
class ViewHandle;

/// base class for custom view's creators
class QWebViewCreator : public ViewCreator {
public:
    QWebViewCreator();
    virtual ~QWebViewCreator(){}

    virtual bool CreateViewByClassName(const Logger& logger, const std::string& className,
                                       const Point* position, const Size* size, ViewHandle** view) const;

    virtual bool CreateViewForUrl(const Logger& logger, const std::string& url,
                                  const Point* position, const Size* size, ViewHandle** view) const;

private:
	bool ShowView(const Logger& logger, ViewHandle* viewHandle) const;

    DISALLOW_COPY_AND_ASSIGN(QWebViewCreator);
};


}  // namespace webdriver

#endif  // WEBDRIVER_QT_WEB_VIEW_CREATOR_H_
