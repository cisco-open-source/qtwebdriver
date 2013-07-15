// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WEBDRIVER_WEBDRIVER_SWITCHES_H_
#define WEBDRIVER_WEBDRIVER_SWITCHES_H_

namespace webdriver {

class Switches {
public:
    /// The URL path prefix to use for all incoming
    /// WebDriver REST requests. A prefix and postfix
    /// '/' will automatically be appended if not present
    static const char kUrlBase[];

    /// The port that QtWebDriver listens on
    /// by default - 9571
    static const char kPort[];

    /// The path of location to serve files from
    /// by default - ./web
    static const char kRoot[];

    /// The number of threads to use for handling HTTP requests
    /// by default - 4
    static const char kHttpThread[];

    /// The path to use for the QtWebDriver server log
    /// by default - ./webdriver.log
    static const char kLogPath[];

    /// If true, QtWebDriver will log lots of stuff to stdout/stderr (false by default)
    static const char kVerbose[];

    /// If true, QtWebDriver will not log anything to stdout/stderr (false by default)
    static const char kSilence[];

    ///  The path to config file (e.g. config.json) in
    /// JSON format with specified WD parameters as
    /// described above (port, root, etc.)
    static const char kConfig[];

    /// If true, web inspector will be enabled (false by default)
    static const char kWIServer[];

    /// Web inspector listening port (by default - 9222)
    static const char kWIPort[];

    /// if "--help" CL argument is present will print help to stdout and exit
    static const char kHelp[];

    /// if "--version" CL argument is present will
    /// print version information to stdout and exit
    static const char kVersion[];
};

}  // namespace webdriver

#endif  // WEBDRIVER_WEBDRIVER_SWITCHES_H_
