// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "commands/session_with_id.h"

#include <sstream>
#include <string>

#include "base/values.h"
#include "base/bind.h"
#include "commands/response.h"
#include "webdriver_session.h"
#include "webdriver_view_enumerator.h"
#include "webdriver_view_executor.h"

namespace webdriver {

SessionWithID::SessionWithID(const std::vector<std::string>& path_segments,
                             const DictionaryValue* const parameters)
    : WebDriverCommand(path_segments, parameters) {}

SessionWithID::~SessionWithID() {}

bool SessionWithID::DoesGet() const {
    return true;
}

bool SessionWithID::DoesDelete() const {
    return true;
}

void SessionWithID::ExecuteGet(Response* const response) {
    DictionaryValue *temp_value = session_->capabilities().caps->DeepCopy();

    response->SetValue(temp_value);
}

void SessionWithID::ExecuteDelete(Response* const response) {
    // close all views
    std::vector<ViewId> views;

    session_->RunSessionTask(base::Bind(
        &ViewEnumerator::EnumerateViews,
        session_,
        &views));

    for (size_t i = 0; i < views.size(); ++i) {
        CloseView(views[i]);
    }
    // Session manages its own lifetime, so do not call delete.
    session_->Terminate();
}

void SessionWithID::CloseView(const ViewId& viewId) {
    typedef scoped_ptr<ViewCmdExecutor> ExecutorPtr;        

    Error* error = NULL;
    ExecutorPtr executor(ViewCmdExecutorFactory::GetInstance()->CreateExecutor(session_, viewId));

    if (NULL == executor.get()) {
        session_->logger().Log(kSevereLogLevel, "Cant get executor.");
        return;
    }

    session_->RunSessionTask(base::Bind(
                &ViewCmdExecutor::Close,
                base::Unretained(executor.get()),
                &error));

    if (error) {
        session_->logger().Log(kSevereLogLevel, "Cant close view: "+error->details());
        delete error;
    }
}

bool SessionWithID::ShouldRunPreAndPostCommandHandlers() {
    return false;
}

}  // namespace webdriver
