/*
 * TextHandlingTest.cc
*/

#include "TextHandlingTest.h"

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets/QGridLayout>
#include <QtGui/QStandardItemModel>
#else
#include <QtGui/QGridLayout>
#include <QtGui/QStandardItemModel>
#endif

#include <stdio.h>
#include <QtCore/QDebug>

static const QString TEXT_EDIT_NAME="oneline";
static const QString MULTI_TEXT_NAME="multiline";
static const QString GROUP_BOX_NAME="notext";
static const QString PROGRESS_BAR_NAME="unvisibletext";
static const QString PREFORMATTED_TEXT_NAME="preformatted";
static const QString EMPTY_TEXT_NAME="emptytext";
static const QString LINE_EDIT_NAME="lineedit";
static const QString CHILD_TEXT_NAME="childtext";
static const QString UNVISIBLE_ITEM_NAME="unvisible";
static const QString LIST_VIEW_NAME="listview";

TextHandlingWidget::TextHandlingWidget()
{
    QGridLayout *controlLayout = new QGridLayout(this);
    this->setLayout(controlLayout);

    simpleTextEdit = new QTextEdit(this);
    simpleTextEdit->setObjectName(TEXT_EDIT_NAME);
    simpleTextEdit->setText("A single line of text");

    scrollArea = new QScrollArea(this);
    multiTextEdit = new QTextEdit(this);
    multiTextEdit->setObjectName(MULTI_TEXT_NAME);
    multiTextEdit->setText("A div containing");
    multiTextEdit->append("\nMore than one line of text");
    multiTextEdit->append("\nand block level elements");

    noTextControl = new QGroupBox();
    noTextControl->setObjectName(GROUP_BOX_NAME);

    progressBar = new QProgressBar();
    progressBar->setObjectName(PROGRESS_BAR_NAME);
    progressBar->setValue(30);
    progressBar->setTextVisible(false);

    preformattedText = new QTextEdit();
    preformattedText->setObjectName(PREFORMATTED_TEXT_NAME);
    preformattedText->setText(" This section has a preformatted");
    preformattedText->append("    text block    ");
    preformattedText->append("  split in four lines");
    preformattedText->append("         ");

    emptyTextEdit = new QTextEdit();
    emptyTextEdit->setObjectName(EMPTY_TEXT_NAME);

    editLine = new QLineEdit();
    editLine->setObjectName(LINE_EDIT_NAME);

    childText = new QLineEdit();
    childText->setObjectName(CHILD_TEXT_NAME);
    childText->setText("child text");
    scrollArea->setWidget(childText);

    unvisibleItem = new QLineEdit();
    unvisibleItem->setObjectName(UNVISIBLE_ITEM_NAME);
    unvisibleItem->setText("text");
    unvisibleItem->setVisible(false);

    listView = new QListView();
    listView->setObjectName(LIST_VIEW_NAME);
    listView->setSelectionMode(QAbstractItemView::ExtendedSelection);

    QStandardItemModel *model = new QStandardItemModel();
    QStandardItem *item = new QStandardItem(QString("Some text"));
    model->setItem(0, 0, item);
    QStandardItem *item2 = new QStandardItem(QString("Some text again"));
    model->setItem(0, 1, item2);
    listView->setModel(model);
    listView->setCurrentIndex(listView->model()->index(0,0));

    controlLayout->addWidget(scrollArea, 0, 0);
    controlLayout->addWidget(simpleTextEdit, 1, 0);
    controlLayout->addWidget(multiTextEdit, 2, 0);
    controlLayout->addWidget(noTextControl, 0, 1);
    controlLayout->addWidget(progressBar, 1, 1);
    controlLayout->addWidget(preformattedText, 2, 1);
    controlLayout->addWidget(emptyTextEdit, 0, 2);
    controlLayout->addWidget(editLine, 1, 2);
    controlLayout->addWidget(unvisibleItem, 2, 2);
    controlLayout->addWidget(listView, 0, 3);
}

TextHandlingWidget::~TextHandlingWidget()
{}
