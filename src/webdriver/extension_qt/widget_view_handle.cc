#include "extension_qt/widget_view_handle.h"

namespace webdriver {

QViewHandle::QViewHandle() 
	: view_(NULL) { }

QViewHandle::QViewHandle(QWidget* view) 
	: view_(view) { }

bool QViewHandle::equals(const ViewHandle* other) const {
	const QViewHandle* toCompare = dynamic_cast<const QViewHandle*>(other);
	if (NULL == toCompare) return false;

	return view_ == toCompare->view_;
}

} // namespace webdriver
