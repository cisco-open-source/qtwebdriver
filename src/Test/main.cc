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

#include <QtCore/QObject>
#include <QtCore/QTextCodec>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    #include <QtConcurrent/QtConcurrentRun>
    #if (WD_TEST_ENABLE_WEB_VIEW == 1)
        #include <QtWebKitWidgets/QWebView>
    #endif
    #include <QtWidgets/QApplication>
#else
    #include <QtGui/QApplication>
    #if (WD_TEST_ENABLE_WEB_VIEW == 1)
        #include <QtWebKit/QtWebKit>
    #endif
#endif

#include <iostream>

#ifndef QT_NO_SAMPLES
#include "WindowTest.h"
#include "ClickTest.h"
#include "ElementAttributeTest.h"
#include "FindingTest.h"
#include "CoordinatesTest.h"
#include "ClickScrollingTest.h"
#include "ElementSelectingTest.h"
#include "TypingTest.h"
#include "BasicKeyboardInterfaceTest.h"
#include "TextHandlingTest.h"
#include "FormHandlingTest.h"
#include "XPathElementFindingTest.h"
#include "StaleElementReferenceTest.h"
#include "VisibilityTest.h"
#include "BasicMouseInterfaceTest.h"
#include "TouchTest.h"
#include "MenuTest.h"

#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
#include "WindowWithDeclarativeViewTest.h"
#endif

#if (1 == WD_ENABLE_PLAYER) && (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include "VideoTest.h"
extern std::string testDataFolder;
#endif // WD_ENABLE_PLAYER
#endif // QT_NO_SAMPLES

#include "base/at_exit.h"
#include "webdriver_server.h"
#include "webdriver_view_transitions.h"
#include "versioninfo.h"
#include "webdriver_route_table.h"
#include "shutdown_command.h"
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
#if (WD_TEST_ENABLE_WEB_VIEW == 1)
    #include "extension_qt/qdeclarativewebview.h"
    #include "extension_qt/qml_web_view_enumerator.h"
    #include "extension_qt/qml_web_view_executor.h"
#endif //WD_TEST_ENABLE_WEB_VIEW
#endif //QT_VERSION

#if (WD_TEST_ENABLE_WEB_VIEW == 1)
#include "extension_qt/web_view_creator.h"
#include "extension_qt/web_view_executor.h"
#include "extension_qt/web_view_enumerator.h"
#include "extension_qt/qwebviewext.h"
#include "extension_qt/graphics_web_view_executor.h"
#include "extension_qt/graphics_web_view_enumerator.h"

#ifndef QT_NO_SAMPLES
#include "GraphicsWebViewTest.h"
#include "WindowWithEmbeddedViewTest.h"
#include "WidgetAndWebViewTest.h"
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
#include "WindowWithSeparatedDeclarativeAndWebViewsTest.h"
#endif //QT_VERSION
#endif // QT_NO_SAMPLES
#endif //WD_TEST_ENABLE_WEB_VIEW

#include "extension_qt/q_view_runner.h"
#include "extension_qt/q_session_lifecycle_actions.h"
#include "extension_qt/widget_view_creator.h"
#include "extension_qt/widget_view_enumerator.h"
#include "extension_qt/widget_view_executor.h"
#include "extension_qt/wd_event_dispatcher.h"
#include "extension_qt/vnc_event_dispatcher.h"
#include "extension_qt/uinput_event_dispatcher.h"

#ifndef QT_NO_VNC
#include "extension_qt/vncclient.h"
#endif // QT_NO_VNC
#ifndef QT_NO_UINPUT
#include "extension_qt/uinput_manager.h"
#endif // QT_NO_UINPUT
#include "webdriver_switches.h"

void setQtSettings();
void PrintVersion();
void PrintHelp();
#ifndef QT_NO_VNC
void InitVNCClient();
#endif // QT_NO_VNC
#ifndef QT_NO_UINPUT
void InitUInputClient();
#endif  // QT_NO_UINPUT

int main(int argc, char *argv[])
{
    base::AtExitManager exit;
    QApplication app(argc, argv);
    app.setQuitOnLastWindowClosed(false);
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("utf8"));
#endif

    webdriver::ViewRunner::RegisterCustomRunner<webdriver::QViewRunner>();

    webdriver::SessionLifeCycleActions::RegisterCustomLifeCycleActions<webdriver::QSessionLifeCycleActions>();

    webdriver::ViewTransitionManager::SetURLTransitionAction(new webdriver::URLTransitionAction_CloseOldView());

    /* Configure widget views */
    webdriver::ViewCreator* widgetCreator = new webdriver::QWidgetViewCreator();

    /* 
       Register view classes (here some test classes) that can be created by WebDriver. 
       Creation can be triggered by client side request like wd.get("qtwidget://WindowTestWidget"); 
    */
    widgetCreator->RegisterViewClass<QWidget>("QWidget");
#ifndef QT_NO_SAMPLES
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
#endif // QT_NO_SAMPLES

#if (WD_TEST_ENABLE_WEB_VIEW == 1)
    /* Configure web views */
    webdriver::ViewCreator* webCreator = new webdriver::QWebViewCreator();
    webCreator->RegisterViewClass<QWebViewExt>("QWebViewExt");
    webdriver::ViewFactory::GetInstance()->AddViewCreator(webCreator);
  
    /* Configure WebView support */
    webdriver::ViewEnumerator::AddViewEnumeratorImpl(new webdriver::WebViewEnumeratorImpl());
    webdriver::ViewCmdExecutorFactory::GetInstance()->AddViewCmdExecutorCreator(new webdriver::QWebViewCmdExecutorCreator());

    /* Configure GraphicsWebView support */
    webdriver::ViewEnumerator::AddViewEnumeratorImpl(new webdriver::GraphicsWebViewEnumeratorImpl());
    webdriver::ViewCmdExecutorFactory::GetInstance()->AddViewCmdExecutorCreator(new webdriver::GraphicsWebViewCmdExecutorCreator());
  
#ifndef QT_NO_SAMPLES
    /* Register som test classes */
    widgetCreator->RegisterViewClass<GraphicsWebViewTestWindows>("GraphicsWebViewTestWindows");
    widgetCreator->RegisterViewClass<WindowWithEmbeddedViewTestWidget>("WindowWithEmbeddedViewTestWidget");
    widgetCreator->RegisterViewClass<WidgetAndWebViewTestWindows>("WidgetAndWebViewTestWindows");


#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
    widgetCreator->RegisterViewClass<WindowWithSeparatedDeclarativeAndWebViewsTestWidget>("WindowWithSeparatedDeclarativeAndWebViewsTestWidget");
#endif // QT_VERSION
#endif // QT_NO_SAMPLES
#endif // WD_TEST_ENABLE_WEB_VIEW

#ifndef QT_NO_QML
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    // Quick2 extension
    webdriver::ViewCreator* qmlCreator = new webdriver::Quick2ViewCreator();
    qmlCreator->RegisterViewClass<QQuickView>("QQuickView");
    webdriver::ViewFactory::GetInstance()->AddViewCreator(qmlCreator);

    webdriver::ViewEnumerator::AddViewEnumeratorImpl(new webdriver::Quick2ViewEnumeratorImpl());

    webdriver::ViewCmdExecutorFactory::GetInstance()->AddViewCmdExecutorCreator(new webdriver::Quick2ViewCmdExecutorCreator());
#else
    // Quick1 extension
    webdriver::ViewCreator* qmlCreator = new webdriver::QQmlViewCreator();
    qmlCreator->RegisterViewClass<QDeclarativeView>("QDeclarativeView");
    webdriver::ViewFactory::GetInstance()->AddViewCreator(qmlCreator);

    webdriver::ViewEnumerator::AddViewEnumeratorImpl(new webdriver::QmlViewEnumeratorImpl());

    webdriver::ViewCmdExecutorFactory::GetInstance()->AddViewCmdExecutorCreator(new webdriver::QQmlViewCmdExecutorCreator());
    
    #if (WD_TEST_ENABLE_WEB_VIEW == 1)
    	qmlRegisterType<QDeclarativeWebSettings>();
    	qmlRegisterType<QDeclarativeWebView>("CiscoQtWebKit", 1, 0, "CiscoWebView");
    	qmlRegisterType<QDeclarativeWebView>("CiscoQtWebKit", 1, 1, "CiscoWebView");
    	qmlRegisterRevision<QDeclarativeWebView, 0>("CiscoQtWebKit", 1, 0);
    	qmlRegisterRevision<QDeclarativeWebView, 1>("CiscoQtWebKit", 1, 1);
    	//webdriver::ViewEnumerator::AddViewEnumeratorImpl(new webdriver::QmlWebViewEnumeratorImpl());
    	//webdriver::ViewCmdExecutorFactory::GetInstance()->AddViewCmdExecutorCreator(new webdriver::QmlWebViewCmdExecutorCreator());
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

    // check if --help CL argument is present
    if (cmd_line.HasSwitch("help")) {
        PrintHelp();
        return 0;
    }

    // check if --version CL argument is present
    if (cmd_line.HasSwitch("version")) {
      PrintVersion();
      return 0;
    }

#ifndef QT_NO_SAMPLES
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
    InitVNCClient();
    InitUInputClient();

    /* Start webdriver */
    int startError = wd_server->Start();
    if (startError){
        std::cout << "Error while starting server, errorCode " << startError << std::endl;
        return startError;
    }

    setQtSettings();

    return app.exec();
}

void setQtSettings() {
#if (WD_TEST_ENABLE_WEB_VIEW == 1)    
    QWebSettings::globalSettings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::JavascriptEnabled, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::JavascriptCanOpenWindows, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::JavascriptCanCloseWindows, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::LocalStorageEnabled, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::LocalContentCanAccessRemoteUrls, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::LocalContentCanAccessFileUrls, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::OfflineStorageDatabaseEnabled, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::OfflineWebApplicationCacheEnabled, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::PluginsEnabled, true);
    QWebSettings::globalSettings()->enablePersistentStorage("./web/html5");
    QWebSettings::globalSettings()->setLocalStoragePath("./web/html5");
    QWebSettings::globalSettings()->setOfflineStoragePath("./web/html5");
    QWebSettings::globalSettings()->setOfflineWebApplicationCachePath("./web/html5");
#endif    
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

void PrintVersion() {
    std::cout <<webdriver::VersionInfo::CreateVersionString()<< std::endl;
}

void PrintHelp() {
    std::cout << "Usage: WebDriver [--OPTION=VALUE]..."                                             << std::endl

                << "Starts QtWebDriver server"                                                        << std::endl
                << ""                                                                                 << std::endl
                << "OPTION         DEFAULT VALUE      DESCRIPTION"                                    << std::endl
                << "http-threads   4                  The number of threads to use for handling"      << std::endl
                << "                                  HTTP requests"                                  << std::endl
                << "log-path       ./webdriver.log    The path to use for the QtWebDriver server"     << std::endl
                << "                                  log"                                            << std::endl
                << "root           ./web              The path of location to serve files from"       << std::endl
                << "port           9517               The port that QtWebDriver listens on"           << std::endl
                << "silence        false              If true, QtWebDriver will not log anything"     << std::endl
                << "                                  to stdout/stderr"                               << std::endl
                << "verbose        false              If true, QtWebDriver will log lots of stuff"    << std::endl
                << "                                  to stdout/stderr"                               << std::endl
                << "url-base                          The URL path prefix to use for all incoming"    << std::endl
                << "                                  WebDriver REST requests. A prefix and postfix"  << std::endl
                << "                                  '/' will automatically be appended if not"      << std::endl
                << "                                  present"                                        << std::endl
                << "config                            The path to config file (e.g. config.json) in"  << std::endl
                << "                                  JSON format with specified WD parameters as"    << std::endl
                << "                                  described above (port, root, etc.)"             << std::endl
                << "wi-server      false              If true, web inspector will be enabled"         << std::endl
                << "wi-port        9222               Web inspector listening port"                   << std::endl
                << "version                           Print version information to stdout and exit"   << std::endl
                << "vnc-login      127.0.0.1:5900     VNC server login parameters"                    << std::endl
                << "                                  format: login:password@ip:port"                 << std::endl
                << "uinput         false              If option set, user input device"               << std::endl
                << "                                  will be registered in the system"               << std::endl
                << "test_data      ./                 Specifies where to look for test specific data" << std::endl
                << "whitelist                         The path to whitelist file (e.g. whitelist.xml)"<< std::endl
                << "                                  in XML format with specified list of IP with"   << std::endl
                << "                                  allowed/denied commands for each of them."      << std::endl
                << "webserver-cfg                     The path to mongoose config file"               << std::endl
                << "                                  (e.g. /path/to/config.json) in JSON format with"<< std::endl
                << "                                  specified mongoose start option"                << std::endl
                << "                                  (extra-mime-types, listening_ports, etc.)"      << std::endl
                << "                                  Option from webserver config file will have"    << std::endl
                << "                                  more priority than commandline param"           << std::endl
                << "                                  that specify the same option."                  << std::endl;
}



