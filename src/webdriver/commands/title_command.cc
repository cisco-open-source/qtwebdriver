// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "commands/title_command.h"

#include <string>

#include "commands/response.h"
#include "webdriver_error.h"
#include "webdriver_session.h"

namespace webdriver {

TitleCommand::TitleCommand(const std::vector<std::string>& path_segments,
                           const DictionaryValue* const parameters)
    : WebDriverCommand(path_segments, parameters) {}

TitleCommand::~TitleCommand() {}

bool TitleCommand::DoesGet() {
    return true;
}

void TitleCommand::ExecuteGet(Response* const response) {
    std::string title;
    Error* error = session_->GetTitle(&title);
    if (error) {
        response->SetError(error);
        return;
    }
    response->SetValue(new StringValue(title));
}

}  // namespace webdriver
