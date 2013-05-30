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

#include <webdriver_server.h>
#include <extension_qt/web_view_creator.h>


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

    //registerView<QWebView>("QWebView");
    //registerView<QWidget>("QWidget");
    //registerView<WindowTestWidget>("WindowTestWidget");
    //registerView<ClickTestWidget>("ClickTestWidget");
    //registerView<ElementAttributeTestWidget>("ElementAttributeTestWidget");
    //registerView<FindingTestWidget>("FindingTestWidget");
    //registerView<CoordinatesTestWidget>("CoordinatesTestWidget");
    //registerView<ClickScrollingTest>("ClickScrollingTest");

    //QFutureWatcher<int> watcher;
    //QObject::connect(&watcher, SIGNAL(finished()), qApp, SLOT(quit()));
    //QFuture<int> future = QtConcurrent::run(main_server, argc, argv);
    //watcher.setFuture(future);

    webdriver::ViewCreator* webCreator = new webdriver::QWebViewCreator();
    webCreator->RegisterViewClass<QWebView>("QWebView");
    webdriver::ViewFactory::GetInstance()->AddViewCreator(webCreator);



    webdriver::Server wd_server;
    if (0 != wd_server.Init(argc, argv)) 
    {
        return 1;
    }

    wd_server.Start();

    return app.exec();
}
