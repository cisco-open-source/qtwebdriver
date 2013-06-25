// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "commands/appcache_status_command.h"

#include <string>

#include "base/bind.h"

#include "commands/response.h"
#include "webdriver_error.h"
#include "webdriver_session.h"
#include "webdriver_view_executor.h"

namespace webdriver {

AppCacheStatusCommand::AppCacheStatusCommand(
    const std::vector<std::string>& path_segments,
    const DictionaryValue* const parameters)
    : ViewCommand(path_segments, parameters) {}

AppCacheStatusCommand::~AppCacheStatusCommand() {}

bool AppCacheStatusCommand::DoesGet() const {
    return true;
}

void AppCacheStatusCommand::ExecuteGet(Response* const response) {
    int status;
    Error* error = NULL;

    session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::GetAppCacheStatus,
            base::Unretained(executor_.get()),
            &status,
            &error));

    if (error) {
        response->SetError(error);
        return;
    }
    response->SetValue(new base::FundamentalValue(status));
}

}  // namespace webdriver
