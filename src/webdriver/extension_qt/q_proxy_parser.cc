// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "q_proxy_parser.h"

#include "base/format_macros.h"
#include "base/stringprintf.h"
#include "base/string_util.h"
#include "base/values.h"
#include "webdriver_error.h"
#include "webdriver_logging.h"
#include "webdriver_util.h"

#include <QtNetwork/QNetworkProxy>
#include <QtCore/QUrl>


using base::DictionaryValue;
using base::Value;

namespace webdriver {

namespace {

Error* CreateBadInputError(const std::string& name,
                           Value::Type type,
                           const Value* option) {
    return new Error(kBadRequest, base::StringPrintf(
        "%s must be of type %s, not %s. Received: %s",
        name.c_str(), GetJsonTypeName(type),
        GetJsonTypeName(option->GetType()),
        JsonStringifyForDisplay(option).c_str()));
}

}  // namespace

QProxyCapabilitiesParser::QProxyCapabilitiesParser(
            const base::DictionaryValue* proxy_dict,
            const Logger& logger,
            QNetworkProxy* proxy) 
            : dict_(proxy_dict),
              logger_(logger),
              proxy_(proxy) {}


QProxyCapabilitiesParser::~QProxyCapabilitiesParser() { }

Error* QProxyCapabilitiesParser::Parse() {
    // Quick check of proxy capabilities.
    std::set<std::string> proxy_options;
    proxy_options.insert("autodetect");
    proxy_options.insert("ftpProxy");
    proxy_options.insert("httpProxy");
    proxy_options.insert("noProxy");
    proxy_options.insert("proxyType");
    proxy_options.insert("proxyAutoconfigUrl");
    proxy_options.insert("sslProxy");
    proxy_options.insert("class");  // Created by BeanToJSONConverter.

    DictionaryValue::key_iterator key_iter = dict_->begin_keys();
    for (; key_iter != dict_->end_keys(); ++key_iter) {
        if (proxy_options.find(*key_iter) == proxy_options.end()) {
            logger_.Log(kInfoLogLevel, "Unrecognized proxy capability: " + *key_iter);
        }
    }

    typedef Error* (QProxyCapabilitiesParser::*Parser)(const DictionaryValue*);
    std::map<std::string, Parser> proxy_type_parser_map;
    proxy_type_parser_map["autodetect"] = &QProxyCapabilitiesParser::ParseProxyAutoDetect;
    proxy_type_parser_map["pac"] = &QProxyCapabilitiesParser::ParseProxyAutoconfigUrl;
    proxy_type_parser_map["manual"] = &QProxyCapabilitiesParser::ParseProxyServers;
    proxy_type_parser_map["direct"] = NULL;
    proxy_type_parser_map["system"] = &QProxyCapabilitiesParser::ParseSystemProxy;

    const Value* proxy_type_value;
    if (!dict_->Get("proxyType", &proxy_type_value))
        return new Error(kBadRequest, "Missing 'proxyType' capability.");

    std::string proxy_type;
    if (!proxy_type_value->GetAsString(&proxy_type))
        return CreateBadInputError("proxyType", Value::TYPE_STRING, proxy_type_value);

    proxy_type = StringToLowerASCII(proxy_type);
    if (proxy_type_parser_map.find(proxy_type) == proxy_type_parser_map.end())
        return new Error(kBadRequest, "Unrecognized 'proxyType': " + proxy_type);

    if (proxy_type == "direct") {
        logger_.Log(kInfoLogLevel, "found noProxy configuration.");
        *proxy_ = QNetworkProxy(QNetworkProxy::NoProxy);
    } else {
        Error* error = (this->*proxy_type_parser_map[proxy_type])(dict_);
        if (error) {
            error->AddDetails("Error occurred while processing 'proxyType': " +
                            proxy_type);
            return error;
        }
    }
    
    return NULL;
}

Error* QProxyCapabilitiesParser::ParseProxyAutoDetect(const DictionaryValue* options) {
    const char kProxyAutoDetectKey[] = "autodetect";
    bool proxy_auto_detect = false;
    if (!options->GetBoolean(kProxyAutoDetectKey, &proxy_auto_detect))
        return CreateBadInputError(kProxyAutoDetectKey,
                               Value::TYPE_BOOLEAN, options);
    
    logger_.Log(kWarningLogLevel, "Proxy \"autodetect\" option is not supported.");

    return NULL;
}

Error* QProxyCapabilitiesParser::ParseProxyAutoconfigUrl(const DictionaryValue* options) {
    //const char kProxyAutoconfigUrlKey[] = "proxyAutoconfigUrl";

    logger_.Log(kWarningLogLevel, "Proxy \"proxyAutoconfigUrl\" option is not supported.");

    return NULL;
}

Error* QProxyCapabilitiesParser::ParseProxyServers(const DictionaryValue* options) {
    const char kNoProxy[] = "noProxy";
    const char kFtpProxy[] = "ftpProxy";
    const char kHttpProxy[] = "httpProxy";
    const char kSslProxy[] = "sslProxy";

    std::set<std::string> proxy_servers_options;
    proxy_servers_options.insert(kFtpProxy);
    proxy_servers_options.insert(kHttpProxy);
    proxy_servers_options.insert(kSslProxy);

    Error* error = NULL;
    const Value* option = NULL;
    bool has_manual_settings = false;
    if (options->Get(kNoProxy, &option) && !option->IsType(Value::TYPE_NULL)) {
        error = ParseNoProxy(option);
        if (error)
            return error;
        has_manual_settings = true;
    }

    std::set<std::string>::const_iterator iter = proxy_servers_options.begin();
    for (; iter != proxy_servers_options.end(); ++iter) {
        if (options->Get(*iter, &option) && !option->IsType(Value::TYPE_NULL)) {
            std::string value;
            if (!option->GetAsString(&value))
                return CreateBadInputError(*iter, Value::TYPE_STRING, option);

            has_manual_settings = true;
            
            if (*iter == kFtpProxy) {
                logger_.Log(kWarningLogLevel, "Proxy \"ftpProxy\" option is not supported.");
                continue;
            }
              
            if (*iter == kSslProxy) {
                logger_.Log(kWarningLogLevel, "Proxy \"sslProxy\" option is not supported.");
                continue;
            }

            if (*iter == kHttpProxy) {
                QString urlString(value.c_str());

                if (!urlString.startsWith("http://"))
                    urlString.insert(0, "http://");

                QUrl proxyUrl(urlString);
                if (proxyUrl.isValid() && !proxyUrl.host().isEmpty()) {
                    int proxyPort = (proxyUrl.port() > 0) ? proxyUrl.port() : 8080;

                    logger_.Log(kInfoLogLevel, "found httpProxy configuration - " + proxyUrl.toString().toStdString());
                    *proxy_ = QNetworkProxy(QNetworkProxy::HttpProxy, proxyUrl.host(), proxyPort);
                }
            }
        }
    }

    if (!has_manual_settings)
        return new Error(kBadRequest, "proxyType is 'manual' but no manual "
                                  "proxy capabilities were found.");

    return NULL;
}

Error* QProxyCapabilitiesParser::ParseNoProxy(const base::Value* option){
    std::string proxy_bypass_list;
    if (!option->GetAsString(&proxy_bypass_list))
        return CreateBadInputError("noProxy", Value::TYPE_STRING, option);

    logger_.Log(kInfoLogLevel, "found noProxy configuration.");

    *proxy_ = QNetworkProxy(QNetworkProxy::NoProxy);
    
    return NULL;
}

Error* QProxyCapabilitiesParser::ParseSystemProxy(const base::DictionaryValue *options) {

    QString urlString(qgetenv("http_proxy"));
    if (urlString == NULL) {
        logger_.Log(kInfoLogLevel,"system proxy configuration not found");
        return NULL;
    }

    if (!urlString.startsWith("http://"))
        urlString.insert(0, "http://");
    QUrl proxyUrl(urlString);

    if (!proxyUrl.isValid()) {
        return new Error(kBadRequest,"Proxy url invalid");
    }
    if (proxyUrl.host().isEmpty()) {
        return new Error(kBadRequest,"Proxy host is empty");
    }

    int proxyPort = (proxyUrl.port() > 0) ? proxyUrl.port() : 8080;
    logger_.Log(kInfoLogLevel, "found system proxy configuration - "+ proxyUrl.toString().toStdString());
    *proxy_ = QNetworkProxy(QNetworkProxy::HttpProxy, proxyUrl.host(), proxyPort);
    return NULL;
}

}  // namespace webdriver
