// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "commands/execute_async_script_command.h"

#include "base/values.h"
#include "base/bind.h"
#include "commands/response.h"
#include "webdriver_error.h"
#include "webdriver_session.h"
#include "webdriver_view_executor.h"

namespace webdriver {

ExecuteAsyncScriptCommand::ExecuteAsyncScriptCommand(
    const std::vector<std::string>& path_segments,
    const DictionaryValue* const parameters)
    : ViewCommand(path_segments, parameters) {}

ExecuteAsyncScriptCommand::~ExecuteAsyncScriptCommand() {}

bool ExecuteAsyncScriptCommand::DoesPost() const {
    return true;
}

void ExecuteAsyncScriptCommand::ExecutePost(Response* const response) {
    std::string script;
    if (!GetStringParameter("script", &script)) {
        response->SetError(new Error(
            kBadRequest, "No script to execute specified"));
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
            &ViewCmdExecutor::ExecuteAsyncScript,
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
