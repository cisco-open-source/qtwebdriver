// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "commands/webdriver_command.h"

#include <string>

#include "base/memory/scoped_ptr.h"
#include "base/memory/singleton.h"
#include "base/string_util.h"
#include "base/stringprintf.h"
#include "base/values.h"
#include "base/bind.h"
#include "commands/response.h"
#include "webdriver_error.h"
#include "webdriver_logging.h"
#include "webdriver_session.h"
#include "webdriver_session_manager.h"
#include "webdriver_util.h"
#include "webdriver_view_executor.h"
#include "webdriver_view_enumerator.h"

namespace webdriver {

WebDriverCommand::WebDriverCommand(
    const std::vector<std::string>& path_segments,
    const DictionaryValue* const parameters)
    : Command(path_segments, parameters), session_(NULL) {
}

WebDriverCommand::~WebDriverCommand() {}

bool WebDriverCommand::Init(Response* const response) {
    // There should be at least 3 path segments to match "/session/$id".
    session_id_ = GetPathVariable(2);
    if (session_id_.length() == 0) {
        response->SetError(
            new Error(kBadRequest, "No session ID specified"));
        return false;
    }

    session_ = SessionManager::GetInstance()->GetSession(session_id_);
    if (session_ == NULL) {
        response->SetError(
            new Error(kSessionNotFound, "Session not found: " + session_id_));
        return false;
    }

    std::string message = base::StringPrintf(
        "Command received (%s)", JoinString(path_segments_, '/').c_str());
    if (parameters_)
        message += " with params " + JsonStringifyForDisplay(parameters_.GetRawPointer());
    session_->logger().Log(kFineLogLevel, message);

    // terminate session if no views found
    std::vector<ViewId> views;

    session_->RunSessionTask(base::Bind(
            &ViewEnumerator::EnumerateViews,
            session_,
            &views));

    if (views.empty()) {
        // Session manages its own lifetime, so do not call delete.
        session_->Terminate();

        response->SetError(
            new Error(kSessionNotFound, "Session not found: " + session_id_));
        return false;   
    }

    if (ShouldRunPreAndPostCommandHandlers()) {
        Error* error = session_->BeforeExecuteCommand();
        if (error) {
            response->SetError(error);
            return false;
        }
    }
    response->SetField("sessionId", Value::CreateStringValue(session_id_));
    return true;
}

void WebDriverCommand::Finish(Response* const response) {
    // The session may have been terminated as a result of the command.
    if (!SessionManager::GetInstance()->Has(session_id_))
        return;

    if (ShouldRunPreAndPostCommandHandlers()) {
        // TODO: review
        scoped_ptr<Error> error(session_->AfterExecuteCommand());
        if (error.get()) {
            session_->logger().Log(kWarningLogLevel,
                             "AfterExecuteCommand failed: " + error->details());
        }
    }

    LogLevel level = kWarningLogLevel;
    if (response->GetStatus() == kSuccess)
        level = kFineLogLevel;
    session_->logger().Log(
        level, base::StringPrintf(
            "Command finished (%s) with response %s",
            JoinString(path_segments_, '/').c_str(),
            JsonStringifyForDisplay(response->GetDictionary()).c_str()));
}

bool WebDriverCommand::ShouldRunPreAndPostCommandHandlers() {
    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////

ViewCommand::ViewCommand(
    const std::vector<std::string>& path_segments,
    const DictionaryValue* const parameters)
    : WebDriverCommand(path_segments, parameters),
      executor_(NULL) {}

ViewCommand::~ViewCommand() {}

bool ViewCommand::Init(Response* const response) {
    if (!WebDriverCommand::Init(response))
        return false;

    // get executor for current view
    executor_.reset(ViewCmdExecutorFactory::GetInstance()->CreateExecutor(session_));
    if (NULL == executor_.get()) {
        response->SetError(new Error(kNoSuchWindow, "cant get view executor."));
        return false;
    }

#if 1
    // TODO: review
    //if (ShouldRunPreAndPostCommandHandlers())
    {

        Error* error = NULL;
        scoped_ptr<Error> switch_error;
        session_->RunSessionTask(base::Bind(
                        &ViewCmdExecutor::SwitchToTopFrameIfCurrentFrameInvalid,
                        base::Unretained(executor_.get()),
                        &error));
        switch_error.reset(error);
        if (switch_error.get()) {
            std::string text;
            Error* terr = NULL;
            scoped_ptr<Error> alert_error;
            session_->RunSessionTask(base::Bind(
                        &ViewCmdExecutor::GetAlertMessage,
                        base::Unretained(executor_.get()),
                        &text,
                        &error));
            alert_error.reset(terr);
            if (alert_error.get()) {
                // Only return a frame checking error if a modal dialog is not present.
                // TODO(kkania): This is ugly. Fix.
                response->SetError(switch_error.release());
                return false;
            }
        }
    }
        
#endif


    return true;
}


}  // namespace webdriver
