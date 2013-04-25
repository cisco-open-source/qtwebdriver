#ifndef FINDINGTEST_H
#define FINDINGTEST_H

#include <QtCore/qglobal.h>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets/QWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLayout>
#else
#include <QtGui/QWidget>
#include <QtGui/QTextEdit>
#include <QtGui/QScrollArea>
#include <QtGui/QLabel>
#include <QtGui/QLayout>
#endif

class FindingTestWidget : public QWidget
{
public:
    FindingTestWidget();
    virtual ~FindingTestWidget();

private:

    QTextEdit *textEdit;
    QScrollArea *scrollArea;
    QLabel *hiddenLabel;
};

#endif // FINDINGTEST_H
