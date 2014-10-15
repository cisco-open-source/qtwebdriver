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

#ifndef WEBDRIVER_QWINDOW_VIEW_HANDLE_H_
#define WEBDRIVER_QWINDOW_VIEW_HANDLE_H_

#include <string>

#include "webdriver_view_id.h"

#include <QtCore/QPointer>

#include <QtCore/QtGlobal>
#include <QtGui/QWindow>

namespace webdriver {

class QWindowViewHandle : public ViewHandle {
public:
    QWindowViewHandle();
    QWindowViewHandle(QWindow* view);
    
    virtual bool is_valid() const { return !view_.isNull(); };
    virtual bool equals(const ViewHandle* other) const;
    QWindow* get() { return view_.data(); };
    
protected:
    QPointer<QWindow> view_;
//private:
	virtual ~QWindowViewHandle() {};
};

}  // namespace webdriver

#endif  // WEBDRIVER_QWINDOW_VIEW_HANDLE_H_
