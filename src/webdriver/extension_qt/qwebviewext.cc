#include "extension_qt/qwebviewext.h"
#include "webdriver_server.h"
#include <QtCore/QVariant>
#include <QtCore/QTime>
#include <QtCore/QDebug>
#include <QtNetwork/QNetworkProxy>
#include "webdriver_switches.h"
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets/QWidget>
#include <QtWebKitWidgets/QWebFrame>
#else
#include <QtGui/QWidget>
#include <QtWebKit/QWebFrame>
#endif

QWebViewExt::QWebViewExt(QWidget *parent) :
    QWebView(parent)
{
    setVisible(false);
    setWebInspectorProperty(this);
    connect(page(), SIGNAL(windowCloseRequested()), this, SLOT(close()));
    connect(this, SIGNAL(titleChanged(QString)), this, SLOT(setWindowTitle(QString)));
}

QWebViewExt::~QWebViewExt()
{
}

QWebView* QWebViewExt::createWindow(QWebPage::WebWindowType type)
{
    QWebViewExt* newView = new QWebViewExt;
    setWebInspectorProperty(newView);

    newView->show();
    newView->setAttribute(Qt::WA_DeleteOnClose, true);
    return newView;
}

void QWebViewExt::setWebInspectorProperty(QWebViewExt *view)
{
    view->settings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
    CommandLine cmdLine = webdriver::Server::GetInstance()->GetCommandLine();

    if (cmdLine.HasSwitch(webdriver::Switches::kWIServer))
    {
        if (cmdLine.HasSwitch(webdriver::Switches::kWIPort))
        {
            std::string wiPort = cmdLine.GetSwitchValueASCII(webdriver::Switches::kWIPort);
            int port = QString(wiPort.c_str()).toInt();
            view->page()->setProperty("_q_webInspectorServerPort", port);
        }
        else
        {
            view->page()->setProperty("_q_webInspectorServerPort", 9222);
        }
    }
}

