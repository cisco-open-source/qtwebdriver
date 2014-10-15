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

#ifndef BASICKEYBOARDINTERFACETEST_H
#define BASICKEYBOARDINTERFACETEST_H


#include "CommonQtTestHeaders.h"

class LEdit;

class BasicKeyboardInterfaceTestWidget: public QWidget
{
    Q_OBJECT

public:
    explicit BasicKeyboardInterfaceTestWidget();
    virtual ~BasicKeyboardInterfaceTestWidget();

private slots:
    void setResultModifier(const QString &str);
    void setResultDown();
    void setResultUp();

private:
    QLabel *plblResultModifier;
    QLabel *plblResultIsKeyDown;
    QLabel *plblResultIsKeyUp;
    LEdit *plnEdit;

};

class LEdit: public QLineEdit {
    Q_OBJECT

private:
    QString currentText;

public:
    LEdit(QWidget * parent = 0);
    LEdit(const QString & contents, QWidget * parent = 0);
    virtual ~LEdit();

signals:
    void keyDown();
    void keyUp();
    void keyModifier(QString str);

public:
    virtual void keyPressEvent(QKeyEvent *pe);
    virtual void keyReleaseEvent(QKeyEvent *pe);
};


#endif // BASICKEYBOARDINTERFACETEST_H
