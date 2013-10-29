// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WEBDRIVER_COMMANDS_SESSION_WITH_ID_H_
#define WEBDRIVER_COMMANDS_SESSION_WITH_ID_H_

#include <string>
#include <vector>

#include "commands/command.h"
#include "commands/webdriver_command.h"

namespace webdriver {

class Response;
class ViewId;

/// Retrieve the capabilities of the specified session. If the HTTP Delete
/// method is used then all chrome instances linked to the session ID are
/// closed.  The session's capabilities will be returned in a JSON object
/// with the following properties:
/// http://code.google.com/p/selenium/wiki/JsonWireProtocol#/session/:sessionId
/// If the HTTP Delete request is sent the session is closed and all temp
/// files and directories created are deleted.
class SessionWithID : public WebDriverCommand {
public:
  	SessionWithID(const std::vector<std::string>& path_segments,
                const DictionaryValue* const parameters);
  	virtual ~SessionWithID();

  	virtual bool DoesGet() const OVERRIDE;
  	virtual bool DoesDelete() const OVERRIDE;

  	virtual void ExecuteGet(Response* const response) OVERRIDE;
  	virtual void ExecuteDelete(Response* const response) OVERRIDE;

  	virtual bool ShouldRunPreAndPostCommandHandlers() OVERRIDE;

private:
	  void CloseView(const ViewId& viewId);

  	DISALLOW_COPY_AND_ASSIGN(SessionWithID);
};

}  // namespace webdriver

#endif  // WEBDRIVER_COMMANDS_SESSION_WITH_ID_H_
