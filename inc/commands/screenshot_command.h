// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WEBDRIVER_COMMANDS_SCREENSHOT_COMMAND_H_
#define WEBDRIVER_COMMANDS_SCREENSHOT_COMMAND_H_

#include <string>
#include <vector>

#include "commands/webdriver_command.h"

namespace base {
class DictionaryValue;
}

namespace webdriver {

class Response;

/// Take a screenshot of the current page.  See:
/// http://code.google.com/p/selenium/wiki/JsonWireProtocol#/session/:sessionId/screenshot
class ScreenshotCommand : public ViewCommand {
public:
  	ScreenshotCommand(const std::vector<std::string>& path_segments,
                    const base::DictionaryValue* const parameters);
  	virtual ~ScreenshotCommand();

  	virtual bool DoesGet() const OVERRIDE;
  	virtual void ExecuteGet(Response* const response) OVERRIDE;

private:
  	DISALLOW_COPY_AND_ASSIGN(ScreenshotCommand);
};

}  // namespace webdriver

#endif  // WEBDRIVER_COMMANDS_SCREENSHOT_COMMAND_H_
