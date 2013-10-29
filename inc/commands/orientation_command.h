// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ORIENTATION_COMMAND_H
#define ORIENTATION_COMMAND_H

#include <string>
#include <vector>

#include "commands/webdriver_command.h"

namespace base {
class DictionaryValue;
}

namespace webdriver {

class Response;

/// Set and Get the current browser orientation
/// The server should return a valid orientation value as defined in ScreenOrientation: {LANDSCAPE|PORTRAIT}.
/// http://code.google.com/p/selenium/wiki/JsonWireProtocol#/session/:sessionId/orientation
class OrientationCommand : public ViewCommand {
public:
    OrientationCommand(const std::vector<std::string>& path_segments,
                    const base::DictionaryValue* const parameters);
    virtual ~OrientationCommand();

    virtual bool DoesGet() const OVERRIDE;
    virtual bool DoesPost() const OVERRIDE;

    virtual void ExecuteGet(Response* const response) OVERRIDE;
    virtual void ExecutePost(Response* const response) OVERRIDE;

private:
    DISALLOW_COPY_AND_ASSIGN(OrientationCommand);
};

} //webdriver

#endif // ORIENTATION_COMMAND_H
