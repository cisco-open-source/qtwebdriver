/********************************************************************************
** Form generated from reading UI file 'ClickScrollingTest.ui'
**
** Created: Tue May 14 16:05:13 2013
**      by: Qt User Interface Compiler version 4.8.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CLICKSCROLLINGTEST_H
#define UI_CLICKSCROLLINGTEST_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QScrollArea>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ClickScrollingTest
{
public:
    QWidget *centralWidget;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QPushButton *buttonHiddenByOverflow;
    QLabel *textLabel;
    QPushButton *visibleButton;
    QRadioButton *radioButtonHiddenByOverflow;
    QCheckBox *checkBoxHiddenByOverflow;
    QRadioButton *visibleRadioButton;
    QRadioButton *partiallyVisibleRadioButton;
    QPushButton *outOfViewButton;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *ClickScrollingTest)
    {
        if (ClickScrollingTest->objectName().isEmpty())
            ClickScrollingTest->setObjectName(QString::fromUtf8("ClickScrollingTest"));
        ClickScrollingTest->resize(398, 284);
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(ClickScrollingTest->sizePolicy().hasHeightForWidth());
        ClickScrollingTest->setSizePolicy(sizePolicy);
        centralWidget = new QWidget(ClickScrollingTest);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        scrollArea = new QScrollArea(centralWidget);
        scrollArea->setObjectName(QString::fromUtf8("scrollArea"));
        scrollArea->setGeometry(QRect(0, 0, 391, 221));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(scrollArea->sizePolicy().hasHeightForWidth());
        scrollArea->setSizePolicy(sizePolicy1);
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QString::fromUtf8("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 800, 800));
        sizePolicy.setHeightForWidth(scrollAreaWidgetContents->sizePolicy().hasHeightForWidth());
        scrollAreaWidgetContents->setSizePolicy(sizePolicy);
        scrollAreaWidgetContents->setMinimumSize(QSize(800, 800));
        buttonHiddenByOverflow = new QPushButton(scrollAreaWidgetContents);
        buttonHiddenByOverflow->setObjectName(QString::fromUtf8("buttonHiddenByOverflow"));
        buttonHiddenByOverflow->setGeometry(QRect(10, 280, 181, 31));
        textLabel = new QLabel(scrollAreaWidgetContents);
        textLabel->setObjectName(QString::fromUtf8("textLabel"));
        textLabel->setGeometry(QRect(10, 10, 67, 21));
        visibleButton = new QPushButton(scrollAreaWidgetContents);
        visibleButton->setObjectName(QString::fromUtf8("visibleButton"));
        visibleButton->setGeometry(QRect(10, 30, 95, 31));
        radioButtonHiddenByOverflow = new QRadioButton(scrollAreaWidgetContents);
        radioButtonHiddenByOverflow->setObjectName(QString::fromUtf8("radioButtonHiddenByOverflow"));
        radioButtonHiddenByOverflow->setGeometry(QRect(10, 320, 421, 20));
        checkBoxHiddenByOverflow = new QCheckBox(scrollAreaWidgetContents);
        checkBoxHiddenByOverflow->setObjectName(QString::fromUtf8("checkBoxHiddenByOverflow"));
        checkBoxHiddenByOverflow->setGeometry(QRect(20, 360, 221, 26));
        visibleRadioButton = new QRadioButton(scrollAreaWidgetContents);
        visibleRadioButton->setObjectName(QString::fromUtf8("visibleRadioButton"));
        visibleRadioButton->setGeometry(QRect(10, 80, 151, 20));
        partiallyVisibleRadioButton = new QRadioButton(scrollAreaWidgetContents);
        partiallyVisibleRadioButton->setObjectName(QString::fromUtf8("partiallyVisibleRadioButton"));
        partiallyVisibleRadioButton->setGeometry(QRect(20, 190, 221, 51));
        partiallyVisibleRadioButton->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 0, 0);"));
        scrollArea->setWidget(scrollAreaWidgetContents);
        outOfViewButton = new QPushButton(centralWidget);
        outOfViewButton->setObjectName(QString::fromUtf8("outOfViewButton"));
        outOfViewButton->setGeometry(QRect(340, 330, 121, 27));
        ClickScrollingTest->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(ClickScrollingTest);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 398, 23));
        ClickScrollingTest->setMenuBar(menuBar);
        mainToolBar = new QToolBar(ClickScrollingTest);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        ClickScrollingTest->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(ClickScrollingTest);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        ClickScrollingTest->setStatusBar(statusBar);

        retranslateUi(ClickScrollingTest);

        QMetaObject::connectSlotsByName(ClickScrollingTest);
    } // setupUi

    void retranslateUi(QMainWindow *ClickScrollingTest)
    {
        ClickScrollingTest->setWindowTitle(QApplication::translate("ClickScrollingTest", "ClickScrollingTest", 0, QApplication::UnicodeUTF8));
        buttonHiddenByOverflow->setText(QApplication::translate("ClickScrollingTest", "buttonHiddenByOverflow", 0, QApplication::UnicodeUTF8));
        textLabel->setText(QApplication::translate("ClickScrollingTest", "TextLabel", 0, QApplication::UnicodeUTF8));
        visibleButton->setText(QApplication::translate("ClickScrollingTest", "visibleButton", 0, QApplication::UnicodeUTF8));
        radioButtonHiddenByOverflow->setText(QApplication::translate("ClickScrollingTest", "radioButtonHiddenByOverflow", 0, QApplication::UnicodeUTF8));
        checkBoxHiddenByOverflow->setText(QApplication::translate("ClickScrollingTest", "checkBoxHiddenByOverflow", 0, QApplication::UnicodeUTF8));
        visibleRadioButton->setText(QApplication::translate("ClickScrollingTest", "visibleRadioButton", 0, QApplication::UnicodeUTF8));
        partiallyVisibleRadioButton->setText(QApplication::translate("ClickScrollingTest", "partiallyVisibleRadioButton", 0, QApplication::UnicodeUTF8));
        outOfViewButton->setText(QApplication::translate("ClickScrollingTest", "outOfViewButton", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class ClickScrollingTest: public Ui_ClickScrollingTest {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CLICKSCROLLINGTEST_H
