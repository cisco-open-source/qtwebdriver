// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

/// \page page_webdriver_switches WD Server switches
/// Switches that can be passed to webdriver::Server::Init()
///

#ifndef WEBDRIVER_WEBDRIVER_SWITCHES_H_
#define WEBDRIVER_WEBDRIVER_SWITCHES_H_

namespace webdriver {

class Switches {
public:
	/// \page page_webdriver_switches WD Server switches
	/// - <b>url-base</b><br>
	/// The URL path prefix to use for all incoming
	/// WebDriver REST requests. A prefix and postfix
	/// '/' will be appended automatically if not present
    static const char kUrlBase[];

    /// \page page_webdriver_switches WD Server switches
    /// - <b>port</b><br>
    /// The port that QtWebDriver listens on 
    /// (default - 9571)
    static const char kPort[];

    /// \page page_webdriver_switches WD Server switches
    /// - <b>root</b><br>
    /// The path of location to serve files from
    /// (default - ./web)
    static const char kRoot[];

    /// \page page_webdriver_switches WD Server switches
    /// - <b>http-threads</b><br>
    /// The number of threads to use for handling HTTP requests
    /// (default - 4)
    static const char kHttpThread[];

    /// \page page_webdriver_switches WD Server switches
    /// - <b>log-path</b><br>
    /// The path to use for the QtWebDriver server log
    /// (default - ./webdriver.log)
    static const char kLogPath[];

    /// \page page_webdriver_switches WD Server switches
    /// - <b>verbose</b><br>
    /// If enabled, QtWebDriver will log lots of stuff to stdout/stderr (false by default)
    static const char kVerbose[];

    /// \page page_webdriver_switches WD Server switches
    /// - <b>silence</b><br>
    /// If enabled, QtWebDriver will not log anything to stdout/stderr (false by default)
    static const char kSilence[];

    /// \page page_webdriver_switches WD Server switches
    /// - <b>config</b><br>
    /// The path to config file (e.g. config.json) in
    /// JSON format with specified WD parameters as
    /// described above (port, root, etc.)
    static const char kConfig[];

    /// \page page_webdriver_switches WD Server switches
    /// - <b>wi-server</b><br>
    /// If true, web inspector will be enabled (false by default)
    static const char kWIServer[];

    /// \page page_webdriver_switches WD Server switches
    /// - <b>wi-port</b><br>
    /// Web inspector listening port (by default - 9222)
    static const char kWIPort[];

    /// \page page_webdriver_switches WD Server switches
    /// - <b>vnc-login</b><br>
    /// VNC server listening port (by default - 5900)
    /// VNC server IP address (by default - 127.0.0.1)
    /// Password for authentication (by default - no password)
    static const char kVNCLogin[];

    /// \page page_webdriver_switches WD Server switches
    /// - <b>uinput</b><br>
    /// if parameter specified, user input device enabled
    static const char kUserInputDevice[];

    /// \page page_webdriver_switches WD Server switches
    /// - <b>white-list</b><br>
    /// The path to whitelist file (e.g. whitelist.xml) in
    /// XML format with specified list of IP with allowed/denied
    /// commands for each of them. For more information please refer:
    ///\ref page_whitelist

    static const char kWhiteList[];

    /// \page page_webdriver_switches WD Server switches
    /// - <b>webserver-cfg</b><br>
    /// The path to mongoose config file (e.g. config.json) in
    /// JSON format with specified mongoose start option
    /// (extra-mime-types, listening_ports, etc.)
    /// Option from webserver config file will have more priority than
    /// commandline param that specify the same option.

    static const char kWebServerCfg[];

};

}  // namespace webdriver

#endif  // WEBDRIVER_WEBDRIVER_SWITCHES_H_
