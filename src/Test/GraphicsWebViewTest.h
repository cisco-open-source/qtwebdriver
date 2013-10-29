#ifndef GRAPHICS_WEB_VIEW_TEST_H
#define GRAPHICS_WEB_VIEW_TEST_H

#include "CommonQtTestHeaders.h"

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWebKitWidgets/QGraphicsWebView>
#include <QtWidgets/QGraphicsView>
#else
#include <QtWebKit/QGraphicsWebView>
#include <QtGui/QGraphicsView>
#endif

class GraphicsWebViewTestWindows : public QGraphicsView
{
    Q_OBJECT
public:
    explicit GraphicsWebViewTestWindows(QWidget *parent = 0);
    ~GraphicsWebViewTestWindows();
};

#endif // GRAPHICS_WEB_VIEW_TEST_H
