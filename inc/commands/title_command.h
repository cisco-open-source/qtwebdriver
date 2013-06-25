// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WEBDRIVER_COMMANDS_TITLE_COMMAND_H_
#define WEBDRIVER_COMMANDS_TITLE_COMMAND_H_

#include <string>
#include <vector>

#include "commands/webdriver_command.h"

namespace webdriver {

class Response;

/// A call with HTTP GET will return the title of the tab. See:
/// http://code.google.com/p/selenium/wiki/JsonWireProtocol#/session/:sessionId/title
class TitleCommand : public ViewCommand {
public:
  	TitleCommand(const std::vector<std::string>& path_segments,
               const DictionaryValue* const parameters);
  	virtual ~TitleCommand();

  	virtual bool DoesGet() const OVERRIDE;
  	virtual void ExecuteGet(Response* const response) OVERRIDE;

private:
  	DISALLOW_COPY_AND_ASSIGN(TitleCommand);
};

}  // namespace webdriver

#endif  // WEBDRIVER_COMMANDS_TITLE_COMMAND_H_
