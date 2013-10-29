#include "extension_qt/widget_element_handle.h"

namespace webdriver {

QElementHandle::QElementHandle()
	: element_(NULL) {}

QElementHandle::QElementHandle(QObject* element)
	: element_(element) {}

bool QElementHandle::equals(const ElementHandle* other) const {
	const QElementHandle* toCompare = dynamic_cast<const QElementHandle*>(other);
	if (NULL == toCompare) return false;

	return element_ == toCompare->element_;	
}	
	

} // namespace webdriver
