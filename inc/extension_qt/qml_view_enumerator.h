#ifndef WEBDRIVER_QML_VIEW_ENUMERATOR_H_
#define WEBDRIVER_QML_VIEW_ENUMERATOR_H_

#include <string>
#include <vector>
#include <set>

#include "webdriver_view_enumerator.h"

namespace webdriver {

/// class for QWebView enumerator
class QmlViewEnumeratorImpl : public AbstractViewEnumeratorImpl {
public:
	virtual void EnumerateViews(Session* session, std::set<ViewId>* views) const;
};

}  // namespace webdriver

#endif  // WEBDRIVER_QML_VIEW_ENUMERATOR_H_
