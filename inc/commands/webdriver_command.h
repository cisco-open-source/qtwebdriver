// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WEBDRIVER_COMMANDS_WEBDRIVER_COMMAND_H_
#define WEBDRIVER_COMMANDS_WEBDRIVER_COMMAND_H_

#include <string>
#include <vector>

#include "commands/command.h"

namespace base {
class DictionaryValue;
}

namespace webdriver {

class Response;
class Session;
class ViewCmdExecutor;

/// All URLs that are found in the document:
/// http://code.google.com/p/selenium/wiki/JsonWireProtocol
/// and are to be supported for all browsers and platforms should inhert
/// this class.  For cases which do not invlove interaction with the
/// browser, such a transfering a file, inhert from the Command class
/// directly.
class WebDriverCommand : public Command {
public:
    WebDriverCommand(const std::vector<std::string>& path_segments,
                    const base::DictionaryValue* const parameters);
    virtual ~WebDriverCommand();

    /// Initializes this webdriver command by fetching the command session.
    virtual bool Init(Response* const response) OVERRIDE;

    virtual void Finish(Response* const response) OVERRIDE;

    /// Returns whether this command should run the session pre and post
    /// command handlers. These handlers include waiting for the page to load.
    virtual bool ShouldRunPreAndPostCommandHandlers();

protected:
    Session* session_;
    std::string session_id_;

    DISALLOW_COPY_AND_ASSIGN(WebDriverCommand);
};

/// Handles commands that interact with current view.
class ViewCommand : public WebDriverCommand {
public:
    ViewCommand(const std::vector<std::string>& path_segments,
                    const base::DictionaryValue* const parameters);
    virtual ~ViewCommand();

    virtual bool Init(Response* const response) OVERRIDE;

protected:
    scoped_ptr<ViewCmdExecutor> executor_;

private:
    DISALLOW_COPY_AND_ASSIGN(ViewCommand);
};

}  // namespace webdriver

#endif  // WEBDRIVER_COMMANDS_WEBDRIVER_COMMAND_H_
