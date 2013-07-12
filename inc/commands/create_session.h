// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WEBDRIVER_COMMANDS_CREATE_SESSION_H_
#define WEBDRIVER_COMMANDS_CREATE_SESSION_H_

#include <string>
#include <vector>

#include "commands/command.h"

namespace webdriver {

class Response;
class Session;
class Error;
class ViewId;
class Rect;

/// Create a new session which is a new instance of the chrome browser with no
/// page loaded.  A new session ID is passed back to the user which is used for
/// all future commands that are sent to control this new instance.  The
/// desired capabilities should be specified in a JSON object with the
/// following properties:
/// http://code.google.com/p/selenium/wiki/JsonWireProtocol#/session
class CreateSession : public Command {
public:
  	CreateSession(const std::vector<std::string>& path_segments,
                	const DictionaryValue* const parameters);
  	virtual ~CreateSession();

  	virtual bool DoesPost() const OVERRIDE;
  	virtual void ExecutePost(Response* const response) OVERRIDE;

private:
	Error* SwitchToView(Session* session, const ViewId& viewId);
	Error* GetViewTitle(Session* session, const ViewId& viewId, std::string* title);
	Error* SetWindowBounds(const DictionaryValue* desired_caps_dict,Session* session, ViewId startView);
	bool FindAndAttachView(Session* session, const std::string& name, ViewId* viewId);
	bool CreateViewByClassName(Session* session, const std::string& name, ViewId* viewId);
	
  	DISALLOW_COPY_AND_ASSIGN(CreateSession);
};

}  // namespace webdriver

#endif  // WEBDRIVER_COMMANDS_CREATE_SESSION_H_
