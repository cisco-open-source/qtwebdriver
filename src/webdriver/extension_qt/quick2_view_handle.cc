#include "extension_qt/quick2_view_handle.h"

namespace webdriver {

QQuick2ViewHandle::QQuick2ViewHandle() 
	: view_(NULL) { }

QQuick2ViewHandle::QQuick2ViewHandle(QQuickWindow* view) 
	: view_(view) { }

bool QQuick2ViewHandle::equals(const ViewHandle* other) const {
	const QQuick2ViewHandle* toCompare = dynamic_cast<const QQuick2ViewHandle*>(other);
	if (NULL == toCompare) return false;

	return view_ == toCompare->view_;
}

} // namespace webdriver
