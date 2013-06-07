// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WEBDRIVER_COMMANDS_URL_COMMAND_WRAPPER_H_
#define WEBDRIVER_COMMANDS_URL_COMMAND_WRAPPER_H_

#include <string>
#include <vector>

#include "commands/webdriver_command.h"

namespace webdriver {

class Response;

class WebDriverCommandWrapper : public WebDriverCommand {
public:
  	WebDriverCommandWrapper(const std::vector<std::string>& path_segments,
             const DictionaryValue* const parameters,
             WebDriverCommand* delegate);
  	virtual ~WebDriverCommandWrapper();

  	virtual bool DoesDelete() OVERRIDE;
    virtual bool DoesGet() OVERRIDE;
    virtual bool DoesPost() OVERRIDE;
    virtual bool Init(Response* const response) OVERRIDE;
    virtual void Finish(Response* const response) OVERRIDE;
    virtual void ExecuteDelete(Response* const response) OVERRIDE;
    virtual void ExecuteGet(Response* const response) OVERRIDE;
    virtual void ExecutePost(Response* const response)  OVERRIDE;
protected:
	scoped_ptr<WebDriverCommand> delegate_;    

private:
  	DISALLOW_COPY_AND_ASSIGN(WebDriverCommandWrapper);
};

class UrlCommandWrapper : public WebDriverCommandWrapper {
public:
  	UrlCommandWrapper(const std::vector<std::string>& path_segments,
             const DictionaryValue* const parameters,
             WebDriverCommand* delegate);
  	virtual ~UrlCommandWrapper();

    virtual void ExecutePost(Response* const response)  OVERRIDE;

private:
  	DISALLOW_COPY_AND_ASSIGN(UrlCommandWrapper);
};

}  // namespace webdriver

#endif  // WEBDRIVER_COMMANDS_URL_COMMAND_WRAPPER_H_
