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

#ifndef WEBDRIVER_QT_DECLARATIVE_ITEM_VIEW_H_
#define WEBDRIVER_QT_DECLARATIVE_ITEM_VIEW_H_

#include <string>

#include "webdriver_view_id.h"

#include <QtDeclarative/QDeclarativeItem>
#include <QtDeclarative/QDeclarativeView>
#include <QtCore/QPointer>
#include <QtCore/QtGlobal>

namespace webdriver {

class QDeclarativeItemViewHandle : public ViewHandle {
public:
    QDeclarativeItemViewHandle();
    QDeclarativeItemViewHandle(QDeclarativeItem* view, QDeclarativeView* container);
    
    virtual bool is_valid() const { return !view_.isNull(); };
    virtual bool equals(const ViewHandle* other) const;
    QDeclarativeItem* get() { return view_.data(); };
    QDeclarativeView* getContainer() { return container_.data(); };
    
protected:
    QPointer<QDeclarativeItem> view_;
    QPointer<QDeclarativeView> container_;
//private:
	virtual ~QDeclarativeItemViewHandle() {};
};

}  // namespace webdriver

#endif  // WEBDRIVER_QT_DECLARATIVE_ITEM_VIEW_H_
