// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WEBDRIVER_WEBDRIVER_SWITCHES_H_
#define WEBDRIVER_WEBDRIVER_SWITCHES_H_

namespace webdriver {

class Switches {
public:
	/// Instructs the mongoose webserver to enable HTTP persistent
	/// connections.
	static const char kEnableKeepAlive[];

	/// Forces proxy auto-detection.
	static const char kProxyAutoDetect[];

	/// Uses the pac script at the given URL
	static const char kProxyPacUrl[];

	/// Uses a specified proxy server, overrides system settings. This switch only
	/// affects HTTP and HTTPS requests.
	static const char kProxyServer[];

	/// Don't use a proxy server, always make direct connections. Overrides any
	/// other proxy server flags that are passed.
	static const char kNoProxyServer[];
};

}  // namespace webdriver

#endif  // WEBDRIVER_WEBDRIVER_SWITCHES_H_
