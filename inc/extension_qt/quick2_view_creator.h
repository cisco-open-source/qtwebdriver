#ifndef WEBDRIVER_QUICK2_VIEW_CREATOR_H_
#define WEBDRIVER_QUICK2_VIEW_CREATOR_H_

#include <string>

#include "webdriver_view_factory.h"

namespace webdriver {

class Session;

class Quick2ViewCreator : public ViewCreator {
public:
    Quick2ViewCreator();
    virtual ~Quick2ViewCreator(){}

    virtual bool CreateViewByClassName(const Logger& logger, const std::string& className, ViewHandle** view) const;

    virtual bool CreateViewForUrl(const Logger& logger, const std::string& url, ViewHandle** view) const;    

private:

    DISALLOW_COPY_AND_ASSIGN(Quick2ViewCreator);
};


}  // namespace webdriver

#endif  // WEBDRIVER_QUICK2_VIEW_CREATOR_H_
