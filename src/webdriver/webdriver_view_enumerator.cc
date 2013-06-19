#include "webdriver_view_enumerator.h"

#include "webdriver_session.h"
#include "base/stringprintf.h"


namespace webdriver {

std::vector<ViewEnumeratorImplPtr> ViewEnumerator::enumerators_;

void ViewEnumerator::EnumerateViews(Session* session, std::vector<ViewId>* views) {
	std::vector<ViewEnumeratorImplPtr>::const_iterator impl;
	std::set<ViewId> views_set;

	for (impl = enumerators_.begin(); impl < enumerators_.end(); ++impl) {
		(*impl)->EnumerateViews(session, &views_set);
	}

	std::copy(views_set.begin(), views_set.end(), std::back_inserter(*views));

	// update session to remove dangling sessionIds
	session->logger().Log(kFineLogLevel, base::StringPrintf("EnumerateViews found %d views.", views_set.size()));
	session->UpdateViews(views_set);
}

void ViewEnumerator::AddViewEnumeratorImpl(AbstractViewEnumeratorImpl* enumeratorImpl) {
	if (NULL != enumeratorImpl) {
		enumerators_.push_back(enumeratorImpl);
	}
}

} // namespace webdriver