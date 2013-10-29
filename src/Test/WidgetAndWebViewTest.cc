#include "WidgetAndWebViewTest.h"

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWebKitWidgets/QWebView>
#else
#include <QtWebKit/QWebView>
#endif

WidgetAndWebViewTestWindows::WidgetAndWebViewTestWindows(QWidget *parent) :
    QWidget(parent) {
    this->setWindowTitle("Test Widget");
    pLineEdit = new QLineEdit();
    pLineEdit->setObjectName("inputURL");
    connect(pLineEdit, SIGNAL(returnPressed()), this, SLOT(openWebView()));

    pButton = new QPushButton("Go!");
    pButton->setObjectName("openViewButton");
    connect(pButton, SIGNAL(clicked()), this, SLOT(openWebView()));

    QHBoxLayout* hbl = new QHBoxLayout();
    hbl->addWidget(pLineEdit);
    hbl->addWidget(pButton);
    this->setLayout(hbl);

}

WidgetAndWebViewTestWindows::~WidgetAndWebViewTestWindows() {}

void WidgetAndWebViewTestWindows::openWebView() {
    QWebView *pWebView = new QWebView;
    pWebView->setAttribute(Qt::WA_DeleteOnClose);
    pWebView->load(pLineEdit->text());
    pWebView->show();

}
