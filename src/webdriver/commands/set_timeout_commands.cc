// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "commands/set_timeout_commands.h"

#include <string>

#include "base/stringprintf.h"
#include "base/values.h"
#include "commands/response.h"
#include "webdriver_error.h"
#include "webdriver_session.h"

namespace webdriver {

const int SetTimeoutCommand::DEFAULT_TIMEOUT = 60000;

SetTimeoutCommand::SetTimeoutCommand(
        const std::vector<std::string>& path_segments,
        const DictionaryValue* const parameters) :
    WebDriverCommand(path_segments, parameters) {}

SetTimeoutCommand::~SetTimeoutCommand() {}

bool SetTimeoutCommand::DoesPost() const {
    return true;
}

void SetTimeoutCommand::ExecutePost(Response* const response) {
    // Timeout value in milliseconds
    const char kTimeoutMsKey[] = "ms";

    if (!HasParameter(kTimeoutMsKey)) {
        response->SetError(new Error(kBadRequest, "Request missing ms parameter"));
        return;
    }

    int ms_to_wait;
    if (!GetIntegerParameter(kTimeoutMsKey, &ms_to_wait)) {
        // Client may have sent us a floating point number. Since DictionaryValue
        // will not do a down cast for us, we must explicitly check for it here.
        // Note webdriver only supports whole milliseconds for a timeout value, so
        // we are safe to downcast.
        double ms;
        if (!GetDoubleParameter(kTimeoutMsKey, &ms)) {
            response->SetError(new Error(
                kBadRequest, "ms parameter is not a number"));
            return;
        }
        ms_to_wait = static_cast<int>(ms);
    }

    Error* error = NULL;
    scoped_ptr<Error> err(error);

    // Timeout type as string
    const char kTimeoutTypeKey[] = "type";
    std::string type_of_operation;
    if (!HasParameter(kTimeoutTypeKey)) {
        error = SetTimeout(ms_to_wait);
        if (error)
            response->SetError(error);
        return;
    }
    if (!GetStringParameter(kTimeoutTypeKey, &type_of_operation)) {
        response->SetError(new Error(
            kBadRequest, "'type' parameter must be a string"));
        return;
    }
    error = SetTimeout(ms_to_wait, type_of_operation);
    if (error)
        response->SetError(error);
}

Error* SetTimeoutCommand::SetTimeout(int timeout_ms) {
    return new Error(kUnknownError, "Request missing 'type' parameter");
}

Error* SetTimeoutCommand::SetTimeout(int timeout_ms, std::string type) {
    if (!type.compare("implicit")) {
        // timeouts value has a lower bound of 0 for implicit wait.
        if (timeout_ms < 0)
            return new Error(kUnknownError, "Timeout for implicit wait must be non-negative");
        session_->set_implicit_wait(timeout_ms);
        return NULL;
    } else {
        // Validate the wait time before setting it to the session.
        if (timeout_ms < 0)
            timeout_ms = DEFAULT_TIMEOUT;
        if (!type.compare("page load")) {
            session_->set_page_load_timeout(timeout_ms);
            return NULL;
        } else if (!type.compare("script")) {
            session_->set_async_script_timeout(timeout_ms);
            return NULL;
        } else {
            return new Error(kUnknownError, "Unknown type of timeout:" + type);
        }
    }
}

SetAsyncScriptTimeoutCommand::SetAsyncScriptTimeoutCommand(
        const std::vector<std::string>& path_segments,
        const DictionaryValue* const parameters)
    : SetTimeoutCommand(path_segments, parameters) {}

SetAsyncScriptTimeoutCommand::~SetAsyncScriptTimeoutCommand() {}

Error* SetAsyncScriptTimeoutCommand::SetTimeout(int timeout_ms) {
    if (timeout_ms < 0)
        timeout_ms = DEFAULT_TIMEOUT;
    session_->set_async_script_timeout(timeout_ms);
    return NULL;
}

ImplicitWaitCommand::ImplicitWaitCommand(
        const std::vector<std::string>& path_segments,
        const DictionaryValue* const parameters)
    : SetTimeoutCommand(path_segments, parameters) {}

ImplicitWaitCommand::~ImplicitWaitCommand() {}

Error* ImplicitWaitCommand::SetTimeout(int timeout_ms) {
    // timeouts value has a lower bound of 0 for implicit wait.
    if (timeout_ms < 0)
        return new Error(kUnknownError, "Timeout for implicit wait must be non-negative");
    session_->set_implicit_wait(timeout_ms);
    return NULL;
}

}  // namespace webdriver
