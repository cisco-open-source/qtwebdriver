#include "extension_qt/qwebviewext.h"
#include <QtCore/QVariant>
#include <QtCore/QTime>
#include <QtCore/QDebug>
#include <QtNetwork/QNetworkProxy>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets/QWidget>
#else
#include <QtGui/QWidget>
#endif

QWebViewExt::QWebViewExt(QWidget *parent) :
    QWebView(parent)
{
    connect(page(), SIGNAL(windowCloseRequested()), this, SLOT(close()));
}

QWebViewExt::~QWebViewExt()
{
}

QWebView* QWebViewExt::createWindow(QWebPage::WebWindowType type)
{
    QWebViewExt* newView = new QWebViewExt;
    newView->show();
    newView->setAttribute(Qt::WA_DeleteOnClose, true);
    QVariant sessionId = property("sessionId");
    if (sessionId.isValid())
        newView->setProperty("sessionId", sessionId.toInt());
    if (!page()->networkAccessManager()->proxy().hostName().isEmpty())
        newView->page()->networkAccessManager()->setProxy(page()->networkAccessManager()->proxy());
    return newView;
}

