// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "commands/url_command.h"

#include <string>

#include "base/bind.h"
#include "commands/response.h"
#include "webdriver_error.h"
#include "webdriver_session.h"
#include "webdriver_view_executor.h"

namespace webdriver {

URLCommand::URLCommand(const std::vector<std::string>& path_segments,
                       const DictionaryValue* const parameters)
    : ViewWebDriverCommand(path_segments, parameters) {}

URLCommand::~URLCommand() {}

bool URLCommand::DoesGet() {
    return true;
}

bool URLCommand::DoesPost() {
    return true;
}

void URLCommand::ExecuteGet(Response* const response) {
    std::string url;
    Error* error = NULL;

    session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::GetURL,
            base::Unretained(executor_.get()),
            &url,
            &error));

    if (error) {
        response->SetError(error);
        return;
    }
    response->SetValue(new StringValue(url));
}

void URLCommand::ExecutePost(Response* const response) {
    std::string url;
    Error* error = NULL;

    if (!GetStringASCIIParameter("url", &url)) {
        response->SetError(new Error(kBadRequest, "Missing 'url' parameter"));
        return;
    }

    session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::NavigateToURL,
            base::Unretained(executor_.get()),
            url,
            &error));

    if (error) {
        response->SetError(error);
        return;
    }
}


}  // namespace webdriver
