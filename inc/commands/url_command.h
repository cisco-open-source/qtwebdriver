// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

/*! \page page_url_post "URL post" for hybrid applications

For hybrid applications there can be several different view types.
And each view type can support only specific content. In example web view
supports html content, widget view supports UI forms.

HWD has next algorithm of loading new URL in current view:
- check if current view supports URL. 
- if YES then apply loading new URL in current view and finish.
- otherwise (NO case) - HWD core uses webdriver::ViewFactory::CreateViewForUrl() to create new view that can handle URL.
- switch to new view and apply loading URL on it.
- do some actions with old view

"some actions with old view" can be customized with webdriver::URLTransitionAction.
Customizer implements own behavior and registers it in webdriver::ViewTransitionManager.
\code
webdriver::ViewTransitionManager::SetURLTransitionAction(new webdriver::URLTransitionAction_CloseOldView());
\endcode

*/
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
class URLCommand : public WebDriverCommand {
public:
  	URLCommand(const std::vector<std::string>& path_segments,
             const DictionaryValue* const parameters);
  	virtual ~URLCommand();

  	virtual bool DoesGet() const OVERRIDE;
  	virtual bool DoesPost() const OVERRIDE;
  	virtual void ExecuteGet(Response* const response) OVERRIDE;
  	virtual void ExecutePost(Response* const response) OVERRIDE;

private:
  	DISALLOW_COPY_AND_ASSIGN(URLCommand);
};

}  // namespace webdriver

#endif  // WEBDRIVER_COMMANDS_URL_COMMAND_H_
