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

#ifndef XPATHELEMENTFINDINGTEST_H
#define XPATHELEMENTFINDINGTEST_H

#include "CommonQtTestHeaders.h"

class XPathElementFindingTestWidget : public QWidget
{
    Q_OBJECT

private:
    QPushButton *ppushBtn;
    QTextEdit *ptxtAreaWorking;
    QLineEdit *pinputElemWithText;
    QLineEdit *pinputElemEmpty;
    QLineEdit *pinputElemReadOnly;
    QScrollArea *pscrollArea;
    QRadioButton *pradioBtn1;
    QRadioButton *pradioBtn2;

public:
    explicit XPathElementFindingTestWidget(QWidget *parent = 0);
    virtual ~XPathElementFindingTestWidget();
        
};

#endif // XPATHELEMENTFINDINGTEST_H
