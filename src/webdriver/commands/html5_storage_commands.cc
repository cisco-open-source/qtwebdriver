// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "commands/html5_storage_commands.h"

#include "base/values.h"
#include "base/bind.h"
#include "commands/response.h"
#include "webdriver_error.h"
#include "webdriver_session.h"
#include "webdriver_view_executor.h"

namespace webdriver {

LocalStorageCommand::LocalStorageCommand(
    const std::vector<std::string>& path_segments,
    const base::DictionaryValue* const parameters)
    : ViewCommand(path_segments, parameters) {}

LocalStorageCommand::~LocalStorageCommand() {}

bool LocalStorageCommand::DoesGet() const {
    return true;
}

bool LocalStorageCommand::DoesPost() const {
    return true;
}

bool LocalStorageCommand::DoesDelete() const {
    return true;
}

void LocalStorageCommand::ExecuteGet(Response* const response) {
    base::ListValue* keys;
    Error* error = NULL;

    session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::GetStorageKeys,
            base::Unretained(executor_.get()),
            kLocalStorageType,
            &keys,
            &error));

    if (error) {
        response->SetError(error);
        return;
    }
    response->SetValue(keys);
}

void LocalStorageCommand::ExecutePost(Response* const response) {
    // "/session/$sessionId/local_storage"
    std::string key;
    std::string value;
    if (!GetStringParameter("key", &key) ||
        !GetStringParameter("value", &value)) {
        response->SetError(new Error(
            kBadRequest, "('key', 'value') parameter is missing or invalid"));
        return;
    }

    Error* error = NULL;

    session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::SetStorageItem,
            base::Unretained(executor_.get()),
            kLocalStorageType,
            key,
            value,
            &error));

    if (error)
        response->SetError(error);
}

void LocalStorageCommand::ExecuteDelete(Response* const response) {
    Error* error = NULL;

    session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::ClearStorage,
            base::Unretained(executor_.get()),
            kLocalStorageType,
            &error));
    if (error)
        response->SetError(error);
}

LocalStorageKeyCommand::LocalStorageKeyCommand(
    const std::vector<std::string>& path_segments,
    const base::DictionaryValue* const parameters)
    : ViewCommand(path_segments, parameters) {}

LocalStorageKeyCommand::~LocalStorageKeyCommand() {}

bool LocalStorageKeyCommand::DoesGet() const {
    return true;
}

bool LocalStorageKeyCommand::DoesDelete() const {
    return true;
}

void LocalStorageKeyCommand::ExecuteGet(Response* const response) {
    // "/session/$sessionId/local_storage/key/$key"
    std::string key = GetPathVariable(5);
    std::string value;
    Error* error = NULL;

    session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::GetStorageItem,
            base::Unretained(executor_.get()),
            kLocalStorageType,
            key,
            &value,
            &error));
    if (error) {
        response->SetError(error);
        return;
    }
    response->SetValue(new base::StringValue(value));
}

void LocalStorageKeyCommand::ExecuteDelete(Response* const response) {
    // "/session/$sessionId/local_storage/key/$key"
    std::string key = GetPathVariable(5);
    std::string value;
    Error* error = NULL;

    session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::RemoveStorageItem,
            base::Unretained(executor_.get()),
            kLocalStorageType,
            key,
            &value,
            &error));

    if (error) {
        response->SetError(error);
        return;
    }
    response->SetValue(new base::StringValue(value));
}

LocalStorageSizeCommand::LocalStorageSizeCommand(
    const std::vector<std::string>& path_segments,
    const base::DictionaryValue* const parameters)
    : ViewCommand(path_segments, parameters) {}

LocalStorageSizeCommand::~LocalStorageSizeCommand() {}

bool LocalStorageSizeCommand::DoesGet() const {
    return true;
}

void LocalStorageSizeCommand::ExecuteGet(Response* const response) {
    int size;
    Error* error = NULL;

    session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::GetStorageSize,
            base::Unretained(executor_.get()),
            kLocalStorageType,
            &size,
            &error));
    if (error) {
        response->SetError(error);
        return;
    }
    response->SetValue(new base::FundamentalValue(size));
}

SessionStorageCommand::SessionStorageCommand(
    const std::vector<std::string>& path_segments,
    const base::DictionaryValue* parameters)
    : ViewCommand(path_segments, parameters) {}

SessionStorageCommand::~SessionStorageCommand() {}

bool SessionStorageCommand::DoesGet() const {
    return true;
}

bool SessionStorageCommand::DoesPost() const {
    return true;
}

bool SessionStorageCommand::DoesDelete() const {
    return true;
}

void SessionStorageCommand::ExecuteGet(Response* const response) {
    base::ListValue* keys;
    Error* error = NULL;

    session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::GetStorageKeys,
            base::Unretained(executor_.get()),
            kSessionStorageType,
            &keys,
            &error));
    if (error) {
        response->SetError(error);
        return;
    }
    response->SetValue(keys);
}

void SessionStorageCommand::ExecutePost(Response* const response) {
    // "/session/$sessionId/session_storage"
    std::string key;
    std::string value;
    if (!GetStringParameter("key", &key) ||
        !GetStringParameter("value", &value)) {
        response->SetError(new Error(
            kBadRequest, "('key', 'value') parameter is missing or invalid"));
        return;
    }

    Error* error = NULL;

    session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::SetStorageItem,
            base::Unretained(executor_.get()),
            kSessionStorageType,
            key,
            value,
            &error));

    if (error)
        response->SetError(error);
}

void SessionStorageCommand::ExecuteDelete(Response* const response) {
    Error* error = NULL;

    session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::ClearStorage,
            base::Unretained(executor_.get()),
            kSessionStorageType,
            &error));
    if (error)
        response->SetError(error);
}

SessionStorageKeyCommand::SessionStorageKeyCommand(
    const std::vector<std::string>& path_segments,
    const base::DictionaryValue* const parameters)
    : ViewCommand(path_segments, parameters) {}

SessionStorageKeyCommand::~SessionStorageKeyCommand() {}

bool SessionStorageKeyCommand::DoesGet() const {
    return true;
}

bool SessionStorageKeyCommand::DoesDelete() const {
    return true;
}

void SessionStorageKeyCommand::ExecuteGet(Response* const response) {
    // "/session/$sessionId/session_storage/key/$key"
    std::string key = GetPathVariable(5);
    std::string value;
    Error* error = NULL;

    session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::GetStorageItem,
            base::Unretained(executor_.get()),
            kSessionStorageType,
            key,
            &value,
            &error));
    if (error) {
        response->SetError(error);
        return;
    }
    response->SetValue(new base::StringValue(value));
}

void SessionStorageKeyCommand::ExecuteDelete(Response* const response) {
    // "/session/$sessionId/session_storage/key/$key"
    std::string key = GetPathVariable(5);
    std::string value;
    Error* error = NULL;

    session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::RemoveStorageItem,
            base::Unretained(executor_.get()),
            kSessionStorageType,
            key,
            &value,
            &error));

    if (error) {
        response->SetError(error);
        return;
    }
    response->SetValue(new base::StringValue(value));
}

SessionStorageSizeCommand::SessionStorageSizeCommand(
    const std::vector<std::string>& path_segments,
    const base::DictionaryValue* const parameters)
    : ViewCommand(path_segments, parameters) {}

SessionStorageSizeCommand::~SessionStorageSizeCommand() {}

bool SessionStorageSizeCommand::DoesGet() const {
    return true;
}

void SessionStorageSizeCommand::ExecuteGet(Response* const response) {
    int size;
    Error* error = NULL;

    session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::GetStorageSize,
            base::Unretained(executor_.get()),
            kSessionStorageType,
            &size,
            &error));
    if (error) {
        response->SetError(error);
        return;
    }
    response->SetValue(new base::FundamentalValue(size));
}

}  // namespace webdriver
