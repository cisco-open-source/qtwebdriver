/****************************************************************************
**
** Copyright © 1992-2014 Cisco and/or its affiliates. All rights reserved.
** All rights reserved.
** 
** $CISCO_BEGIN_LICENSE:LGPL$
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** $CISCO_END_LICENSE$
**
****************************************************************************/

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
