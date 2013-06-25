// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "commands/source_command.h"

#include <string>

#include "base/values.h"
#include "base/bind.h"
#include "commands/response.h"
#include "webdriver_error.h"
#include "webdriver_session.h"
#include "webdriver_view_executor.h"

namespace webdriver {

SourceCommand::SourceCommand(const std::vector<std::string>& path_segments,
                             const DictionaryValue* const parameters)
    : ViewCommand(path_segments, parameters) {}

SourceCommand::~SourceCommand() {}

bool SourceCommand::DoesGet() const {
    return true;
}

void SourceCommand::ExecuteGet(Response* const response) {
    std::string page_source;
    Error* error = NULL;

    session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::GetSource,
            base::Unretained(executor_.get()),
            &page_source,
            &error));
    
    if (error) {
        response->SetError(error);
        return;
    }

    response->SetValue(new StringValue(page_source));
}

}  // namespace webdriver
