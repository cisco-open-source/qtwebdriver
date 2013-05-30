#ifndef WEBDRIVER_QT_WEB_VIEW_CREATOR_H_
#define WEBDRIVER_QT_WEB_VIEW_CREATOR_H_

#include <string>

#include "webdriver_view_factory.h"

namespace webdriver {

class Session;

/// base class for custom view's creators
class QWebViewCreator : public ViewCreator {
public:
    QWebViewCreator();
    virtual ~QWebViewCreator(){}

    virtual bool CreateViewByClassName(Session* session, const std::string& className, ViewId* viewId) const;

    virtual bool CreateViewForUrl(Session* session, const std::string& url, ViewId* viewId) const;    

private:

    DISALLOW_COPY_AND_ASSIGN(QWebViewCreator);
};


}  // namespace webdriver

#endif  // WEBDRIVER_QT_WEB_VIEW_CREATOR_H_
