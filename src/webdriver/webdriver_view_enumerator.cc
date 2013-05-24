#include "webdriver_view_enumerator.h"

#include "webdriver_session.h"


namespace webdriver {

std::vector<ViewEnumeratorImplPtr> ViewEnumerator::enumerators_;

void ViewEnumerator::EnumerateViews(Session* session, std::vector<ViewId>* views) {
	std::vector<ViewEnumeratorImplPtr>::const_iterator impl;

	for (impl = enumerators_.begin(); impl < enumerators_.end(); ++impl) {
		(*impl)->EnumerateViews(session);
	}

	if (NULL != views) {
		session->GetViews(views);
	}
}

void ViewEnumerator::AddViewEnumeratorImpl(AbstractViewEnumeratorImpl* enumeratorImpl) {
	if (NULL != enumeratorImpl) {
		enumerators_.push_back(enumeratorImpl);
	}
}

} // namespace webdriver