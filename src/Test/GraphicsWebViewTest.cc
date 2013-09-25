#include "GraphicsWebViewTest.h"

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWebKitWidgets/QWebView>
#else
#include <QtWebKit/QWebView>
#endif

GraphicsWebViewTestWindows::GraphicsWebViewTestWindows(QWidget *parent) :
    QGraphicsView(parent) {
    this->setWindowTitle("Test Widget");

    QGraphicsScene *pScene = new QGraphicsScene();
    this->setScene(pScene);
    this->resize(800, 600);
    QGraphicsWebView *pWebView = new QGraphicsWebView();
    this->scene()->addItem(pWebView);
}

GraphicsWebViewTestWindows::~GraphicsWebViewTestWindows() {}

