// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WEBDRIVER_Q_PROXY_CAPABILITIES_PARSER_H_
#define WEBDRIVER_Q_PROXY_CAPABILITIES_PARSER_H_

#include <string>
#include <vector>

#include "base/basictypes.h"
#include "webdriver_logging.h"

#include <QtNetwork/QNetworkProxy>

namespace base {
class DictionaryValue;
class Value;
}

namespace webdriver {

class Error;

class QProxyCapabilitiesParser {
public:
    QProxyCapabilitiesParser(const base::DictionaryValue* proxy_dict,
                        const Logger& logger,
                        QNetworkProxy* proxy);
    ~QProxyCapabilitiesParser();

    Error* Parse();

private:
    Error* ParseNoProxy(const base::Value* option);
    Error* ParseProxy(const base::Value* option);
    Error* ParseProxyAutoDetect(const base::DictionaryValue* options);
    Error* ParseProxyAutoconfigUrl(const base::DictionaryValue* options);
    Error* ParseProxyServers(const base::DictionaryValue* options);
    Error* ParseSystemProxy(const base::DictionaryValue* options);

    const base::DictionaryValue* dict_;
    const Logger& logger_;

    // A pointer to the capabilities to modify while parsing.
    QNetworkProxy* proxy_;

    DISALLOW_COPY_AND_ASSIGN(QProxyCapabilitiesParser);
};

}  // namespace webdriver

#endif  // WEBDRIVER_Q_PROXY_CAPABILITIES_PARSER_H_
