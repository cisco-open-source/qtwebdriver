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

#include "build/build_config.h"
#include "extension_qt/uinput_event_dispatcher.h"

#if defined(OS_LINUX)

UInputEventDispatcher::UInputEventDispatcher(UInputManager *manager)
    : _eventManager(manager)
{
}

UInputEventDispatcher::~UInputEventDispatcher()
{
}

bool UInputEventDispatcher::dispatch(QEvent *event, bool consumed)
{
    if (consumed)
        return false;

    QKeyEvent *keyEvent = dynamic_cast<QKeyEvent*>(event);
    if(NULL != keyEvent)
    {
        _eventManager->injectKeyEvent(keyEvent);

        return true;
    }

    return false;
}

#endif // OS_LINUX
