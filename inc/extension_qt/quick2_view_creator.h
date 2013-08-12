#ifndef WEBDRIVER_QUICK2_VIEW_CREATOR_H_
#define WEBDRIVER_QUICK2_VIEW_CREATOR_H_

#include <string>

#include "webdriver_view_factory.h"

namespace webdriver {

class Session;

class Quick2QmlViewCreator : public ViewCreator {
public:
    Quick2QmlViewCreator();
    virtual ~Quick2QmlViewCreator(){}

    virtual bool CreateViewByClassName(const Logger& logger, const std::string& className, ViewHandle** view) const;

    virtual bool CreateViewForUrl(const Logger& logger, const std::string& url, ViewHandle** view) const;    

private:

    DISALLOW_COPY_AND_ASSIGN(Quick2QmlViewCreator);
};


}  // namespace webdriver

#endif  // WEBDRIVER_QUICK2_VIEW_CREATOR_H_
