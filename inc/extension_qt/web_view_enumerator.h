#ifndef WEBDRIVER_WEB_VIEW_ENUMERATOR_H_
#define WEBDRIVER_WEB_VIEW_ENUMERATOR_H_

#include <string>
#include <vector>

#include "webdriver_view_enumerator.h"


namespace webdriver {

/// class for QWebView enumerator
class WebViewEnumeratorImpl : public AbstractViewEnumeratorImpl {
public:
    virtual void EnumerateViews(Session* session) const;
};

}  // namespace webdriver

#endif  // WEBDRIVER_WEB_VIEW_ENUMERATOR_H_
