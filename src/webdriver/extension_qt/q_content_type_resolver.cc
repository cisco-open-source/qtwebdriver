/****************************************************************************
**
** Copyright © 1992-2014 Cisco and/or its affiliates. All rights reserved.
** All rights reserved.
** 
** $CISCO_BEGIN_LICENSE:LGPL$
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** $CISCO_END_LICENSE$
**
****************************************************************************/

#include "q_content_type_resolver.h"
#include "webdriver_error.h"
#include "webdriver_logging.h"

#include <QtCore/QEventLoop>
#include <QtCore/QString>

#include <QtNetwork/QNetworkReply>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtCore/QMimeDatabase>
#include <QtCore/QMimeType>
#else
#include "third_party/mimetypes-qt4/include/QMimeDatabase"
#include "third_party/mimetypes-qt4/include/QMimeType"
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

            // network-related error was detected
            if (err == QNetworkReply::UnknownNetworkError)
                return error;
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
        QMimeDatabase mimeDatabase;
        QMimeType mimeType;
        mimeType = mimeDatabase.mimeTypeForUrl(contentUrl);

        qmimetype = mimeType.name();
    }

    mimetype = qmimetype.toStdString();

    GlobalLogger::Log(kInfoLogLevel, "QContentTypeResolver::resolveContentType() : content type: " + mimetype);

    return error;
}

}
