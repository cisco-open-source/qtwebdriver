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

#ifndef UINPUT_EVENT_DISPATCHER_H
#define UINPUT_EVENT_DISPATCHER_H

#ifdef OS_LINUX

#include "extension_qt/event_dispatcher.h"
#include "extension_qt/uinput_manager.h"

class UInputEventDispatcher : public EventDispatcher
{
public:
    /// Constructor
    /// @param manager - pointer to user events manager
    UInputEventDispatcher(UInputManager *manager);
    /// Destructor
    ~UInputEventDispatcher();

    /// Dispatch event to user input device
    /// @param event - pointer to event for dispatching
    /// @param consumed - flag whether event was consumed by previous dispatchers
    /// @return true, if event was consumed, else false
    virtual bool dispatch(QEvent *event, bool consumed);

private:
    UInputManager* _eventManager;
};

#endif // OS_LINUX

#endif // UINPUT_EVENT_DISPATCHER_H
