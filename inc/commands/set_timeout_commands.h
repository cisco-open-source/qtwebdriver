// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WEBDRIVER_COMMANDS_SET_TIMEOUT_COMMANDS_H_
#define WEBDRIVER_COMMANDS_SET_TIMEOUT_COMMANDS_H_

#include <string>
#include <vector>

#include "commands/webdriver_command.h"

namespace base {
class DictionaryValue;
}

namespace webdriver {

class Response;

/// Set the amount of time that a particular type of operation can execute (/session/*/timeouts").
/// Valid values are: "script" for script timeouts,
/// "implicit" for modifying the implicit wait timeout
/// and "page load" for setting a page load timeout.
/// E.g. - {"ms": 2000, "type": "page load"}

class SetTimeoutCommand : public WebDriverCommand {
public:
    SetTimeoutCommand(const std::vector<std::string>& path_segments,
                    const base::DictionaryValue* const parameters);
    virtual ~SetTimeoutCommand();

    virtual bool DoesPost() const OVERRIDE;
    virtual void ExecutePost(Response* const response) OVERRIDE;
    virtual Error* SetTimeout(int timeout_ms);

    /// Amount of time of operation can execute (/session/*/timeouts") by default, in ms.
    static const int DEFAULT_TIMEOUT;

private:
    Error* SetTimeout(int timeout_ms, std::string type);
    DISALLOW_COPY_AND_ASSIGN(SetTimeoutCommand);
};

/// Set timeout for asynchronous script execution (/session/*/execute_async).
class SetAsyncScriptTimeoutCommand : public SetTimeoutCommand {
public:
    SetAsyncScriptTimeoutCommand(const std::vector<std::string>& path_segments,
                               const base::DictionaryValue* const parameters);
    virtual ~SetAsyncScriptTimeoutCommand();
    virtual Error* SetTimeout(int timeout_ms) OVERRIDE;

private:
    DISALLOW_COPY_AND_ASSIGN(SetAsyncScriptTimeoutCommand);
};

/// Set the amount of time the driver should wait when searching for elements.
class ImplicitWaitCommand : public SetTimeoutCommand {
public:
    ImplicitWaitCommand(const std::vector<std::string>& path_segments,
                      const base::DictionaryValue* const parameters);
    virtual ~ImplicitWaitCommand();
    virtual Error* SetTimeout(int timeout_ms) OVERRIDE;

private:
    DISALLOW_COPY_AND_ASSIGN(ImplicitWaitCommand);
};

}  // namespace webdriver

#endif  // WEBDRIVER_COMMANDS_SET_TIMEOUT_COMMANDS_H_
