// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WEBDRIVER_COMMANDS_NON_SESSION_COMMANDS_H_
#define WEBDRIVER_COMMANDS_NON_SESSION_COMMANDS_H_

#include <string>
#include <vector>

#include "commands/command.h"

namespace base {
class DictionaryValue;
}

namespace webdriver {

class Response;

/// Sends status
/// http://code.google.com/p/selenium/wiki/JsonWireProtocol#/status
class StatusCommand : public Command {
public:
  	StatusCommand(const std::vector<std::string>& path_segments,
                    const base::DictionaryValue* const parameters);
  	virtual ~StatusCommand();

  	virtual bool DoesGet() const OVERRIDE;

  	/// A call with HTTP GET will return the status.
  	virtual void ExecuteGet(Response* const response) OVERRIDE;

private:
  	DISALLOW_COPY_AND_ASSIGN(StatusCommand);
};

/// Sends global logs
class GlobalLogCommand : public Command {
public:
  	GlobalLogCommand(const std::vector<std::string>& path_segments,
                    const base::DictionaryValue* const parameters);
  	virtual ~GlobalLogCommand();

  	virtual bool DoesGet() const OVERRIDE;

  	virtual void ExecuteGet(Response* const response) OVERRIDE;

private:
  	DISALLOW_COPY_AND_ASSIGN(GlobalLogCommand);
};

}  // namespace webdriver

#endif  // WEBDRIVER_COMMANDS_NON_SESSION_COMMANDS_H_
