#ifndef WEBDRIVER_QML_WEB_VIEW_ENUMERATOR_H_
#define WEBDRIVER_QML_WEB_VIEW_ENUMERATOR_H_

#include <string>
#include <vector>
#include <set>

#include "webdriver_view_enumerator.h"

class QDeclarativeItem;

namespace webdriver {

/// class for QDeclarativeWebView enumerator
class QmlWebViewEnumeratorImpl : public AbstractViewEnumeratorImpl {
public:
	virtual void EnumerateViews(Session* session, std::set<ViewId>* views) const;
private:
	bool isWebView(QDeclarativeItem* item) const;
};

}  // namespace webdriver

#endif  // WEBDRIVER_QML_WEB_VIEW_ENUMERATOR_H_
