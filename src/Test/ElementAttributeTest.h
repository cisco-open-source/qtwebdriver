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

#ifndef ELEMENTATTRIBUTETEST_H
#define ELEMENTATTRIBUTETEST_H

#include "CommonQtTestHeaders.h"

class ElementAttributeTestWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ElementAttributeTestWidget();
    virtual ~ElementAttributeTestWidget();

private:
    QLabel *plblDis;
    QLabel *plblEn;
    QPushButton *pbtnEnabled;
    QPushButton *pbtnDisabled;
    QLineEdit *ptxtDis1;
    QLineEdit *ptxtDis2;
    QLineEdit *ptxtEn;
    QPlainTextEdit *ptxtAreaWithDefaultText;
    QPlainTextEdit *ptxtAreaWorking;
    QCheckBox *pchboxWithText;
    QCheckBox *pchboxWithoutText;
    QRadioButton *pradioBtnBlue;
    QRadioButton *pradioBtnYellow;
    
};

#endif // ELEMENTATTRIBUTETEST_H
