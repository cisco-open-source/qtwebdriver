// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "commands/alert_commands.h"

#include "base/values.h"
#include "base/bind.h"
#include "commands/response.h"
#include "webdriver_error.h"
#include "webdriver_session.h"
#include "webdriver_view_executor.h"

namespace webdriver {

AlertTextCommand::AlertTextCommand(
    const std::vector<std::string>& path_segments,
    const DictionaryValue* parameters)
    : ViewCommand(path_segments, parameters) {
}

AlertTextCommand::~AlertTextCommand() {
}

bool AlertTextCommand::DoesGet() const {
    return true;
}

bool AlertTextCommand::DoesPost() const {
    return true;
}

void AlertTextCommand::ExecuteGet(Response* const response) {
    std::string text;
    Error* error = NULL;

    session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::GetAlertMessage,
            base::Unretained(executor_.get()),
            &text,
            &error));

    if (error) {
        response->SetError(error);
        return;
    }

    response->SetValue(Value::CreateStringValue(text));
}

void AlertTextCommand::ExecutePost(Response* const response) {
    std::string text;
    if (!GetStringParameter("text", &text)) {
        response->SetError(new Error(
            kBadRequest, "'text' is missing or invalid"));
        return;
    }

    Error* error = NULL;

    session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::SetAlertPromptText,
            base::Unretained(executor_.get()),
            text,
            &error));
    
    if (error)
        response->SetError(error);
}

AcceptAlertCommand::AcceptAlertCommand(
    const std::vector<std::string>& path_segments,
    const DictionaryValue* parameters)
    : ViewCommand(path_segments, parameters) {
}

AcceptAlertCommand::~AcceptAlertCommand() {
}

bool AcceptAlertCommand::DoesPost() const {
    return true;
}

void AcceptAlertCommand::ExecutePost(Response* const response) {
    Error* error = NULL;

    session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::AcceptOrDismissAlert,
            base::Unretained(executor_.get()),
            true,
            &error));

    if (error)
        response->SetError(error);
}

DismissAlertCommand::DismissAlertCommand(
    const std::vector<std::string>& path_segments,
    const DictionaryValue* parameters)
    : ViewCommand(path_segments, parameters) {
}

DismissAlertCommand::~DismissAlertCommand() {
}

bool DismissAlertCommand::DoesPost() const {
    return true;
}

void DismissAlertCommand::ExecutePost(Response* const response) {
    Error* error = NULL;

    session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::AcceptOrDismissAlert,
            base::Unretained(executor_.get()),
            false,
            &error));
  
    if (error)
        response->SetError(error);
}

}  // namespace webdriver
