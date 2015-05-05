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

#include "extension_qt/qnetwork_access_manager_tracer.h"
#include "webdriver_session.h"
#include "webdriver_util.h"
#include "web_view_util.h"

#include "base/values.h"
#include "base/time.h"
#include "base/threading/platform_thread.h"
#include "base/string_number_conversions.h"

#include <QtCore/QThread>
#include <QtNetwork/QNetworkReply>

QNetworkAccessManagerTracer::QNetworkAccessManagerTracer(webdriver::Session* session, QObject* parent)
    :  QNetworkAccessManager(parent), session_(session) {
}

QNetworkAccessManagerTracer::~QNetworkAccessManagerTracer() { }

QNetworkReply* QNetworkAccessManagerTracer::createRequest(QNetworkAccessManager::Operation op, const QNetworkRequest &req, QIODevice *outgoingData) {

    timeStamp_ = static_cast<double>(base::TimeTicks::NowFromSystemTraceTime().ToInternalValue());
    QNetworkReply* reply = QNetworkAccessManager::createRequest(op, req, outgoingData);
    connect(this, SIGNAL(finished(QNetworkReply*)), this, SLOT(writeReply(QNetworkReply*)));

    return reply;
}

void QNetworkAccessManagerTracer::writeReply(QNetworkReply *reply) {
    webdriver::LogLevel level = session_->GetMinPerfLogLevel();

    double thread_timestamp = static_cast<double>((base::TimeTicks::IsThreadNowSupported() ?
              base::TimeTicks::ThreadNow() : base::TimeTicks()).ToInternalValue());

    std::string file = reply->url().path().toStdString();
    // delete '/' in the beginning:
    file.erase(0, 1);

    //HTTP status code
    QVariant statusCode = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute );
    QString reason;
    if ( !statusCode.isValid() ) {
        reason = "INVALID";
        level = webdriver::LogLevelFromString("SEVERE");
    } else {
        int status = statusCode.toInt();
        if ( status != 200 ) {
            level = webdriver::LogLevelFromString("WARNING");
        } else {
            level = webdriver::LogLevelFromString("INFO");
        }
        reason = reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();
    }
    base::DictionaryValue* args_entry = new base::DictionaryValue();
    args_entry->SetString("method", getMethod(reply->operation()));
    args_entry->SetString("status", reason.toStdString());
    args_entry->SetString("file", file);

    base::DictionaryValue* message_entry = new base::DictionaryValue();
    message_entry->Set("args", args_entry);
    message_entry->SetDouble("ts", timeStamp_);
    message_entry->SetInteger("tid", static_cast<int>(base::PlatformThread::CurrentId()));
    message_entry->SetDouble("tts", thread_timestamp);

    base::DictionaryValue* entry = new base::DictionaryValue;
    std::string webview = webdriver::QWebViewUtil::getWebView(session_,session_->current_view())->metaObject()->className();
    entry->SetString("webview", webview);
    entry->Set("message", message_entry);
    std::string log_entry = webdriver::JsonStringifyForDisplay(entry);

    session_->AddPerfLogEntry(level, log_entry);
    delete entry;

}

std::string QNetworkAccessManagerTracer::getMethod(QNetworkAccessManager::Operation op) {
    std::string method;
    switch (op) {
    case QNetworkAccessManager::HeadOperation:
        method = "HEAD";
        break;
    case QNetworkAccessManager::GetOperation:
        method = "GET";
        break;
    case QNetworkAccessManager::PutOperation:
        method= "PUT";
        break;
    case QNetworkAccessManager::PostOperation:
        method = "POST";
        break;
    case QNetworkAccessManager::DeleteOperation:
        method = "DELETE";
        break;
    case QNetworkAccessManager::CustomOperation:
        method = "custom";
        break;
    default:
        method = "unsupported method";
        break;
    }
    return method;
}

