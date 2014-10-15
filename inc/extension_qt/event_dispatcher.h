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

#ifndef EVENTDISPATCHER_H
#define EVENTDISPATCHER_H

#include <QtCore/QEvent>

/// Abstract class which provides interface for event dispatchers
class EventDispatcher
{
public:

    /// Abstract method, should be implemented in descendants
    virtual bool dispatch(QEvent *event, bool consumed)=0;
};

#endif // EVENTDISPATCHER_H
