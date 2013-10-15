#include "q_content_type_resolver.h"
#include "webdriver_error.h"
#include "webdriver_logging.h"

#include <QtCore/QEventLoop>
#include <QtCore/QString>

#include <QtNetwork/QNetworkReply>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtCore/QMimeDatabase>
#include <QtCore/QMimeType>
#endif

namespace webdriver {

QContentTypeResolver::QContentTypeResolver(QNetworkAccessManager *pmanager)
    : manager_(pmanager) {}

QContentTypeResolver::~QContentTypeResolver() {}

Error* QContentTypeResolver::resolveContentType(const std::string& url, std::string& mimetype) {
    QUrl contentUrl(QString::fromStdString(url));
    QString scheme = contentUrl.scheme();
    QString qmimetype;
    Error* error = NULL;

    if ( (0 == scheme.compare("http", Qt::CaseInsensitive)) ||
         (0 == scheme.compare("https", Qt::CaseInsensitive)) ) {

        QEventLoop loop;
        QObject::connect(manager_, SIGNAL(finished(QNetworkReply*)),
                &loop,SLOT(quit()));

        QNetworkRequest request;
        request.setUrl(contentUrl);

        QNetworkReply *reply = manager_->head(request);
        if (NULL == reply) {
            GlobalLogger::Log(kWarningLogLevel, "QContentTypeResolver::resolveContentType() : invalid Request ");
            return new Error(kBadRequest);
        }

        if(reply->isRunning()) {
            loop.exec();
        }

        QNetworkReply::NetworkError err = reply->error();
        if (err) {
            GlobalLogger::Log(kWarningLogLevel, "QContentTypeResolver::resolveContentType() : NetworkError : " + QString::number(err).toStdString());
            return new Error(kBadRequest);
        }

        QVariant contentMimeType  = reply->header(QNetworkRequest::ContentTypeHeader);
        if (!contentMimeType.isValid()) {
            GlobalLogger::Log(kWarningLogLevel, "QContentTypeResolver::resolveContentType() : ContentMimeType invalid ");
            return new Error(kBadRequest);
        }

        qmimetype = contentMimeType.toString();
        int index = qmimetype.indexOf(";");
        if (index != -1) {
            qmimetype.remove(index, qmimetype.length()-index);
        }

        if (qmimetype.isEmpty()) {
            GlobalLogger::Log(kWarningLogLevel, "QContentTypeResolver::resolveContentType() : ContentMimeType is empty ");
            return new Error(kBadRequest);
        }
    } else {
        // non http schemes
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        QMimeDatabase mimeDatabase;
        QMimeType mimeType;
        mimeType = mimeDatabase.mimeTypeForUrl(contentUrl);

        qmimetype = mimeType.name();
#else
        // TODO: implement for qt4
#endif    

    }

    mimetype = qmimetype.toStdString();

    GlobalLogger::Log(kInfoLogLevel, "QContentTypeResolver::resolveContentType() : content type: " + mimetype);

    return error;
}

}
