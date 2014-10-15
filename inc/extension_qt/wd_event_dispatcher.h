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

#ifndef WDEVENTDISPATCHER_H
#define WDEVENTDISPATCHER_H

#include <QtCore/QVector>

#include "event_dispatcher.h"

/// This class contains dispathers chain.
class WDEventDispatcher
{
private:
    /// Constructor
    WDEventDispatcher();
    /// Destructor
    virtual ~WDEventDispatcher();

public:
    /// Method returns WDEventDispatcher instance
    /// @return WDEventDispatcher pointer
    static WDEventDispatcher *getInstance();
    /// Add dispatcher item to dispatchers chain
    /// @param dispatcher - pointer to new dispatcher instance
    void add(EventDispatcher *dispatcher);
    /// Return container with dispatcher chain
    /// @return QVector with EventDispatcher pointers
    QVector<EventDispatcher*>& getDispatchers();
    /// Method dispatch event
    /// @param event - event for dispatching
    /// @return true if event was consumed, else false
    bool dispatch(QEvent *event);

private:
    static WDEventDispatcher *_instance;
    QVector<EventDispatcher*> _dispatchers;
};

#endif // WDEVENTDISPATCHER_H
