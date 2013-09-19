#include "WindowWithSeparatedDeclarativeAndWebViewsTest.h"

WindowWithSeparatedDeclarativeAndWebViewsTestWidget::WindowWithSeparatedDeclarativeAndWebViewsTestWidget(QWidget *parent) :
    QWidget(parent) {
    this->setWindowTitle("Test Widget");
    pLineEditForWeb = new QLineEdit();
    pLineEditForWeb->setObjectName("inputWebURL");
    connect(pLineEditForWeb, SIGNAL(returnPressed()), this, SLOT(openWebView()));

    pButtonWeb = new QPushButton("Go to Web!");
    pButtonWeb->setObjectName("openWebViewButton");
    connect(pButtonWeb, SIGNAL(clicked()), this, SLOT(openWebView()));

    pLineEditForQml = new QLineEdit();
    pLineEditForQml->setObjectName("inputQmlURL");
    connect(pLineEditForQml, SIGNAL(returnPressed()), this, SLOT(openQmlView()));

    pButtonQml = new QPushButton("Go to Qml!");
    pButtonQml->setObjectName("openQmlViewButton");
    connect(pButtonQml, SIGNAL(clicked()), this, SLOT(openQmlView()));

    QHBoxLayout* hblweb = new QHBoxLayout();
    hblweb->addWidget(pLineEditForWeb);
    hblweb->addWidget(pButtonWeb);

    QHBoxLayout* hblqml = new QHBoxLayout();
    hblqml->addWidget(pLineEditForQml);
    hblqml->addWidget(pButtonQml);

    QVBoxLayout* vbl = new QVBoxLayout();
    vbl->addLayout(hblweb);
    vbl->addLayout(hblqml);
    this->setLayout(vbl);
}

WindowWithSeparatedDeclarativeAndWebViewsTestWidget::~WindowWithSeparatedDeclarativeAndWebViewsTestWidget() {}

void WindowWithSeparatedDeclarativeAndWebViewsTestWidget::openWebView() {
    QWebView *pWebView = new QWebView;
    pWebView->setAttribute(Qt::WA_DeleteOnClose);
    pWebView->load(pLineEditForWeb->text());
    pWebView->show();
}

void WindowWithSeparatedDeclarativeAndWebViewsTestWidget::openQmlView() {
    QDeclarativeView *pView = new QDeclarativeView();
    pView->setAttribute(Qt::WA_DeleteOnClose);
    pView->setSource(pLineEditForQml->text());
    pView->show();
}
