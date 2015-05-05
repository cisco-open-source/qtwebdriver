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

const char Capabilities::kBrowserName[]                 = "browserName";
const char Capabilities::kVersion[]                     = "version";
const char Capabilities::kPlatform[]                    = "platform";
const char Capabilities::kJavascriptEnabled[]           = "javascriptEnabled";
const char Capabilities::kTakesScreenshot[]             = "takesScreenshot";
const char Capabilities::kTakesElementScreenshot[]      = "takesElementScreenshot";
const char Capabilities::kRemotePlayerEnabled[]         = "remotePlayerEnabled";
const char Capabilities::kHandlesAlerts[]               = "handlesAlerts";
const char Capabilities::kDatabaseEnabled[]             = "databaseEnabled";
const char Capabilities::kLocationContextEnabled[]      = "locationContextEnabled";
const char Capabilities::kApplicationCacheEnabled[]     = "applicationCacheEnabled";
const char Capabilities::kBrowserConnectionEnabled[]    = "browserConnectionEnabled";
const char Capabilities::kCssSelectorsEnabled[]         = "cssSelectorsEnabled";
const char Capabilities::kWebStorageEnabled[]           = "webStorageEnabled";
const char Capabilities::kRotatable[]                   = "rotatable";
const char Capabilities::kAcceptSslCerts[]              = "acceptSslCerts";
const char Capabilities::kNativeEvents[]                = "nativeEvents";
const char Capabilities::kProxy[]                       = "proxy";
const char Capabilities::kLoggingPrefs[]                = "loggingPrefs";
const char Capabilities::kLoadAsync[]                   = "loadAsync";
const char Capabilities::kBrowserStartWindow[]          = "browserStartWindow";
const char Capabilities::kBrowserClass[]                = "browserClass";
const char Capabilities::kWindowSize[]                  = "windowsize";
const char Capabilities::kWindowPosition[]              = "windowposition";
const char Capabilities::kMaximize[]                    = "maximize";
const char Capabilities::kHybrid[]                      = "hybrid";
const char Capabilities::kReuseUI[]                     = "reuseUI";

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
      caps(new DictionaryValue()) {
    log_levels[LogType::kDriver] = kAllLogLevel;
    log_levels[LogType::kBrowser] = kAllLogLevel;
    // disabled by default
    log_levels[LogType::kPerformance] = kOffLogLevel;
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
        { Capabilities::kLoggingPrefs, &CapabilitiesParser::ParseLoggingPrefs },
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
  
    parser_map[Capabilities::kLoadAsync] = &CapabilitiesParser::ParseLoadAsync;
    parser_map[Capabilities::kBrowserStartWindow] = &CapabilitiesParser::ParseBrowserStartWindow;
    parser_map[Capabilities::kBrowserClass] = &CapabilitiesParser::ParseBrowserClass;

    DictionaryValue::key_iterator key_iter = dict_->begin_keys();
    for (; key_iter != dict_->end_keys(); ++key_iter) {
        if (parser_map.find(*key_iter) == parser_map.end()) {
            logger_.Log(kFineLogLevel, "Capability: " +  *key_iter + " skipped by parser.");
            continue;
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

    //caps_->caps.reset(dict_->DeepCopy());

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

Error* CapabilitiesParser::ParseBrowserStartWindow(const Value* option) {
    if (!option->GetAsString(&caps_->browser_start_window))
        return CreateBadInputError(Capabilities::kBrowserStartWindow, Value::TYPE_STRING, option);
    return NULL;
}

Error* CapabilitiesParser::ParseBrowserClass(const Value* option) {
    if (!option->GetAsString(&caps_->browser_class))
        return CreateBadInputError(Capabilities::kBrowserClass, Value::TYPE_STRING, option);
    return NULL;
}

}  // namespace webdriver
