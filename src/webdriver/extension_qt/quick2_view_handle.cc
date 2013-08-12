#include "extension_qt/quick2_view_handle.h"

namespace webdriver {

Quick2ViewHandle::Quick2ViewHandle() 
	: view_(NULL) { }

Quick2ViewHandle::Quick2ViewHandle(QQuickWindow* view) 
	: view_(view) { }

bool Quick2ViewHandle::equals(const ViewHandle* other) const {
	const Quick2ViewHandle* toCompare = dynamic_cast<const Quick2ViewHandle*>(other);
	if (NULL == toCompare) return false;

	return view_ == toCompare->view_;
}

} // namespace webdriver
