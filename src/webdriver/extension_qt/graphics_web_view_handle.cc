#include "extension_qt/graphics_web_view_handle.h"

namespace webdriver {

GraphicsWebViewHandle::GraphicsWebViewHandle() 
	: view_(NULL) { }

GraphicsWebViewHandle::GraphicsWebViewHandle(QGraphicsWebView* view) 
	: view_(view) { }

bool GraphicsWebViewHandle::equals(const ViewHandle* other) const {
	const GraphicsWebViewHandle* toCompare = dynamic_cast<const GraphicsWebViewHandle*>(other);
	if (NULL == toCompare) return false;

	return view_ == toCompare->view_;
}

} // namespace webdriver
