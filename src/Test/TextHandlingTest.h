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
