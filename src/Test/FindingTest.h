#ifndef FINDINGTEST_H
#define FINDINGTEST_H

#include <QtCore/qglobal.h>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets/QWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QLabel>
#else
#include <QtGui/QWidget>
#include <QtGui/QTextEdit>
#include <QtGui/QScrollArea>
#include <QtGui/QLabel>
#endif

class FindingTestWidget : public QWidget
{
public:
    FindingTestWidget();
    ~FindingTestWidget();

private:

    QTextEdit *textEdit;
    QScrollArea *scrollArea;
    QLabel *hiddenLabel;
};

#endif // FINDINGTEST_H
