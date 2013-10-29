// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "commands/screenshot_command.h"

#include <string>
#include <vector>

#include "base/base64.h"
#include "base/values.h"
#include "base/bind.h"
#include "commands/response.h"
#include "webdriver_error.h"
#include "webdriver_session.h"
#include "webdriver_view_executor.h"

namespace webdriver {

ScreenshotCommand::ScreenshotCommand(const std::vector<std::string>& ps,
                                     const DictionaryValue* const parameters)
    : ViewCommand(ps, parameters) {}

ScreenshotCommand::~ScreenshotCommand() {}

bool ScreenshotCommand::DoesGet() const {
    return true;
}

void ScreenshotCommand::ExecuteGet(Response* const response) {
    std::string raw_bytes;

    Error* error = NULL;

    session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::GetScreenShot,
            base::Unretained(executor_.get()),
            &raw_bytes,
            &error));
    
    if (error) {
        response->SetError(error);
        return;
    }

    // Convert the raw binary data to base 64 encoding for webdriver.
    std::string base64_screenshot;
    if (!base::Base64Encode(raw_bytes, &base64_screenshot)) {
        response->SetError(new Error(
            kUnknownError, "Encoding the PNG to base64 format failed"));
        return;
    }

    response->SetValue(new StringValue(base64_screenshot));
}

}  // namespace webdriver
