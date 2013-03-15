#include <QtGui/QApplication>
#include <QtWebKit/QWebSettings>
#include <QtCore/QTextCodec>
#include <MhapWebDriver.h>
#include <QtCore/QFuture>
#include <QtCore/QObject>
#include <QtCore/QFutureWatcher>
#include <QtCore/QtConcurrentRun>
//#include <QDebug>

#include <QtGui/QWidget>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>

#include <QtWebKit/QtWebKit>


class MainWindow : public QWidget
 {
     Q_OBJECT

 public:
     MainWindow();
     virtual ~MainWindow();

 private slots:
     void testPushButton();

 private:
     QLineEdit *textEdit;
     QPushButton *pushButton;
     QWebView *webView;
 };

MainWindow::MainWindow()
{
    textEdit = new QLineEdit(this);
    textEdit->move(10, 10);
    textEdit->setAccessibleName("InputText");

    pushButton = new QPushButton(this);
    pushButton->setText("Push me");
    pushButton->setAccessibleName("PushButton");
    pushButton->move(10, 60);

    webView = new QWebView(this);
    webView->move(10, 100);
    webView->setWindowTitle(QString("WD"));

    this->resize(1024, 800);
    this->setWindowTitle("WD_native");

    connect(pushButton, SIGNAL(clicked()), this, SLOT(testPushButton()));
}

MainWindow::~MainWindow()
{

}

void MainWindow::testPushButton()
{
    qDebug() << "\n\n!!!!!!!!!!!! Test PushButton !!!!!!!!!!!!!!\n\n";
    textEdit->setText("It's alive ;-)");
}

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

    // start hybridTest window
    MainWindow mainWindow;
    mainWindow.show();

    QFutureWatcher<int> watcher;
    QObject::connect(&watcher, SIGNAL(finished()), qApp, SLOT(quit()));
    QFuture<int> future = QtConcurrent::run(main_server, argc, argv);
    watcher.setFuture(future);

    return app.exec();
}


#include "moc_main.cc"
