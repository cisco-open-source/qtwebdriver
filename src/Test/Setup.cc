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

#include "Headers.h"

#ifndef QT_NO_SAMPLES
#include "Samples.h"
#endif

#ifndef QT_NO_VNC
void InitVNCClient();
#endif // QT_NO_VNC
#ifndef QT_NO_UINPUT
void InitUInputClient();
#endif  // QT_NO_UINPUT

int wd_setup(int argc, char *argv[])
{
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
    wd_samples_setup(widgetCreator, webCreator, qmlCreator);
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

    /* Optional VNC an user input support */
#ifndef QT_NO_VNC
    InitVNCClient();
#endif // QT_NO_VNC
#ifndef QT_NO_UINPUT
    InitUInputClient();
#endif // QT_NO_UINPUT
    
    /* Start webdriver */
    int startError = wd_server->Start();
    if (startError){
        std::cout << "Error while starting server, errorCode " << startError << std::endl;
        return startError;
    }

    return startError;
}

#ifndef QT_NO_VNC
void InitVNCClient() {
    // start VNC module
    CommandLine cmdLine = webdriver::Server::GetInstance()->GetCommandLine();
    if (cmdLine.HasSwitch(webdriver::Switches::kVNCLogin))
    {
        QString address = "127.0.0.1";
        QString login = "anonymous";
        QString *password = new QString();
        QString port = "5900";

        QString loginInfo = cmdLine.GetSwitchValueASCII(webdriver::Switches::kVNCLogin).c_str();
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
}
#endif // QT_NO_VNC

#ifndef QT_NO_UINPUT
void InitUInputClient() {
    // start user input device
#ifdef OS_LINUX
    CommandLine cmdLine = webdriver::Server::GetInstance()->GetCommandLine();
    if (cmdLine.HasSwitch(webdriver::Switches::kUserInputDevice))
    {
        UInputManager *manager = UInputManager::getInstance();
        if (!manager->isReady())
        {
            manager->registerUinputDevice();
        }

        WDEventDispatcher::getInstance()->add(new UInputEventDispatcher(manager));
    }
#endif // OS_LINUX
}
#endif // QT_NO_UINPUT

#ifndef QT_NO_SAMPLES
int wd_samples_setup(webdriver::ViewCreator* widgetCreator,
    webdriver::ViewCreator* webCreator,
    webdriver::ViewCreator* qmlCreator)
{
    if(widgetCreator != NULL)
    {
        widgetCreator->RegisterViewClass<WindowTestWidget>("WindowTestWidget");
        widgetCreator->RegisterViewClass<ClickTestWidget>("ClickTestWidget");
        widgetCreator->RegisterViewClass<ElementAttributeTestWidget>("ElementAttributeTestWidget");
        widgetCreator->RegisterViewClass<FindingTestWidget>("FindingTestWidget");
        widgetCreator->RegisterViewClass<CoordinatesTestWidget>("CoordinatesTestWidget");
        widgetCreator->RegisterViewClass<ClickScrollingTestWidget>("ClickScrollingTestWidget");
        widgetCreator->RegisterViewClass<ElementSelectingTestWidget>("ElementSelectingTestWidget");
        widgetCreator->RegisterViewClass<TypingTestWidget>("TypingTestWidget");
        widgetCreator->RegisterViewClass<BasicKeyboardInterfaceTestWidget>("BasicKeyboardInterfaceTestWidget");
        widgetCreator->RegisterViewClass<TextHandlingTestWidget>("TextHandlingTestWidget");
        widgetCreator->RegisterViewClass<FormHandlingTestWidget>("FormHandlingTestWidget");
        widgetCreator->RegisterViewClass<XPathElementFindingTestWidget>("XPathElementFindingTestWidget");
        widgetCreator->RegisterViewClass<StaleElementReferenceTestWidget>("StaleElementReferenceTestWidget");
        widgetCreator->RegisterViewClass<VisibilityTestWidget>("VisibilityTestWidget");
        widgetCreator->RegisterViewClass<BasicMouseInterfaceTestWidget>("BasicMouseInterfaceTestWidget");
        widgetCreator->RegisterViewClass<TouchTestWidget>("TouchTestWidget");
        widgetCreator->RegisterViewClass<MenuTestWidget>("MenuTestWidget");
    #if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
        widgetCreator->RegisterViewClass<WindowWithDeclarativeViewTestWidget>("WindowWithDeclarativeViewTestWidget");
    #endif

    #if (1 == WD_ENABLE_PLAYER) && (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        widgetCreator->RegisterViewClass<VideoTestWidget>("VideoTestWidget");
    #endif //WD_ENABLE_PLAYER

    #if (WD_ENABLE_WEB_VIEW == 1)
        /* Register som test classes */
        widgetCreator->RegisterViewClass<GraphicsWebViewTestWindows>("GraphicsWebViewTestWindows");
        widgetCreator->RegisterViewClass<WindowWithEmbeddedViewTestWidget>("WindowWithEmbeddedViewTestWidget");
        widgetCreator->RegisterViewClass<WidgetAndWebViewTestWindows>("WidgetAndWebViewTestWindows");

    #if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
        widgetCreator->RegisterViewClass<WindowWithSeparatedDeclarativeAndWebViewsTestWidget>("WindowWithSeparatedDeclarativeAndWebViewsTestWidget");
    #endif // QT_VERSION
    #endif // WD_ENABLE_WEB_VIEW
    }
#if (1 == WD_ENABLE_PLAYER) && (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    // check if --test_data_folder CL argument is present
    std::string testDataFolderSwitch = "test_data_folder";

    if (cmd_line.HasSwitch(testDataFolderSwitch)) {
      testDataFolder = cmd_line.GetSwitchValueASCII(testDataFolderSwitch);
    } else {
      testDataFolder = "./";
    }

    std::cout << "Using "<< testDataFolder << " as test data folder" << std::endl;
#endif // WD_ENABLE_PLAYER
    return 0;
}
#endif // QT_NO_SAMPLES