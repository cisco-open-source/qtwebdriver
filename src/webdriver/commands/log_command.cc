// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "commands/log_command.h"

#include "base/string_util.h"
#include "base/values.h"
#include "base/bind.h"
#include "commands/response.h"
#include "webdriver_error.h"
#include "webdriver_session.h"
#include "webdriver_view_executor.h"

namespace webdriver {

LogCommand::LogCommand(
    const std::vector<std::string>& path_segments,
    const DictionaryValue* parameters)
    : ViewCommand(path_segments, parameters) {
}

LogCommand::~LogCommand() {
}

bool LogCommand::DoesPost() const {
    return true;
}

void LogCommand::ExecutePost(Response* const response) {
    std::string type;
    if (!GetStringParameter("type", &type)) {
        response->SetError(new Error(kUnknownError, "Missing or invalid 'type'"));
        return;
    }

    LogType log_type;
    base::ListValue* browserLog = NULL;
    if (!LogType::FromString(type, &log_type)) {
        browserLog = new base::ListValue();
        response->SetValue(browserLog);
        return;
    }

    if (log_type.type() == LogType::kDriver) {
        response->SetValue(session_->GetLog());
    }
    else if (log_type.type() == LogType::kPerformance) {
        response->SetValue(session_->GetPerfLog());
    }
    else if (log_type.type() == LogType::kBrowser) {
        Error* error = NULL;

        session_->RunSessionTask(base::Bind(
                    &ViewCmdExecutor::GetBrowserLog,
                    base::Unretained(executor_.get()),
                    &browserLog,
                    &error));

        if (error)
            response->SetError(error);
        else if (browserLog == NULL)
        {
            browserLog = new base::ListValue();
            response->SetValue(browserLog);
        }
        else
        {
            response->SetValue(browserLog);
        }
    }
}

LogTypesCommand::LogTypesCommand(
    const std::vector<std::string>& path_segments,
    const DictionaryValue* parameters)
    : WebDriverCommand(path_segments, parameters) {
}

LogTypesCommand::~LogTypesCommand() {
}

bool LogTypesCommand::DoesGet() const {
    return true;
}

void LogTypesCommand::ExecuteGet(Response* const response) {
    base::ListValue* logTypes_list = new base::ListValue();
    logTypes_list->Append(Value::CreateStringValue(LogType(LogType::kDriver).ToString()));
    logTypes_list->Append(Value::CreateStringValue(LogType(LogType::kBrowser).ToString()));
    if (session_->GetMinPerfLogLevel() != kOffLogLevel) {
        logTypes_list->Append(Value::CreateStringValue(LogType(LogType::kPerformance).ToString()));
    }
    response->SetValue(logTypes_list);
}

}  // namespace webdriver
