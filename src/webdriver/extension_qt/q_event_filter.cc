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

#include "q_event_filter.h"

QRepaintEventFilter::QRepaintEventFilter(QObject *parent) :
    QObject(parent)
{
}

QRepaintEventFilter::~QRepaintEventFilter() {}

bool QRepaintEventFilter::eventFilter(QObject *pobject, QEvent *pevent) {
    if (pevent->type()== QEvent::Paint) {
        emit repainted();
    }
    return false;
}

void QCheckPagePaint::pagePainted() {
    is_painting = true;
}
