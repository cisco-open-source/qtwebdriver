#ifndef WEB_VIEW_VISUALIZER_H
#define WEB_VIEW_VISUALIZER_H

#include <QtCore/QtGlobal>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWebKitWidgets/QWebView>
#else
#include <QtWebKit/QtWebKit>
#endif

#include "third_party/pugixml/pugixml.hpp"

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
    QSharedPointer<pugi::xml_document> ParseXml(const QString& input, Error** error);
    std::string Tidy(const std::string& input, Error** error) const;
    static void UnescapeXml(QString& input);

    void AssemblePage(pugi::xml_node element) const;
    void AssembleLink(pugi::xml_node element) const;
    void AssembleImg(pugi::xml_node element) const;
    void AssembleStyle(pugi::xml_node element) const;
    void AssembleStyle(pugi::xml_attribute attribute) const;
    QString AssembleStyle(const QString& value) const;
    void RemoveScripts(pugi::xml_node element) const;

    QString AbsoluteUrl(const QString& url) const;
    void Download(const QString& url, QByteArray* buffer, QString* contentType) const;
    QString DownloadAndEncode(const QString& url) const;

private Q_SLOTS:
    void DownloadFinished();

private:
    static bool isEmpty(const pugi::xml_object_range<pugi::xml_node_iterator>& range);
    static void clearChildren(pugi::xml_node element);
    static const char DATA_PROTOCOL[];

    QWebViewCmdExecutor* executor_;
    Session* session_;
    QWebView* view_;
};

class QWebViewVisualizerShowPointCommand {
public:
    QWebViewVisualizerShowPointCommand(QWebViewCmdExecutor* executor, Session* session, QWebView* view);

    void Execute(Error** error);

private:
    QWebViewCmdExecutor* executor_;
    Session* session_;
    QWebView* view_;
};

}  // namespace webdriver

#endif // WEB_VIEW_VISUALIZER_H
