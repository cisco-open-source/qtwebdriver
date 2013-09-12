#include "extension_qt/declarative_item_view_handle.h"

namespace webdriver {

QDeclarativeItemViewHandle::QDeclarativeItemViewHandle() 
	: view_((QDeclarativeItem*)NULL) { }

QDeclarativeItemViewHandle::QDeclarativeItemViewHandle(QDeclarativeItem* view) 
	: view_(view) { }

bool QDeclarativeItemViewHandle::equals(const ViewHandle* other) const {
	const QDeclarativeItemViewHandle* toCompare = dynamic_cast<const QDeclarativeItemViewHandle*>(other);
	if (NULL == toCompare) return false;

	return view_ == toCompare->view_;
}

} // namespace webdriver
