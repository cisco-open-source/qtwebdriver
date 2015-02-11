#ifndef QWEBVIEWEXT_H
#define QWEBVIEWEXT_H

#include <QtCore/QGlobalStatic>
#include <QtCore/QDebug>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWebKitWidgets/QWebView>
#else
#include <QtWebKit/QtWebKit>
#endif

class QNetworkAccessManagerTracer;

class QWebViewExt : public QWebView
{
    Q_OBJECT
public:
    explicit QWebViewExt(QWidget *parent = 0);
    virtual ~QWebViewExt();


protected:
    QWebView* createWindow(QWebPage::WebWindowType type);

private:
    void setWebInspectorProperty(QWebViewExt* view);
    void setNetworkAccessManagerTracer();
    QNetworkAccessManagerTracer* manager_;
};

#endif // QWEBVIEWEXT_H
