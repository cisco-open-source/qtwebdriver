#include <QtCore/QFuture>
#include <QtCore/QFutureWatcher>
#include <QtCore/QObject>
#include <QtCore/QTextCodec>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtConcurrent/QtConcurrentRun>
#include <QtWebKitWidgets/QWebView>
#include <QtWidgets/QApplication>
#else
#include <QtCore/QtConcurrentRun>
//#include <QDebug>

#include <QtGui/QApplication>

#include <QtWebKit/QtWebKit>

#endif

#include "WindowTest.h"
#include "ClickTest.h"
#include "ElementAttributeTest.h"
#include "FindingTest.h"
#include "CoordinatesTest.h"
#include "ClickScrollingTest.h"

#include "webdriver_server.h"
#include "extension_qt/web_view_creator.h"
#include "extension_qt/web_view_executor.h"
#include "extension_qt/web_view_enumerator.h"
#include "extension_qt/q_view_runner.h"
#include "extension_qt/qwebviewext.h"
#include "extension_qt/widget_view_creator.h"
#include "extension_qt/widget_view_enumerator.h"
#include "extension_qt/widget_view_executor.h"


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setQuitOnLastWindowClosed(false);
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("utf8"));
#endif
    QWebSettings::globalSettings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::JavascriptEnabled, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::JavascriptCanOpenWindows, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::LocalStorageEnabled, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::LocalContentCanAccessRemoteUrls, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::LocalContentCanAccessFileUrls, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::OfflineStorageDatabaseEnabled, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::OfflineWebApplicationCacheEnabled, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::PluginsEnabled, true);
    QWebSettings::globalSettings()->enablePersistentStorage("./web/html5");
    QWebSettings::globalSettings()->setLocalStoragePath("./web/html5");
    QWebSettings::globalSettings()->setOfflineStoragePath("./web/html5");
    QWebSettings::globalSettings()->setOfflineWebApplicationCachePath("./web/html5");

    //QFutureWatcher<int> watcher;
    //QObject::connect(&watcher, SIGNAL(finished()), qApp, SLOT(quit()));
    //QFuture<int> future = QtConcurrent::run(main_server, argc, argv);
    //watcher.setFuture(future);

    webdriver::ViewRunner::RegisterCustomRunner<webdriver::QViewRunner>();

    webdriver::ViewCreator* webCreator = new webdriver::QWebViewCreator();
    webCreator->RegisterViewClass<QWebViewExt>("QWebViewExt");

    webdriver::ViewCreator* widgetCreator = new webdriver::QWidgetViewCreator();
    widgetCreator->RegisterViewClass<QWidget>("QWidget");
    widgetCreator->RegisterViewClass<WindowTestWidget>("WindowTestWidget");
    widgetCreator->RegisterViewClass<ClickTestWidget>("ClickTestWidget");
    widgetCreator->RegisterViewClass<ElementAttributeTestWidget>("ElementAttributeTestWidget");
    widgetCreator->RegisterViewClass<FindingTestWidget>("FindingTestWidget");
    widgetCreator->RegisterViewClass<CoordinatesTestWidget>("CoordinatesTestWidget");
    widgetCreator->RegisterViewClass<ClickScrollingTest>("ClickScrollingTest");

    webdriver::ViewFactory::GetInstance()->AddViewCreator(webCreator);
    webdriver::ViewFactory::GetInstance()->AddViewCreator(widgetCreator);

    webdriver::ViewEnumerator::AddViewEnumeratorImpl(new webdriver::WebViewEnumeratorImpl());
    webdriver::ViewEnumerator::AddViewEnumeratorImpl(new webdriver::WidgetViewEnumeratorImpl());

    webdriver::ViewCmdExecutorFactory::GetInstance()->AddViewCmdExecutorCreator(new webdriver::QWebViewCmdExecutorCreator());

    webdriver::Server wd_server;
    if (0 != wd_server.Init(argc, argv)) {
        return 1;
    }

    wd_server.Start();

    return app.exec();
}
