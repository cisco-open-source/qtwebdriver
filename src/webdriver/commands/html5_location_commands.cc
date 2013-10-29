// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "commands/html5_location_commands.h"

#include "base/values.h"
#include "base/bind.h"
#include "commands/response.h"
#include "webdriver_error.h"
#include "webdriver_session.h"
#include "webdriver_view_executor.h"

namespace webdriver {

HTML5LocationCommand::HTML5LocationCommand(
    const std::vector<std::string>& path_segments,
    const base::DictionaryValue* const parameters)
    : ViewCommand(path_segments, parameters) {}

HTML5LocationCommand::~HTML5LocationCommand() {}

bool HTML5LocationCommand::DoesGet() const {
    return true;
}

bool HTML5LocationCommand::DoesPost() const {
    return true;
}

void HTML5LocationCommand::ExecuteGet(Response* const response) {
    base::DictionaryValue* geolocation;
    Error* error = NULL;
    session_->RunSessionTask(base::Bind(
                                 &ViewCmdExecutor::GetGeoLocation,
                                 base::Unretained(executor_.get()),
                                 &geolocation,
                                 &error));
    if (error) {
        response->SetError(error);
        return;
    }

    response->SetValue(geolocation);
}

void HTML5LocationCommand::ExecutePost(Response* const response) {
    const base::DictionaryValue* geolocation;
    if (!GetDictionaryParameter("location", &geolocation)) {
        response->SetError(new Error(
        kBadRequest, "Missing or invalid 'location'"));
        return;
    }
    Error* error = NULL;
    session_->RunSessionTask(base::Bind(
                                 &ViewCmdExecutor::SetGeoLocation,
                                 base::Unretained(executor_.get()),
                                 geolocation,
                                 &error));
    if (error)
        response->SetError(error);
}

}  // namespace webdriver
