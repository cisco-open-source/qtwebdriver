// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WEBDRIVER_COMMANDS_ALERT_COMMANDS_H_
#define WEBDRIVER_COMMANDS_ALERT_COMMANDS_H_

#include <string>
#include <vector>

#include "commands/webdriver_command.h"

namespace base {
class DictionaryValue;
}

namespace webdriver {

class Response;

/// Gets the message of the JavaScript modal dialog, or sets the prompt text.
class AlertTextCommand : public ViewCommand {
 public:
    AlertTextCommand(const std::vector<std::string>& path_segments,
                   const base::DictionaryValue* parameters);
    virtual ~AlertTextCommand();

    virtual bool DoesGet() const OVERRIDE;
    virtual bool DoesPost() const OVERRIDE;
    virtual void ExecuteGet(Response* const response) OVERRIDE;
    virtual void ExecutePost(Response* const response) OVERRIDE;

private:
    DISALLOW_COPY_AND_ASSIGN(AlertTextCommand);
};

class AcceptAlertCommand : public ViewCommand {
public:
    AcceptAlertCommand(const std::vector<std::string>& path_segments,
                     const base::DictionaryValue* parameters);
    virtual ~AcceptAlertCommand();

    virtual bool DoesPost() const OVERRIDE;
    virtual void ExecutePost(Response* const response) OVERRIDE;

private:
    DISALLOW_COPY_AND_ASSIGN(AcceptAlertCommand);
};

class DismissAlertCommand : public ViewCommand {
public:
    DismissAlertCommand(const std::vector<std::string>& path_segments,
                      const base::DictionaryValue* parameters);
    virtual ~DismissAlertCommand();

    virtual bool DoesPost() const OVERRIDE;
    virtual void ExecutePost(Response* const response) OVERRIDE;

private:
    DISALLOW_COPY_AND_ASSIGN(DismissAlertCommand);
};

}  // namespace webdriver

#endif  // WEBDRIVER_COMMANDS_ALERT_COMMANDS_H_
