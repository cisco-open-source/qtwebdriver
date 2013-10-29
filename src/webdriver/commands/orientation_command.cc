// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "commands/orientation_command.h"

#include <string>
#include <vector>

#include "base/memory/scoped_ptr.h"
#include "base/string_split.h"
#include "base/string_util.h"
#include "base/values.h"
#include "base/bind.h"
#include "commands/response.h"
#include "webdriver_session.h"
#include "webdriver_view_executor.h"

namespace webdriver {

OrientationCommand::OrientationCommand(const std::vector<std::string>& path_segments,
                             const DictionaryValue* const parameters)
    : ViewCommand(path_segments, parameters) {}

OrientationCommand::~OrientationCommand() {}


bool OrientationCommand::DoesGet() const {
    return true;
}

bool OrientationCommand::DoesPost() const {
    return true;
}

void OrientationCommand::ExecuteGet(Response* const response) {
    std::string orientation;
    Error* error = NULL;

    session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::GetOrientation,
            base::Unretained(executor_.get()),
            &orientation,
            &error));

    if (error) {
        response->SetError(error);
        return;
    }

    response->SetValue(Value::CreateStringValue(orientation));

}

void OrientationCommand::ExecutePost(Response* const response) {

    std::string orientation;
    if (!GetStringParameter("orientation", &orientation)) {
        response->SetError(new Error(
            kBadRequest, "'orientation' is missing or invalid"));
        return;
    }

    Error* error = NULL;

    session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::SetOrientation,
            base::Unretained(executor_.get()),
            orientation,
            &error));

    if (error) {
        response->SetError(error);
        return;
    }
}

} // namespace webdriver
