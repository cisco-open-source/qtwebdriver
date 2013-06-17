#ifndef FINDINGTEST_H
#define FINDINGTEST_H

#include "CommonQtTestHeaders.h"

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
