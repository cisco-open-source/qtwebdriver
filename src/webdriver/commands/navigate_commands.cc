// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "commands/navigate_commands.h"

#include "commands/response.h"
#include "webdriver_error.h"
#include "webdriver_session.h"
#include "webdriver_view_executor.h"

#include "base/bind.h"

namespace webdriver {

ForwardCommand::ForwardCommand(const std::vector<std::string>& path_segments,
                 const DictionaryValue* const parameters)
      : ViewCommand(path_segments, parameters) {}

ForwardCommand::~ForwardCommand() {}

bool ForwardCommand::DoesPost() const {
    return true;
}

void ForwardCommand::ExecutePost(Response* const response) {
    Error* error = NULL;

    session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::GoForward,
            base::Unretained(executor_.get()),
            &error));

    if (error)
        response->SetError(error);
}

BackCommand::BackCommand(const std::vector<std::string>& path_segments,
              const DictionaryValue* const parameters)
      : ViewCommand(path_segments, parameters) {}

BackCommand::~BackCommand() {}

bool BackCommand::DoesPost() const {
    return true;
}

void BackCommand::ExecutePost(Response* const response) {
    Error* error = NULL;

    session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::GoBack,
            base::Unretained(executor_.get()),
            &error));

    if (error)
        response->SetError(error);
}

RefreshCommand::RefreshCommand(const std::vector<std::string>& path_segments,
                               const DictionaryValue* const parameters)
    : ViewCommand(path_segments, parameters) {}

RefreshCommand::~RefreshCommand() {}

bool RefreshCommand::DoesPost() const {
    return true;
}

void RefreshCommand::ExecutePost(Response* const response) {
    Error* error = NULL;

    session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::Reload,
            base::Unretained(executor_.get()),
            &error));

    if (error)
        response->SetError(error);
}

}  // namespace webdriver
