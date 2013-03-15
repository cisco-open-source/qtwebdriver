#ifndef QWEBVIEWEXT_H
#define QWEBVIEWEXT_H

#include <QtWebKit/QWebView>

class QWebViewExt : public QWebView
{
    Q_OBJECT
public:
    explicit QWebViewExt(QWidget *parent = 0);
    ~QWebViewExt();


protected:
    QWebView* createWindow(QWebPage::WebWindowType type);
    
};

#endif // QWEBVIEWEXT_H
