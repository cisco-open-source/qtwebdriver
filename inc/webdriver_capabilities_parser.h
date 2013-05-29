// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WEBDRIVER_WEBDRIVER_CAPABILITIES_PARSER_H_
#define WEBDRIVER_WEBDRIVER_CAPABILITIES_PARSER_H_

#include <string>
#include <vector>

#include "base/basictypes.h"
#include "base/command_line.h"
#include "base/file_path.h"
#include "base/memory/scoped_ptr.h"
#include "webdriver_logging.h"

namespace base {
class DictionaryValue;
class Value;
}

namespace webdriver {

class Error;

/// Contains all the capabilities that a user may request when starting a
/// new session.
struct Capabilities {
    Capabilities();
    ~Capabilities();

    /// options with switches.
    CommandLine options;

    /// Whether Chrome should not block when loading.
    bool load_async;

    /// The minimum level to log for each log type.
    LogLevel log_levels[LogType::kNum];

    /// Whether Chrome should simulate input events using OS APIs instead of
    /// WebKit APIs.
    bool native_events;

    /// Name of the window to use for connecting to an already running QWidget subclass.
    std::string browser_start_window;

    /// Name of the WebView class to create as "browser"
    std::string browser_class;

    /// Name of the browser
    std::string browser_name;

    /// save proxy capabilities to return in "sessions" command
    DictionaryValue *proxy;

};

/// Parses the given capabilities dictionary to produce a Capabilities
/// instance.
/// See webdriver's desired capabilities for more info.
class CapabilitiesParser {
public:
    /// Create a new parser. 
    /// @param capabilities_dict the dictionary for all of the user's desired capabilities.
    /// @param logger logger
    /// @param capabilities return value
    CapabilitiesParser(const base::DictionaryValue* capabilities_dict,
                        const Logger& logger,
                        Capabilities* capabilities);
    ~CapabilitiesParser();

    /// Parses the capabilities. May return an error.
    /// @return NULL - if ok. Error object - otherwise.
    Error* Parse();

private:
    Error* ParseLoadAsync(const base::Value* option);
    Error* ParseLoggingPrefs(const base::Value* option);
    Error* ParseNativeEvents(const base::Value* option);
    Error* ParseNoProxy(const base::Value* option);
    Error* ParseProxy(const base::Value* option);
    Error* ParseProxyAutoDetect(const base::DictionaryValue* options);
    Error* ParseProxyAutoconfigUrl(const base::DictionaryValue* options);
    Error* ParseProxyServers(const base::DictionaryValue* options);
    Error* ParseBrowserStartWindow(const base::Value* option);
    Error* ParseBrowserClass(const base::Value* option);
    Error* ParseBrowserName(const base::Value* option);

    // The capabilities dictionary to parse.
    const base::DictionaryValue* dict_;

    // Reference to the logger to use.
    const Logger& logger_;

    // A pointer to the capabilities to modify while parsing.
    Capabilities* caps_;

    DISALLOW_COPY_AND_ASSIGN(CapabilitiesParser);
};

}  // namespace webdriver

#endif  // WEBDRIVER_WEBDRIVER_CAPABILITIES_PARSER_H_
