// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "commands/browser_connection_command.h"

#include "base/bind.h"
#include "commands/response.h"
#include "webdriver_error.h"
#include "webdriver_session.h"
#include "webdriver_view_executor.h"
#include "webdriver_view_enumerator.h"

namespace webdriver {

typedef scoped_ptr<ViewCmdExecutor> ExecutorPtr;

BrowserConnectionCommand::BrowserConnectionCommand(const std::vector<std::string>& path_segments,
                                                   const base::DictionaryValue* const parameters)
    : WebDriverCommand(path_segments, parameters) {
}

BrowserConnectionCommand::~BrowserConnectionCommand() {
}

bool BrowserConnectionCommand::DoesPost()  const {
    return true;
}

bool BrowserConnectionCommand::DoesGet()  const {
    return true;
}

void BrowserConnectionCommand::ExecuteGet(Response* const response) {
    bool online;
    Error* error = NULL;

    ExecutorPtr executor(ViewCmdExecutorFactory::GetInstance()->CreateExecutor(session_));
    if (NULL == executor.get()) {
        response->SetError(new Error(kBadRequest, "cant get view executor."));
        return;
    }

    session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::IsOnline,
            base::Unretained(executor.get()),
            &online,
            &error));

    if (error) {
        response->SetError(error);
        return;
    }
    response->SetValue(base::Value::CreateBooleanValue(online));
}

void BrowserConnectionCommand::ExecutePost(Response* const response) {
    const char kStateKey[] = "state";

    if (!HasParameter(kStateKey)) {
        response->SetError(new Error(kBadRequest, "Request missing 'state' parameter"));
        return;
    }

    bool online;
    if (!GetBooleanParameter("state", &online)) {
        response->SetError(new Error(
            kBadRequest, "'state' is  invalid"));
        return;
    }

    // update online state for all views
    std::vector<ViewId> views;

    session_->RunSessionTask(base::Bind(
        &ViewEnumerator::EnumerateViews,
        session_,
        &views));

    for (size_t i = 0; i < views.size(); ++i) {
        Error* error = NULL;
        ExecutorPtr executor(ViewCmdExecutorFactory::GetInstance()->CreateExecutor(session_, views.at(i)));
        if (NULL == executor.get()) {
            GlobalLogger::Log(kWarningLogLevel, "Cant update online mode for view(no executor), skip.");
            continue;
        }

        session_->RunSessionTask(base::Bind(
                &ViewCmdExecutor::SetOnline,
                base::Unretained(executor.get()),
                online,
                &error));

        if (error) {
            response->SetError(error);
            return;
        }
    }
}


}
