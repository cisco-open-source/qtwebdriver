#include "extension_qt/declarative_item_view_handle.h"

namespace webdriver {

QDeclarativeItemViewHandle::QDeclarativeItemViewHandle() 
	: view_(NULL), container_(NULL) { }

QDeclarativeItemViewHandle::QDeclarativeItemViewHandle(QDeclarativeItem* view, QDeclarativeView* container) 
	: view_(view), container_(container) { }

bool QDeclarativeItemViewHandle::equals(const ViewHandle* other) const {
	const QDeclarativeItemViewHandle* toCompare = dynamic_cast<const QDeclarativeItemViewHandle*>(other);
	if (NULL == toCompare) return false;

	return (view_ == toCompare->view_)&&(container_ == toCompare->container_);
}

} // namespace webdriver
