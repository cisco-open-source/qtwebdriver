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

typedef scoped_ptr<ViewCmdExecutor> ExecutorPtr;    

URLCommand::URLCommand(const std::vector<std::string>& path_segments,
                       const DictionaryValue* const parameters)
    : WebDriverCommand(path_segments, parameters) {}

URLCommand::~URLCommand() {}

bool URLCommand::DoesGet() const {
    return true;
}

bool URLCommand::DoesPost() const {
    return true;
}

void URLCommand::ExecuteGet(Response* const response) {
    std::string url;
    Error* error = NULL;

    ExecutorPtr executor(ViewCmdExecutorFactory::GetInstance()->CreateExecutor(session_));
    if (NULL == executor.get()) {
        response->SetError(new Error(kBadRequest, "cant get view executor."));
        return;
    }

    session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::GetURL,
            base::Unretained(executor.get()),
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

    ExecutorPtr executor(ViewCmdExecutorFactory::GetInstance()->CreateExecutor(session_));
    if (NULL == executor.get()) {
        response->SetError(new Error(kBadRequest, "cant get view executor."));
        return;
    }

    if (!GetStringASCIIParameter("url", &url)) {
        response->SetError(new Error(kBadRequest, "Missing 'url' parameter"));
        return;
    }

    // TODO: get capabilities and choose sync/async method
    session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::NavigateToURL,
            base::Unretained(executor.get()),
            url,
            true,
            &error));

    if (error) {
        response->SetError(error);
        return;
    }
}


}  // namespace webdriver
