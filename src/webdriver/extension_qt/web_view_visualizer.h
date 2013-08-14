#ifndef WEB_VIEW_VISUALIZER_H
#define WEB_VIEW_VISUALIZER_H

#include <QtCore/QtGlobal>
#include <QtXml/QDomDocument>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWebKitWidgets/QWebView>
#else
#include <QtWebKit/QtWebKit>
#endif

namespace webdriver {

class QWebViewCmdExecutor;
class Error;
class Session;

class QWebViewVisualizerSourceCommand : public QObject {
    Q_OBJECT

public:
    QWebViewVisualizerSourceCommand(QWebViewCmdExecutor* executor, Session* session, QWebView* view);

    void Execute(std::string* source, Error** error);

private:
    static QSharedPointer<QDomDocument> ParseXml(const QString& input, Error** error);
    static void UnescapeXml(QString& input);

    void AssemblePage(QDomElement element) const;
    void AssembleLink(QDomElement element) const;
    void AssembleImg(QDomElement element) const;
    void AssembleStyle(QDomElement element) const;
    void AssembleStyle(QDomAttr attribute) const;
    QString AssembleStyle(const QString& value) const;
    void RemoveScripts(QDomElement element) const;

    QString AbsoluteUrl(const QString& url) const;
    void Download(const QString& url, QByteArray* buffer, QString* contentType) const;
    QString DownloadAndEncode(const QString& url) const;

    static QString trimmed(const QString& str, const QString& symbols);

private Q_SLOTS:
    void DownloadFinished();

private:
    static const char DATA_PROTOCOL[];

    QWebViewCmdExecutor* executor_;
    Session* session_;
    QWebView* view_;
};

}  // namespace webdriver

#endif // WEB_VIEW_VISUALIZER_H
