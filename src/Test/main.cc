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

#include <WebDriver.h>

#if defined(_WIN32)
#include <windows.h>
#include <signal.h>
#endif

#include "ServerThread.h"

ServerThread* pServerThread;
 
#if defined(_WIN32)
BOOL CtrlHandler(DWORD fdwCtrlType)
{
  switch (fdwCtrlType)
  {
    // Handle the CTRL-C signal.
    case CTRL_BREAK_EVENT:
    case CTRL_C_EVENT:
      qDebug() << "try to stop thread";
      pServerThread->terminate();
      
      while (!pServerThread->isFinished()) {}
 
      if (!pServerThread->isRunning())
        qDebug() << "thread is stopped";

      return TRUE;
      
    default:
      return FALSE;
  }
}
#endif //_WIN32

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

//    registerView<QWebView>("QWebView");
#if defined(_WIN32)
    SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE);
#endif //_WIN32

    //QFutureWatcher<int> watcher;
    //QObject::connect(&watcher, SIGNAL(finished()), qApp, SLOT(quit()));
    //QFuture<int> future = QtConcurrent::run(main_server, argc, argv);
    //watcher.setFuture(future);
    pServerThread = new ServerThread;
    QObject::connect(pServerThread, SIGNAL(finished()), qApp, SLOT(quit()));

    pServerThread->argc = argc;
    pServerThread->argv = argv;
    pServerThread->start();
       
    return app.exec();
}

