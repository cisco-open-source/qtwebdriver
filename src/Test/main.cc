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

#include <QtGui/QWidget>
#include <QtGui/QPushButton>

#include <QtWebKit/QtWebKit>
#endif

class MainWindow : public QWidget
 {
     Q_OBJECT

 public:
     MainWindow();
     virtual ~MainWindow();

 private slots:
     void PushButtonPressed();
     void PushButtonReleased();
     void PushButtonClicked();


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

    this->resize(300, 200);
    this->setWindowTitle("WD_native");

    webView = NULL;

    connect(pushButton, SIGNAL(pressed()), this, SLOT(PushButtonPressed()));
    connect(pushButton, SIGNAL(released()), this, SLOT(PushButtonReleased()));
    connect(pushButton, SIGNAL(clicked()), this, SLOT(PushButtonClicked()));
}

MainWindow::~MainWindow()
{

}

void MainWindow::PushButtonPressed()
{
    QString msg = "!!!!!!! Test PushButton pressed !!!!!!!!";
    qDebug() << msg;
}

void MainWindow::PushButtonReleased()
{
    QString msg = "!!!!!!! Test PushButton released !!!!!!!!";
    qDebug() << msg;
}

void MainWindow::PushButtonClicked()
{
    QString msg = "!!!!!!! Test PushButton clicked !!!!!!!!";
    qDebug() << msg;

    if (NULL == webView)
    {
        webView = new QWebView();
        webView->setWindowTitle(QString("WD"));
        webView->show();
    }

    webView->load(QUrl(textEdit->text()));
}

#include <WebDriver.h>

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


    regitsterView<QWebView>("QWebView");
    regitsterView<QWidget>("QWidget");
    regitsterView<MainWindow>("MainNativeWindow");
//    MainWindow mainWindow;
//    mainWindow.show();

    QFutureWatcher<int> watcher;
    QObject::connect(&watcher, SIGNAL(finished()), qApp, SLOT(quit()));
    QFuture<int> future = QtConcurrent::run(main_server, argc, argv);
    watcher.setFuture(future);

    return app.exec();
}


#include "moc_main.cc"
