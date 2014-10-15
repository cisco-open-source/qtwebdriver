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

#include "extension_qt/vnc_event_dispatcher.h"

VNCEventDispatcher::VNCEventDispatcher(VNCClient *client)
    : _vncClient(client)
{
}

VNCEventDispatcher::~VNCEventDispatcher()
{
}

bool VNCEventDispatcher::dispatch(QEvent *event, bool consumed)
{
    if (consumed)
        return false;

    switch(event->type())
    {
        case QEvent::KeyPress:
        case QEvent::KeyRelease: _vncClient->sendKeyEvent(dynamic_cast<QKeyEvent*>(event)); break;
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
        case QEvent::MouseButtonDblClick: _vncClient->sendMouseEvent(dynamic_cast<QMouseEvent*>(event)); break;
        default: break;
    }

    return true;
}
