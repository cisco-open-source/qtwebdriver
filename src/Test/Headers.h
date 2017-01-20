/****************************************************************************
**
** Copyright © 1992-2014 Cisco and/or its affiliates. All rights reserved.
** All rights reserved.
** 
** $CISCO_BEGIN_LICENSE:LGPL$
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** $CISCO_END_LICENSE$
**
****************************************************************************/

#ifndef WD_SETUP_H
#define WD_SETUP_H

#include <QtCore/QObject>
#include <QtCore/QTextCodec>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    #include <QtConcurrent/QtConcurrentRun>
    #if (WD_ENABLE_WEB_VIEW == 1)
        #include <QtWebKitWidgets/QWebView>
    #endif
    #include <QtWidgets/QApplication>
#else
    #include <QtGui/QApplication>
    #if (WD_ENABLE_WEB_VIEW == 1)
        #include <QtWebKit/QtWebKit>
    #endif
#endif

#include <iostream>

#include "base/at_exit.h"
#include "webdriver_server.h"
#include "webdriver_view_transitions.h"
#include "versioninfo.h"
#include "webdriver_route_table.h"
#include "commands/shutdown_command.h"
#include "webdriver_route_patterns.h"

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    // headers for Quick2 extension
    #include "extension_qt/quick2_view_creator.h"
    #include "extension_qt/quick2_view_executor.h"
    #include "extension_qt/quick2_view_enumerator.h"
#else
    #include <QtDeclarative/QDeclarativeView>
    // headers for Quick1 extension
    #include "extension_qt/qml_view_creator.h"
    #include "extension_qt/qml_view_executor.h"
    #include "extension_qt/qml_view_enumerator.h"
#if (WD_ENABLE_WEB_VIEW == 1)
    #include "extension_qt/qdeclarativewebview.h"
    #include "extension_qt/qml_web_view_enumerator.h"
    #include "extension_qt/qml_web_view_executor.h"
#endif //WD_ENABLE_WEB_VIEW
#endif //QT_VERSION

#if (WD_ENABLE_WEB_VIEW == 1)
#include "extension_qt/web_view_creator.h"
#include "extension_qt/web_view_executor.h"
#include "extension_qt/web_view_enumerator.h"
#include "extension_qt/qwebviewext.h"
#include "extension_qt/graphics_web_view_executor.h"
#include "extension_qt/graphics_web_view_enumerator.h"
#endif //WD_ENABLE_WEB_VIEW

#include "extension_qt/q_view_runner.h"
#include "extension_qt/q_session_lifecycle_actions.h"
#include "extension_qt/widget_view_creator.h"
#include "extension_qt/widget_view_enumerator.h"
#include "extension_qt/widget_view_executor.h"
#include "extension_qt/wd_event_dispatcher.h"
#include "extension_qt/vnc_event_dispatcher.h"
#include "extension_qt/uinput_event_dispatcher.h"

#include "extension_qt/vncclient.h"
#include "extension_qt/uinput_manager.h"
#include "webdriver_switches.h"

#ifndef QT_NO_SAMPLES
#include "Samples.h"
#endif

int wd_setup(int argc, char *argv[])
{
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("utf8"));
#endif
    webdriver::ViewRunner::RegisterCustomRunner<webdriver::QViewRunner>();

    webdriver::SessionLifeCycleActions::RegisterCustomLifeCycleActions<webdriver::QSessionLifeCycleActions>();

    webdriver::ViewTransitionManager::SetURLTransitionAction(new webdriver::URLTransitionAction_CloseOldView());

    /* Configure widget views */
    webdriver::ViewCreator* widgetCreator = new webdriver::QWidgetViewCreator();
    webdriver::ViewCreator* webCreator = NULL;
    webdriver::ViewCreator* qmlCreator =  NULL;
    /* 
       Register view classes (here some test classes) that can be created by WebDriver. 
       Creation can be triggered by client side request like wd.get("qtwidget://WindowTestWidget"); 
       See https://github.com/cisco-open-source/qtwebdriver/wiki/Hybridity-And-View-Management
    */
    widgetCreator->RegisterViewClass<QWidget>("QWidget");
    
#if (WD_ENABLE_WEB_VIEW == 1)
    /* Configure web views */
    webCreator = new webdriver::QWebViewCreator();
    webCreator->RegisterViewClass<QWebViewExt>("QWebViewExt");
    webdriver::ViewFactory::GetInstance()->AddViewCreator(webCreator);
  
    /* Configure WebView support */
    webdriver::ViewEnumerator::AddViewEnumeratorImpl(new webdriver::WebViewEnumeratorImpl());
    webdriver::ViewCmdExecutorFactory::GetInstance()->AddViewCmdExecutorCreator(new webdriver::QWebViewCmdExecutorCreator());

    /* Configure GraphicsWebView support */
    webdriver::ViewEnumerator::AddViewEnumeratorImpl(new webdriver::GraphicsWebViewEnumeratorImpl());
    webdriver::ViewCmdExecutorFactory::GetInstance()->AddViewCmdExecutorCreator(new webdriver::GraphicsWebViewCmdExecutorCreator());  
#endif // WD_ENABLE_WEB_VIEW

#ifndef QT_NO_QML
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    // Quick2 extension
    qmlCreator = new webdriver::Quick2ViewCreator();
    qmlCreator->RegisterViewClass<QQuickView>("QQuickView");
    webdriver::ViewFactory::GetInstance()->AddViewCreator(qmlCreator);
    webdriver::ViewEnumerator::AddViewEnumeratorImpl(new webdriver::Quick2ViewEnumeratorImpl());
    webdriver::ViewCmdExecutorFactory::GetInstance()->AddViewCmdExecutorCreator(new webdriver::Quick2ViewCmdExecutorCreator());
#else
    // Quick1 extension
    qmlCreator = new webdriver::QQmlViewCreator();
    qmlCreator->RegisterViewClass<QDeclarativeView>("QDeclarativeView");
    webdriver::ViewFactory::GetInstance()->AddViewCreator(qmlCreator);
    webdriver::ViewEnumerator::AddViewEnumeratorImpl(new webdriver::QmlViewEnumeratorImpl());
    webdriver::ViewCmdExecutorFactory::GetInstance()->AddViewCmdExecutorCreator(new webdriver::QQmlViewCmdExecutorCreator());
    
    #if (WD_ENABLE_WEB_VIEW == 1)
        qmlRegisterType<QDeclarativeWebSettings>();
        qmlRegisterType<QDeclarativeWebView>("CiscoQtWebKit", 1, 0, "CiscoWebView");
        qmlRegisterType<QDeclarativeWebView>("CiscoQtWebKit", 1, 1, "CiscoWebView");
        qmlRegisterRevision<QDeclarativeWebView, 0>("CiscoQtWebKit", 1, 0);
        qmlRegisterRevision<QDeclarativeWebView, 1>("CiscoQtWebKit", 1, 1);
    #endif

#endif
#endif //QT_NO_QML
    /* Add widget creator last so that it deos not conflict with webview creator (QWebView is a subclass of QWidget)*/
    webdriver::ViewFactory::GetInstance()->AddViewCreator(widgetCreator);
    webdriver::ViewEnumerator::AddViewEnumeratorImpl(new webdriver::WidgetViewEnumeratorImpl());
    webdriver::ViewCmdExecutorFactory::GetInstance()->AddViewCmdExecutorCreator(new webdriver::QWidgetViewCmdExecutorCreator());
    
    CommandLine cmd_line(CommandLine::NO_PROGRAM);
#if defined(OS_WIN)
    cmd_line.ParseFromString(::GetCommandLineW());
#elif defined(OS_POSIX)
    cmd_line.InitFromArgv(argc, argv);
#endif

#ifndef QT_NO_SAMPLES
    /* optional samples setup */
    wd_samples_setup(widgetCreator, webCreator, qmlCreator, cmd_line);
#endif // QT_NO_SAMPLES

#if defined(OS_WIN)
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    system("qtenv2.bat vsvars");
#else  //QT_VERSION
    system("qtvars.bat vsvars");
#endif //QT_VERSION
#endif //OS_WIN

    /* Parse command line */
    webdriver::Server* wd_server = webdriver::Server::GetInstance();
    if (0 != wd_server->Configure(cmd_line)) {
        std::cout << "Error while configuring WD server, exiting..." << std::endl;
        return 1;
    }

    /* Example how to add a custom command */
    webdriver::RouteTable *routeTableWithShutdownCommand = new webdriver::RouteTable(wd_server->GetRouteTable());
    const char shutdownCommandRoute[] = "/-cisco-shutdown";
    routeTableWithShutdownCommand->Add<webdriver::ShutdownCommand>(shutdownCommandRoute);
    routeTableWithShutdownCommand->Add<webdriver::ShutdownCommand>(webdriver::CommandRoutes::kShutdown);
    wd_server->SetRouteTable(routeTableWithShutdownCommand);

    /* check for VNC input support */
    if (cmd_line.HasSwitch(webdriver::Switches::kVNCLogin))
    {
        QString address = "127.0.0.1";
        QString login = "anonymous";
        QString *password = new QString();
        QString port = "5900";

        QString loginInfo = cmd_line.GetSwitchValueASCII(webdriver::Switches::kVNCLogin).c_str();
        VNCClient::SplitVncLoginParameters(loginInfo, &login, password, &address, &port);

        VNCClient *client = VNCClient::getInstance();
        if (!client->isReady())
        {
            if (password->isEmpty())
                client->Init(address, port.toInt());
            else
                client->Init(address, port.toInt(), password);
        }

        WDEventDispatcher::getInstance()->add(new VNCEventDispatcher(client));
    }

    /* check for Linux UInput input support */
#ifdef OS_LINUX
    if (cmd_line.HasSwitch(webdriver::Switches::kUserInputDevice))
    {
        UInputManager *manager = UInputManager::getInstance();
        if (!manager->isReady())
        {
            manager->registerUinputDevice();
        }

        WDEventDispatcher::getInstance()->add(new UInputEventDispatcher(manager));
    }
#endif // OS_LINUX

    /* Start webdriver */
    int startError = wd_server->Start();
    if (startError){
        std::cout << "Error while starting server, errorCode " << startError << std::endl;
        return startError;
    }

    return startError;
}
#endif // WD_SETUP_H
