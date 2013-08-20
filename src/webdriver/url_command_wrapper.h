// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WEBDRIVER_COMMANDS_URL_COMMAND_WRAPPER_H_
#define WEBDRIVER_COMMANDS_URL_COMMAND_WRAPPER_H_

#include <string>
#include <vector>

#include "commands/command.h"

namespace webdriver {

class Response;

class CommandWrapper : public Command {
public:
    CommandWrapper(Command* delegate);
  	virtual ~CommandWrapper();

  	virtual bool DoesDelete() const OVERRIDE;
    virtual bool DoesGet() const OVERRIDE;
    virtual bool DoesPost() const OVERRIDE;
    virtual bool Init(Response* const response) OVERRIDE;
    virtual void Finish(Response* const response) OVERRIDE;
    virtual void ExecuteDelete(Response* const response) OVERRIDE;
    virtual void ExecuteGet(Response* const response) OVERRIDE;
    virtual void ExecutePost(Response* const response)  OVERRIDE;
protected:
	scoped_ptr<Command> delegate_;    

private:
  	DISALLOW_COPY_AND_ASSIGN(CommandWrapper);
};

class UrlCommandWrapper : public CommandWrapper {
public:
    UrlCommandWrapper(Command* delegate);
  	virtual ~UrlCommandWrapper();

    virtual void ExecutePost(Response* const response)  OVERRIDE;

private:
  	DISALLOW_COPY_AND_ASSIGN(UrlCommandWrapper);
};

}  // namespace webdriver

#endif  // WEBDRIVER_COMMANDS_URL_COMMAND_WRAPPER_H_
