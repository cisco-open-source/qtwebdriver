// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WEBDRIVER_COMMANDS_LOG_COMMAND_H_
#define WEBDRIVER_COMMANDS_LOG_COMMAND_H_

#include <string>
#include <vector>

#include "commands/webdriver_command.h"

namespace base {
class DictionaryValue;
}

namespace webdriver {

class Response;

class LogCommand : public ViewCommand {
public:
  	LogCommand(const std::vector<std::string>& path_segments,
             const base::DictionaryValue* parameters);
  	virtual ~LogCommand();

  	virtual bool DoesPost() const OVERRIDE;
  	virtual void ExecutePost(Response* const response) OVERRIDE;

private:
  	DISALLOW_COPY_AND_ASSIGN(LogCommand);
};

class LogTypesCommand : public WebDriverCommand {
public:
  	LogTypesCommand(const std::vector<std::string>& path_segments,
             const base::DictionaryValue* parameters);
  	virtual ~LogTypesCommand();

  	virtual bool DoesGet() const OVERRIDE;
  	virtual void ExecuteGet(Response* const response) OVERRIDE;

private:
  	DISALLOW_COPY_AND_ASSIGN(LogTypesCommand);
};

}  // namespace webdriver

#endif  // WEBDRIVER_COMMANDS_LOG_COMMAND_H_
