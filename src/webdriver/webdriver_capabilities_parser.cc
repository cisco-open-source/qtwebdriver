// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "webdriver_capabilities_parser.h"

#include "base/file_util.h"
#include "base/format_macros.h"
#include "base/stringprintf.h"
#include "base/string_util.h"
#include "base/values.h"
#include "webdriver_switches.h"
#include "webdriver_error.h"
#include "webdriver_logging.h"
#include "webdriver_util.h"

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

Capabilities::Capabilities()
    : options(CommandLine::NO_PROGRAM),
      load_async(false),
      native_events(false),
      proxy(0) {
    log_levels[LogType::kDriver] = kAllLogLevel;
}

Capabilities::~Capabilities() { }

CapabilitiesParser::CapabilitiesParser(
    const DictionaryValue* capabilities_dict,
    const Logger& logger,
    Capabilities* capabilities)
    : dict_(capabilities_dict),
      logger_(logger),
      caps_(capabilities) {
}

CapabilitiesParser::~CapabilitiesParser() { }

Error* CapabilitiesParser::Parse() {
    // Parse WebDriver standard capabilities.
    typedef Error* (CapabilitiesParser::*Parser)(const Value*);

    struct NameAndParser {
        const char* name;
        Parser parser;
    };

    NameAndParser name_and_parser[] = {
        { "proxy", &CapabilitiesParser::ParseProxy },
        { "loggingPrefs", &CapabilitiesParser::ParseLoggingPrefs },
    };

    for (size_t i = 0; i < ARRAYSIZE_UNSAFE(name_and_parser); ++i) {
        const Value* value;
        if (dict_->Get(name_and_parser[i].name, &value)) {
            Error* error = (this->*name_and_parser[i].parser)(value);
            if (error)
                return error;
        }
    }

    std::map<std::string, Parser> parser_map;
  
    parser_map["loadAsync"] = &CapabilitiesParser::ParseLoadAsync;
    parser_map["nativeEvents"] = &CapabilitiesParser::ParseNativeEvents;
    parser_map["browserStartWindow"] = &CapabilitiesParser::ParseBrowserStartWindow;
    parser_map["browserClass"] = &CapabilitiesParser::ParseBrowserClass;

    DictionaryValue::key_iterator key_iter = dict_->begin_keys();
    for (; key_iter != dict_->end_keys(); ++key_iter) {
        if (parser_map.find(*key_iter) == parser_map.end()) {
            return new Error(kBadRequest, "Unrecognized capability: " +  *key_iter);
        }

        const Value* option = NULL;
        dict_->GetWithoutPathExpansion(*key_iter, &option);
        Error* error = (this->*parser_map[*key_iter])(option);
        if (error) {
            error->AddDetails(base::StringPrintf(
                "Error occurred while processing capability '%s'",
                (*key_iter).c_str()));
            return error;
        }
    }
    return NULL;
}

Error* CapabilitiesParser::ParseLoadAsync(const Value* option) {
    if (!option->GetAsBoolean(&caps_->load_async))
        return CreateBadInputError("loadAsync", Value::TYPE_BOOLEAN, option);
    return NULL;
}

Error* CapabilitiesParser::ParseLoggingPrefs(const base::Value* option) {
    const DictionaryValue* logging_prefs;
    if (!option->GetAsDictionary(&logging_prefs))
        return CreateBadInputError("loggingPrefs", Value::TYPE_DICTIONARY, option);

    DictionaryValue::key_iterator key_iter = logging_prefs->begin_keys();
    for (; key_iter != logging_prefs->end_keys(); ++key_iter) {
        LogType log_type;
        if (!LogType::FromString(*key_iter, &log_type))
            continue;

        const Value* level_value;
        logging_prefs->Get(*key_iter, &level_value);
        std::string level_name;
        if (!level_value->GetAsString(&level_name)) {
            return CreateBadInputError(
                std::string("loggingPrefs.") + *key_iter,
                Value::TYPE_STRING,
                level_value);
        }
        caps_->log_levels[log_type.type()] = LogLevelFromString(level_name);
    }
    return NULL;
}

Error* CapabilitiesParser::ParseNativeEvents(const Value* option) {
    if (!option->GetAsBoolean(&caps_->native_events))
        return CreateBadInputError("nativeEvents", Value::TYPE_BOOLEAN, option);
    return NULL;
}

Error* CapabilitiesParser::ParseProxy(const base::Value* option) {
    const DictionaryValue* options;
    if (!option->GetAsDictionary(&options))
        return CreateBadInputError("proxy", Value::TYPE_DICTIONARY, option);

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

    DictionaryValue::key_iterator key_iter = options->begin_keys();
    for (; key_iter != options->end_keys(); ++key_iter) {
        if (proxy_options.find(*key_iter) == proxy_options.end()) {
            logger_.Log(kInfoLogLevel, "Unrecognized proxy capability: " + *key_iter);
        }
    }

    typedef Error* (CapabilitiesParser::*Parser)(const DictionaryValue*);
    std::map<std::string, Parser> proxy_type_parser_map;
    proxy_type_parser_map["autodetect"] =
        &CapabilitiesParser::ParseProxyAutoDetect;
    proxy_type_parser_map["pac"] =
        &CapabilitiesParser::ParseProxyAutoconfigUrl;
    proxy_type_parser_map["manual"] = &CapabilitiesParser::ParseProxyServers;
    proxy_type_parser_map["direct"] = NULL;
    proxy_type_parser_map["system"] = NULL;

    const Value* proxy_type_value;
    if (!options->Get("proxyType", &proxy_type_value))
        return new Error(kBadRequest, "Missing 'proxyType' capability.");

    std::string proxy_type;
    if (!proxy_type_value->GetAsString(&proxy_type))
        return CreateBadInputError("proxyType", Value::TYPE_STRING,
                               proxy_type_value);

    proxy_type = StringToLowerASCII(proxy_type);
    if (proxy_type_parser_map.find(proxy_type) == proxy_type_parser_map.end())
        return new Error(kBadRequest, "Unrecognized 'proxyType': " + proxy_type);

    if (proxy_type == "direct") {
        caps_->options.AppendSwitch(Switches::kNoProxyServer);
    } else if (proxy_type == "system") {
        // Chrome default.
    } else {
        Error* error = (this->*proxy_type_parser_map[proxy_type])(options);
        if (error) {
            error->AddDetails("Error occurred while processing 'proxyType': " +
                            proxy_type);
            return error;
        }
    }
    caps_->proxy = options->DeepCopy();
    return NULL;
}

Error* CapabilitiesParser::ParseProxyAutoDetect(
    const DictionaryValue* options) {
    const char kProxyAutoDetectKey[] = "autodetect";
    bool proxy_auto_detect = false;
    if (!options->GetBoolean(kProxyAutoDetectKey, &proxy_auto_detect))
        return CreateBadInputError(kProxyAutoDetectKey,
                               Value::TYPE_BOOLEAN, options);
    if (proxy_auto_detect)
        caps_->options.AppendSwitch(Switches::kProxyAutoDetect);
    return NULL;
}

Error* CapabilitiesParser::ParseProxyAutoconfigUrl(
    const DictionaryValue* options){
    const char kProxyAutoconfigUrlKey[] = "proxyAutoconfigUrl";
    CommandLine::StringType proxy_pac_url;
    if (!options->GetString(kProxyAutoconfigUrlKey, &proxy_pac_url))
        return CreateBadInputError(kProxyAutoconfigUrlKey,
                               Value::TYPE_STRING, options);
    caps_->options.AppendSwitchNative(Switches::kProxyPacUrl, proxy_pac_url);
    return NULL;
}

Error* CapabilitiesParser::ParseProxyServers(
    const DictionaryValue* options) {
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

    std::vector<std::string> proxy_servers;
    std::set<std::string>::const_iterator iter = proxy_servers_options.begin();
    for (; iter != proxy_servers_options.end(); ++iter) {
        if (options->Get(*iter, &option) && !option->IsType(Value::TYPE_NULL)) {
            std::string value;
            if (!option->GetAsString(&value))
                return CreateBadInputError(*iter, Value::TYPE_STRING, option);
            has_manual_settings = true;
            // Converts into Chrome proxy scheme.
            // Example: "http=localhost:9000;ftp=localhost:8000".
            if (*iter == kFtpProxy)
                value = "ftp=" + value;
            if (*iter == kHttpProxy)
                value = "http=" + value;
            if (*iter == kSslProxy)
                value = "https=" + value;
            proxy_servers.push_back(value);
        }
    }

    if (!has_manual_settings)
        return new Error(kBadRequest, "proxyType is 'manual' but no manual "
                                  "proxy capabilities were found.");

    std::string proxy_server_value = JoinString(proxy_servers, ';');
    caps_->options.AppendSwitchASCII(Switches::kProxyServer, proxy_server_value);

    return NULL;
}

Error* CapabilitiesParser::ParseNoProxy(const base::Value* option){
    std::string proxy_bypass_list;
    if (!option->GetAsString(&proxy_bypass_list))
        return CreateBadInputError("noProxy", Value::TYPE_STRING, option);
    //if (!proxy_bypass_list.empty())
    //caps_->options.AppendSwitchASCII(Switches::kProxyBypassList,
    //                                     proxy_bypass_list);
    return NULL;
}

Error* CapabilitiesParser::ParseBrowserStartWindow(const Value* option) {
    if (!option->GetAsString(&caps_->browser_start_window))
        return CreateBadInputError("browserStartWindow", Value::TYPE_STRING, option);
    return NULL;
}

Error* CapabilitiesParser::ParseBrowserClass(const Value* option) {
    if (!option->GetAsString(&caps_->browser_class))
        return CreateBadInputError("browserClass", Value::TYPE_STRING, option);
    return NULL;
}

}  // namespace webdriver
