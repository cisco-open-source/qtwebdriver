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

#ifndef MENU_TEST_H_
#define MENU_TEST_H_

#include "CommonQtTestHeaders.h"

class MenuTestWidget : public QMainWindow
{
    Q_OBJECT

public:
    MenuTestWidget(QWidget *parent = 0);

protected slots:

    void updateStatus(QString text);
private:
    QLabel *plblStatus;
};

#endif // MENU_TEST_H_
