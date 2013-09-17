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

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    pView = new QQuickView();
#else
    pView = new QDeclarativeView();
#endif

    pView->setObjectName("declarativeView");

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    connect(pView, SIGNAL(statusChanged(QQuickView::Status)), this, SLOT(displayStatus()));
#else
    connect(pView, SIGNAL(statusChanged(QDeclarativeView::Status)), this, SLOT(displayStatus()));
#endif

    QHBoxLayout* hbl = new QHBoxLayout();
    hbl->addWidget(pLineEdit);
    hbl->addWidget(pButton);

    QVBoxLayout *vbl = new QVBoxLayout(this);
    vbl->addLayout(hbl);
    vbl->addWidget(pLabel);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    QWidget *container = QWidget::createWindowContainer(pView);
    vbl->addWidget(container);
#else
    vbl->addWidget(pView);
#endif
    this->setLayout(vbl);
}

WindowWithDeclarativeViewTestWidget::~WindowWithDeclarativeViewTestWidget() { }

void WindowWithDeclarativeViewTestWidget::loadQML() {
    pView->setSource(QUrl(pLineEdit->text()));
    pView->show();
}

void WindowWithDeclarativeViewTestWidget::displayStatus() {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    if (pView->status() == QQuickView::Ready)
#else
    if (pView->status() == QDeclarativeView::Ready)
#endif
        pLabel->setText("Loading successfully");

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    if (pView->status() == QQuickView::Error)
#else
    if (pView->status() == QDeclarativeView::Error)
#endif
        pLabel->setText("Error");
}
