#ifndef Q_CONTENT_TYPE_RESOLVER_H
#define Q_CONTENT_TYPE_RESOLVER_H

#include <string>
#include <QtNetwork/QNetworkAccessManager>

namespace webdriver {

class Error;

class QContentTypeResolver
{
public:
    QContentTypeResolver(QNetworkAccessManager *pmanager);
    ~QContentTypeResolver();

    Error* resolveContentType(const std::string& url, std::string& mimetype);

private:
    QNetworkAccessManager *manager_;

};

}

#endif // Q_CONTENT_TYPE_RESOLVER_H
