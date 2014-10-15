// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

/*! \page page_hybrid_capabilities Hybrid WD capabilities

<h2>Start window capabilities</h2>
There are two capabilities which define strategy what view will be current after session create:
- "browserStartWindow" - HWD will search all existed views and attach to one with same title. Or if this capablities equals "*" then to first one.
- "browserClass" - HWD will call ViewFactory and create window registered with specified classname.
- "windowsize" - HWD will be try create window with given size, as a string (e.g. - "600, 600")
- "windowposition" - HWD will be try start window with desired position, as a string (e.g. - "80, 60")
- "maximize" - HWD will be try start window with maximum size
- "reuseUI" - HWD checks this caps, if it is not specified,
in case of attempt to create second session we get exception of "one session only",<br/>
otherwise prev session will be terminated without closing windows and new session can reuse those windows

For browserClass customizer can define some generic classes. In example in default 
QT extension there is handling of "WidgetView" and "WebView" values for this capability.
In this case ViewFactory will create first registered class of related view's type.

Both options can be passed in desired and required capabilities.

*/

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

    static const char kBrowserName[];
    static const char kVersion[];
    static const char kPlatform[];
    static const char kJavascriptEnabled[];
    static const char kTakesScreenshot[];
    static const char kTakesElementScreenshot[];
    static const char kRemotePlayerEnabled[];
    static const char kHandlesAlerts[];
    static const char kDatabaseEnabled[];
    static const char kLocationContextEnabled[];
    static const char kApplicationCacheEnabled[];
    static const char kBrowserConnectionEnabled[];
    static const char kCssSelectorsEnabled[];
    static const char kWebStorageEnabled[];
    static const char kRotatable[];
    static const char kAcceptSslCerts[];
    static const char kNativeEvents[];
    static const char kProxy[];
    static const char kLoggingPrefs[];
    static const char kLoadAsync[];
    static const char kBrowserStartWindow[];
    static const char kBrowserClass[];
    static const char kHybrid[];
    /// desired size, as a string (e.g. - "600, 600") for start window
    static const char kWindowSize[];
    /// desired position, as a string (e.g. - "80, 60") for start window
    static const char kWindowPosition[];
    /// start window with maximum size
    static const char kMaximize[];

    static const char kReuseUI[];

    Capabilities();
    ~Capabilities();

    /// options with switches.
    CommandLine options;

    /// Whether Chrome should not block when loading.
    bool load_async;

    /// The minimum level to log for each log type.
    LogLevel log_levels[LogType::kNum];

    /// Name of the window to use for connecting to an already running QWidget subclass.
    std::string browser_start_window;

    /// Name of the WebView class to create as "browser"
    std::string browser_class;

    /// Name of the browser
    std::string browser_name;

    /// capabilities dictionary
    scoped_ptr<DictionaryValue> caps;
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
    Error* ParseBrowserStartWindow(const base::Value* option);
    Error* ParseBrowserClass(const base::Value* option);

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
