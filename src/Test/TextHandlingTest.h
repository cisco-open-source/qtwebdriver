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

#ifndef TEXTHANDLINGTEST_H
#define TEXTHANDLINGTEST_H

#include <QtCore/qglobal.h>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets/QWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListView>
#else
#include <QtGui/QWidget>
#include <QtGui/QTextEdit>
#include <QtGui/QScrollArea>
#include <QtGui/QLabel>
#include <QtGui/QLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QProgressBar>
#include <QtGui/QLineEdit>
#include <QtGui/QListView>
#endif

class TextHandlingTestWidget : public QWidget
{
    Q_OBJECT
public:
    TextHandlingTestWidget();
    ~TextHandlingTestWidget();

private:
    QTextEdit *simpleTextEdit;
    QScrollArea *scrollArea;
    QTextEdit *multiTextEdit;
    QGroupBox *noTextControl;
    QProgressBar *progressBar;
    QTextEdit *preformattedText;
    QTextEdit *emptyTextEdit;
    QLineEdit *editLine;
    QLineEdit *childText;
    QLineEdit *unvisibleItem;
    QListView *listView;
};

#endif // TEXTHANDLINGTEST_H
