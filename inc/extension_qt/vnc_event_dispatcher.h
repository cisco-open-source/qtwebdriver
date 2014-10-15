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

#ifndef VNCEVENTDISPATCHER_H
#define VNCEVENTDISPATCHER_H

#include "event_dispatcher.h"
#include "extension_qt/vncclient.h"

/// This class dispatch events to VNC module.
class VNCEventDispatcher : public EventDispatcher
{
public:
    /// Constructor with parameters
    /// @param client - VNC client pointer
    VNCEventDispatcher(VNCClient *client);
    /// Destructor
    virtual ~VNCEventDispatcher();

    /// Constructor with parameters
    /// @param event - event for dispatching
    /// @param consumed - flag, whether event was consumed by previous dispatchers
    /// @return if event consumed - return true, else false
    bool dispatch(QEvent *event, bool consumed);

private:
    bool isRemoteControlEvent(QEvent *event);

private:
    VNCClient *_vncClient;
};

#endif // VNCEVENTDISPATCHER_H
