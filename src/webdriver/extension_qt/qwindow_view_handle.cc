#include "extension_qt/qwindow_view_handle.h"

namespace webdriver {

QWindowViewHandle::QWindowViewHandle() 
	: view_(NULL) { }

QWindowViewHandle::QWindowViewHandle(QWindow* view) 
	: view_(view) { }

bool QWindowViewHandle::equals(const ViewHandle* other) const {
	const QWindowViewHandle* toCompare = dynamic_cast<const QWindowViewHandle*>(other);
	if (NULL == toCompare) return false;

	return view_ == toCompare->view_;
}

} // namespace webdriver
