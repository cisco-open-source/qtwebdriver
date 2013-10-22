// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BROWSER_CONNECTION_COMMAND_H
#define BROWSER_CONNECTION_COMMAND_H

#include "commands/webdriver_command.h"

namespace webdriver {
class Response;

/// Handles browser connection commands what set online mode.<br>
/// Limitation: setOnline() can change connection state only for currently opened windows.
/// If new view will be open now, it sets online by default (while doesn't receive a command explicitly).
class BrowserConnectionCommand : public WebDriverCommand {
public:
    BrowserConnectionCommand(const std::vector<std::string>& path_segments,
                             const base::DictionaryValue* const parameters);
    ~BrowserConnectionCommand();

    virtual bool DoesPost() const OVERRIDE;
    virtual void ExecutePost(Response* const response) OVERRIDE;
    virtual bool DoesGet() const OVERRIDE;
    virtual void ExecuteGet(Response* const response) OVERRIDE;

    DISALLOW_COPY_AND_ASSIGN(BrowserConnectionCommand);
};

}
#endif // BROWSER_CONNECTION_COMMAND_H
