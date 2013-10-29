// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "commands/title_command.h"

#include <string>

#include "base/bind.h"

#include "commands/response.h"
#include "webdriver_error.h"
#include "webdriver_session.h"
#include "webdriver_view_executor.h"

namespace webdriver {

TitleCommand::TitleCommand(const std::vector<std::string>& path_segments,
                           const DictionaryValue* const parameters)
    : ViewCommand(path_segments, parameters) {}

TitleCommand::~TitleCommand() {}

bool TitleCommand::DoesGet() const {
    return true;
}

void TitleCommand::ExecuteGet(Response* const response) {
    std::string title;
    Error* error = NULL;

    session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::GetTitle,
            base::Unretained(executor_.get()),
            &title,
            &error));

    if (error) {
        response->SetError(error);
        return;
    }
    response->SetValue(new StringValue(title));
}

}  // namespace webdriver
