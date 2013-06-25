// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "commands/execute_command.h"

#include <string>

#include "base/values.h"
#include "base/bind.h"
#include "commands/response.h"
#include "webdriver_error.h"
#include "webdriver_session.h"
#include "webdriver_view_executor.h"

namespace webdriver {

ExecuteCommand::ExecuteCommand(const std::vector<std::string>& path_segments,
                               const DictionaryValue* const parameters)
    : ViewCommand(path_segments, parameters) {}

ExecuteCommand::~ExecuteCommand() {}

bool ExecuteCommand::DoesPost() const {
    return true;
}

void ExecuteCommand::ExecutePost(Response* const response) {
    std::string script;
    if (!GetStringParameter("script", &script)) {
        response->SetError(new Error(kBadRequest, "No script specified"));
        return;
    }

    const ListValue* args;
    if (!GetListParameter("args", &args)) {
        response->SetError(new Error(
            kBadRequest, "No script arguments specified"));
        return;
    }

    Value* result = NULL;
    Error* error = NULL;

    session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::ExecuteScript,
            base::Unretained(executor_.get()),
            script,
            args,
            &result,
            &error));

    if (error) {
        error->AddDetails("Script execution failed. Script: " + script);
        response->SetError(error);
        return;
    }
    response->SetValue(result);
}

}  // namspace webdriver
