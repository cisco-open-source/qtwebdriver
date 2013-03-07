#include <QtGui/QApplication>
#include <QtWebKit/QWebSettings>
#include <QtCore/QTextCodec>
#include <MhapWebDriver.h>
#include <QtCore/QFuture>
#include <QtCore/QObject>
#include <QtCore/QFutureWatcher>
#include <QtCore/QtConcurrentRun>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setQuitOnLastWindowClosed(false);
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("utf8"));
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

    QFutureWatcher<int> watcher;
    QObject::connect(&watcher, SIGNAL(finished()), qApp, SLOT(quit()));
    QFuture<int> future = QtConcurrent::run(main_server, argc, argv);
    watcher.setFuture(future);

    return app.exec();
}
