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
    pWebView = new QGraphicsWebView();
    pScene->addItem(pWebView);

    this->setScene(pScene);
}

GraphicsWebViewTestWindows::~GraphicsWebViewTestWindows() {
    delete pWebView;
}
