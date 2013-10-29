#include "WindowWithDeclarativeViewTest.h"

WindowWithDeclarativeViewTestWidget::WindowWithDeclarativeViewTestWidget(QWidget *parent) :
    QWidget(parent)
{
    setWindowTitle("Test Window");
    pLineEdit = new QLineEdit();
    pLineEdit->setObjectName("inputURL");
    connect(pLineEdit, SIGNAL(returnPressed()), this, SLOT(loadQML()));

    pButton = new QPushButton("Go!");
    pButton->setObjectName("loadButton");
    connect(pButton, SIGNAL(clicked()), this, SLOT(loadQML()));

    pLabel = new QLabel("Status");
    pLabel->setObjectName("labelStatus");
    pLabel->setVisible(true);

    pView = new QDeclarativeView();
    pView->setObjectName("declarativeView");

    connect(pView, SIGNAL(statusChanged(QDeclarativeView::Status)), this, SLOT(displayStatus()));

    QHBoxLayout* hbl = new QHBoxLayout();
    hbl->addWidget(pLineEdit);
    hbl->addWidget(pButton);

    QVBoxLayout *vbl = new QVBoxLayout(this);
    vbl->addLayout(hbl);
    vbl->addWidget(pLabel);

    vbl->addWidget(pView);

    this->setLayout(vbl);
}

WindowWithDeclarativeViewTestWidget::~WindowWithDeclarativeViewTestWidget() { }

void WindowWithDeclarativeViewTestWidget::loadQML() {
    pView->setSource(QUrl(pLineEdit->text()));
    pView->show();
}

void WindowWithDeclarativeViewTestWidget::displayStatus() {

    if (pView->status() == QDeclarativeView::Ready) {
        pLabel->setText("Loading successfully");
        return;
    }

    if (pView->status() == QDeclarativeView::Error) {
        pLabel->setText("Error");
    }
}
