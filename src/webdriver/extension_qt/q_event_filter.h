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

#ifndef Q_EVENT_FILTER_H
#define Q_EVENT_FILTER_H

#include <QtCore/QObject>
#include <QtCore/qcoreevent.h>

class QRepaintEventFilter : public QObject
{
    Q_OBJECT
signals:
    void repainted();
protected:
    virtual bool eventFilter(QObject *pobject, QEvent *pevent);

public:
    explicit QRepaintEventFilter(QObject *parent = 0);
    virtual ~QRepaintEventFilter();
};

class QCheckPagePaint : public QObject {
    Q_OBJECT
public:
    explicit QCheckPagePaint() :
                QObject(NULL) {is_painting = false;}
    bool isPainting() {return is_painting;}

public slots:
    void pagePainted();

private:
    bool is_painting;

};

#endif // Q_EVENT_FILTER_H
