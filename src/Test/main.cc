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

#include "base/at_exit.h"
#include "webdriver_server.h"
#include "webdriver_view_transitions.h"
#include "versioninfo.h"

#if (WD_TEST_ENABLE_WEB_VIEW == 1)
#include "extension_qt/web_view_creator.h"
#include "extension_qt/web_view_executor.h"
#include "extension_qt/web_view_enumerator.h"
#include "extension_qt/qwebviewext.h"
#endif

#include "extension_qt/q_view_runner.h"
#include "extension_qt/q_session_lifecycle_actions.h"
#include "extension_qt/widget_view_creator.h"
#include "extension_qt/widget_view_enumerator.h"
#include "extension_qt/widget_view_executor.h"
#include "extension_qt/wd_event_dispatcher.h"
#include "extension_qt/vnc_event_dispatcher.h"

#include "vnc/vncclient.h"
#include "webdriver_switches.h"

void setQtSettings();
void PrintVersion();
void PrintHelp();

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

#if (WD_TEST_ENABLE_WEB_VIEW == 1)    
    webdriver::ViewCreator* webCreator = new webdriver::QWebViewCreator();
    webCreator->RegisterViewClass<QWebViewExt>("QWebViewExt");
    webdriver::ViewFactory::GetInstance()->AddViewCreator(webCreator);

    webdriver::ViewEnumerator::AddViewEnumeratorImpl(new webdriver::WebViewEnumeratorImpl());

    webdriver::ViewCmdExecutorFactory::GetInstance()->AddViewCmdExecutorCreator(new webdriver::QWebViewCmdExecutorCreator());

#endif    

    webdriver::ViewCreator* widgetCreator = new webdriver::QWidgetViewCreator();
    widgetCreator->RegisterViewClass<QWidget>("QWidget");
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

#if defined(OS_WIN)
#if (QT_VERSION == QT_VERSION_CHECK(5, 1, 0))
    system("qtenv2.bat vsvars");
#else  //QT_VERSION
    system("qtvars.bat vsvars");
#endif //QT_VERSION
#endif //OS_WIN

    webdriver::Server* wd_server = webdriver::Server::GetInstance();
    if (0 != wd_server->Configure(cmd_line)) {
        return 1;
    }

    VNCClient *client = new VNCClient();
    client->Init("http://127.0.0.1", 5900);


    CommandLine cmdLine = webdriver::Server::GetInstance()->GetCommandLine();

    if (cmdLine.HasSwitch(webdriver::Switches::kVNCEnabled))
    {
        WDEventDispatcher::getInstance()->add(new VNCEventDispatcher(client));
    }
	
	setQtSettings();
    wd_server->Start();

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
                << "version                           Print version information to stdout and exit"   << std::endl;
}



