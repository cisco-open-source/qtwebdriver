#include "q_content_type_resolver.h"
#include "webdriver_error.h"
#include "webdriver_logging.h"

#include <QtCore/QEventLoop>
#include <QtCore/QString>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWebKitWidgets/QWebView>
#else
#include <QtWebKit/QtWebKit>
#endif

namespace webdriver {

QContentTypeResolver::QContentTypeResolver(QNetworkAccessManager *pmanager)
    : manager_(pmanager) {}

QContentTypeResolver::~QContentTypeResolver() {}

Error* QContentTypeResolver::resolveContentType(const std::string& url, std::string& mimetype) {
    QEventLoop loop;
    QObject::connect(manager_, SIGNAL(finished(QNetworkReply*)),
            &loop,SLOT(quit()));

    Error *error = NULL;
    QNetworkRequest request;
    request.setUrl(QUrl(QString::fromStdString(url)));
    QNetworkReply *reply = manager_->head(request);
    if (NULL == reply) {
        error = new Error(kBadRequest);
        GlobalLogger::Log(kWarningLogLevel, " Invalid Request ");
        return error;
    }

    if(reply->isRunning()) {
        loop.exec();
    }

    QNetworkReply::NetworkError err = reply->error();
    if (err) {
        error = new Error(kBadRequest);
        GlobalLogger::Log(kWarningLogLevel, "NetworkError : " + QString::number(err).toStdString());
        return error;
    }

    QVariant contentMimeType  = reply->header(QNetworkRequest::ContentTypeHeader);
    if (!contentMimeType.isValid()) {
        error = new Error(kBadRequest);
        GlobalLogger::Log(kWarningLogLevel, " ContentMimeType invalid ");
        return error;
    }

    QString qmimetype = contentMimeType.toString();
    int index = qmimetype.indexOf(";");
    if (index != -1) {
        qmimetype.remove(index, qmimetype.length()-index);
    }

    if (qmimetype.isEmpty()) {
        error = new Error(kBadRequest);
        GlobalLogger::Log(kWarningLogLevel, " ContentMimeType is empty ");
        return error;
    }

    mimetype = qmimetype.toStdString();
    return error;
}

}
