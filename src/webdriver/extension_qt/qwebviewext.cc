#include "extension_qt/qwebviewext.h"
#include "webdriver_server.h"
#include "webdriver_session_manager.h"
#include "webdriver_session.h"
#include "webdriver_logging.h"
#include "extension_qt/qnetwork_access_manager_tracer.h"
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
    QWebView(parent), manager_(0)
{
    setVisible(false);
    setWebInspectorProperty(this);
    setNetworkAccessManagerTracer();
    connect(page(), SIGNAL(windowCloseRequested()), this, SLOT(close()));
    connect(this, SIGNAL(titleChanged(QString)), this, SLOT(setWindowTitle(QString)));
}

QWebViewExt::~QWebViewExt()
{
    delete manager_;
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

void QWebViewExt::setNetworkAccessManagerTracer()
{    
    std::map<std::string, webdriver::Session*> sessionMap = webdriver::SessionManager::GetInstance()->GetSessions();
    if (sessionMap.size() != 1)
    {
        webdriver::GlobalLogger::Log(webdriver::kWarningLogLevel, "Session should be initialized");
        return;
    }
    webdriver::Session* session = sessionMap.begin()->second;
    if (session->GetMinPerfLogLevel() == webdriver::kOffLogLevel)
    {
        webdriver::GlobalLogger::Log(webdriver::kInfoLogLevel, "Performance Log is disabled by defaul");
        return;
    }
    manager_ = new QNetworkAccessManagerTracer(session, this->page());
    this->page()->setNetworkAccessManager(manager_);
}
