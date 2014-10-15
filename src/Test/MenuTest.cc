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

#include "MenuTest.h"

#include <QtCore/QSignalMapper>

MenuTestWidget::MenuTestWidget(QWidget *parent) :
    QMainWindow(parent)
{
    QSignalMapper* signalMapper = new QSignalMapper (this);

    plblStatus = new QLabel("no action", this);
    plblStatus->setObjectName("statusLabel");
    setCentralWidget(plblStatus);

    QAction *action_1_1 = new QAction("Action_1_1", this);
    QAction *action_1_2 = new QAction("Action_1_2", this);
    QAction *action_1_3 = new QAction("Action_1_3", this);

    QAction *action_2_1 = new QAction("Action_2_1", this);
    QAction *action_2_2 = new QAction("Action_2_2", this);

    QAction *action_2_1_1 = new QAction("Action_2_1_1", this);
    QAction *action_2_1_2 = new QAction("Action_2_1_2", this);

    QMenu *menu1;
    menu1 = menuBar()->addMenu("Menu_1");
    menu1->addAction(action_1_1);
    menu1->addAction(action_1_2);
    menu1->addAction(action_1_3);

    QMenu *menu2;
    menu2 = menuBar()->addMenu("Menu_2");
    menu2->addAction(action_2_1);
    menu2->addAction(action_2_2);

    QMenu *menu2_1;
    menu2_1 = menu2->addMenu("Menu_2_1");
    menu2_1->addAction(action_2_1_1);
    menu2_1->addAction(action_2_1_2);
    
    connect(action_1_1, SIGNAL(triggered()), signalMapper, SLOT(map()));
    connect(action_1_2, SIGNAL(triggered()), signalMapper, SLOT(map()));
    connect(action_1_3, SIGNAL(triggered()), signalMapper, SLOT(map()));
    connect(action_2_1, SIGNAL(triggered()), signalMapper, SLOT(map()));
    connect(action_2_2, SIGNAL(triggered()), signalMapper, SLOT(map()));
    connect(action_2_1_1, SIGNAL(triggered()), signalMapper, SLOT(map()));
    connect(action_2_1_2, SIGNAL(triggered()), signalMapper, SLOT(map()));
    
    signalMapper->setMapping(action_1_1, "Menu 1, Action 1");
    signalMapper->setMapping(action_1_2, "Menu 1, Action 2");
    signalMapper->setMapping(action_1_3, "Menu 1, Action 3");
    signalMapper->setMapping(action_2_1, "Menu 2, Action 1");
    signalMapper->setMapping(action_2_2, "Menu 2, Action 2");
    signalMapper->setMapping(action_2_1_1, "Menu 2-1, Action 1");
    signalMapper->setMapping(action_2_1_2, "Menu 2-1, Action 2");
 
    connect(signalMapper, SIGNAL(mapped(QString)), this, SLOT(updateStatus(QString))) ;
}

void MenuTestWidget::updateStatus(QString text) {
    plblStatus->setText(text);
}
