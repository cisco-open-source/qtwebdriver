// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WEBDRIVER_COMMANDS_APPCACHE_STATUS_COMMAND_H_
#define WEBDRIVER_COMMANDS_APPCACHE_STATUS_COMMAND_H_

#include <string>
#include <vector>

#include "commands/webdriver_command.h"

namespace base {
class DictionaryValue;
}

namespace webdriver {

class Response;

class AppCacheStatusCommand : public ViewCommand {
public:
  	AppCacheStatusCommand(const std::vector<std::string>& path_segments,
                        const base::DictionaryValue* const parameters);
  	virtual ~AppCacheStatusCommand();

  	virtual bool DoesGet() const OVERRIDE;

  	// A call with HTTP GET will return the status of the appcahe.
  	virtual void ExecuteGet(Response* const response) OVERRIDE;

private:
  	DISALLOW_COPY_AND_ASSIGN(AppCacheStatusCommand);
};

}  // namespace webdriver

#endif  // WEBDRIVER_COMMANDS_APPCACHE_STATUS_COMMAND_H_
