// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WEBDRIVER_COMMANDS_URL_COMMAND_H_
#define WEBDRIVER_COMMANDS_URL_COMMAND_H_

#include <string>
#include <vector>

#include "commands/webdriver_command.h"

namespace webdriver {

class Response;

/// Controls navigate to new web pages for the current tab.  A call with
/// and HTTP GET will return the URL of the tab. See:
/// http://code.google.com/p/selenium/wiki/JsonWireProtocol#/session/:sessionId/url
class URLCommand : public ViewCommand {
public:
  	URLCommand(const std::vector<std::string>& path_segments,
             const DictionaryValue* const parameters);
  	virtual ~URLCommand();

  	virtual bool DoesGet() OVERRIDE;
  	virtual bool DoesPost() OVERRIDE;
  	virtual void ExecuteGet(Response* const response) OVERRIDE;
  	virtual void ExecutePost(Response* const response) OVERRIDE;

private:
  	DISALLOW_COPY_AND_ASSIGN(URLCommand);
};

}  // namespace webdriver

#endif  // WEBDRIVER_COMMANDS_URL_COMMAND_H_
