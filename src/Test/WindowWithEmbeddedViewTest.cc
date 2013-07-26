#include "WindowWithEmbeddedViewTest.h"
#include <QtWebKit/QWebView>

WindowWithEmbeddedViewTestWidget::WindowWithEmbeddedViewTestWidget(QWidget *parent) :
    QWidget(parent)
{
    setWindowTitle("Test Window");
    pLineEdit = new QLineEdit();
    pLineEdit->setObjectName("inputURL");
    connect(pLineEdit, SIGNAL(returnPressed()), this, SLOT(loadHTML()));

    pButton = new QPushButton("Go!");
    pButton->setObjectName("loadButton");
    connect(pButton, SIGNAL(clicked()), this, SLOT(loadHTML()));

    pLabel = new QLabel("Here Will Be HTML Title");
    pLabel->setObjectName("labelTitle");
    pLabel->setVisible(true);

    pView = new QWebView();
    pView->setObjectName("webView");
    connect(pView, SIGNAL(loadFinished(bool)), this, SLOT(setHTMLTitle()));

    QHBoxLayout* hbl = new QHBoxLayout();
    hbl->addWidget(pLineEdit);
    hbl->addWidget(pButton);

    QVBoxLayout *vbl = new QVBoxLayout(this);
    vbl->addLayout(hbl);
    vbl->addWidget(pLabel);
    vbl->addWidget(pView);
    this->setLayout(vbl);
}

WindowWithEmbeddedViewTestWidget::~WindowWithEmbeddedViewTestWidget() {}

void WindowWithEmbeddedViewTestWidget::loadHTML() {
    pView->load(QUrl(pLineEdit->text()));
}

void WindowWithEmbeddedViewTestWidget::setHTMLTitle() {
    pLabel->setText(pView->title());
}
