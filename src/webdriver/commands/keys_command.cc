// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "commands/keys_command.h"

#include "base/values.h"
#include "base/bind.h"
#include "commands/response.h"
#include "webdriver_error.h"
#include "webdriver_session.h"
#include "webdriver_view_executor.h"
#include "webdriver_util.h"

namespace webdriver {

KeysCommand::KeysCommand(const std::vector<std::string>& path_segments,
                         const DictionaryValue* const parameters)
    : ViewCommand(path_segments, parameters) {}

KeysCommand::~KeysCommand() {}

bool KeysCommand::DoesPost() const {
    return true;
}

void KeysCommand::ExecutePost(Response* const response) {
    const ListValue* key_list;
    if (!GetListParameter("value", &key_list)) {
        response->SetError(new Error(
            kBadRequest, "Missing or invalid 'value' parameter"));
        return;
    }

    // Flatten the given array of strings into one.
    string16 keys;
    Error* error = FlattenStringArray(key_list, &keys);
    if (error) {
        response->SetError(error);
        return;
    }

    typedef void (ViewCmdExecutor::*SendKeys)(const string16&, Error**);
    SendKeys sendKeys = static_cast<SendKeys>(&ViewCmdExecutor::SendKeys);
    session_->RunSessionTask(base::Bind(
            sendKeys,
            base::Unretained(executor_.get()),
            keys,
            &error));

    if (error) {
        response->SetError(error);
        return;
    }
}

}  // namespace webdriver
