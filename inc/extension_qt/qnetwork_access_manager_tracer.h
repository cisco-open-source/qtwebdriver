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

#ifndef NETWORK_ACCESS_MANAGER_H
#define NETWORK_ACCESS_MANAGER_H

#include <string>
#include <QtNetwork/QNetworkAccessManager>

namespace webdriver {
class Session;
}

/// Auxiliary class for performance logging support.<br>
/// Limitation: It is currently not supported to change the network access manager after the QWebPage has used it.
class QNetworkAccessManagerTracer: public QNetworkAccessManager {
    Q_OBJECT
public:   
    /// Create custom network access manager.
    /// @param session pointer to Session object.
    /// @param parent set as the parent object.
    QNetworkAccessManagerTracer(webdriver::Session* session, QObject* parent = 0);

    virtual ~QNetworkAccessManagerTracer();

protected:
    /// Overrided, additional write a JSON entry for every received reply.
    virtual QNetworkReply* createRequest(Operation op, const QNetworkRequest& req, QIODevice* outgoingData = 0);

protected slots:
    /// Create single log entry and add it to session's performance log.
    /// <br>Currently WebDriver supports only Performance Log of Network.
    /// <p>Each entry is a JSON string of the following structure:
    /// @code
    /// {
    ///     "webview": "class_name",
    ///     "message": {
    ///         "args": {
    ///             "file": "file_path/name",
    ///             "method": "HTTP_method",
    ///             "status": "HTTP_status_code",
    ///         },
    ///         "tid": "thread_id",
    ///         "ts": "timestamp",
    ///         "tts": "thread-specific_CPU-time"
    ///     }
    /// }
    /// @endcode
    /// @param reply pointer to reply, that contains the data which will be written to PerfLog.
    void writeReply(QNetworkReply* reply);

private:
    std::string getMethod(Operation op);
    webdriver::Session* session_;
    double timeStamp_;
};

#endif //NETWORK_ACCESS_MANAGER_H
