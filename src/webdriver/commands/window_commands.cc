// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "commands/window_commands.h"

#include "base/values.h"
#include "base/bind.h"
#include "base/memory/scoped_ptr.h"
#include "commands/response.h"
#include "webdriver_error.h"
#include "webdriver_session.h"
#include "webdriver_util.h"
#include "webdriver_view_executor.h"

using base::Value;

namespace webdriver {

namespace {

typedef scoped_ptr<ViewCmdExecutor> ExecutorPtr;    

bool GetWindowId(const std::string& window_id_string,
                 const ViewId& current_id,
                 ViewId* window_id,
                 Response* const response) {
    if (window_id_string == "current") {
        *window_id = current_id;
    } else {
        *window_id = ViewId(window_id_string);
    }
    return true;
}

}

WindowSizeCommand::WindowSizeCommand(
    const std::vector<std::string>& path_segments,
    const base::DictionaryValue* parameters)
    : WebDriverCommand(path_segments, parameters) {
}

WindowSizeCommand::~WindowSizeCommand() {
}

bool WindowSizeCommand::DoesGet() const {
    return true;
}

bool WindowSizeCommand::DoesPost() const {
    return true;
}

void WindowSizeCommand::ExecuteGet(Response* const response) {
    // Path segment: "/session/$sessionId/window/$windowHandle/size"
    ViewId window_id;
    if (!GetWindowId(GetPathVariable(4), session_->current_view(), &window_id, response))
        return;

    ExecutorPtr executor(ViewCmdExecutorFactory::GetInstance()->CreateExecutor(session_, window_id));
    if (NULL == executor.get()) {
        response->SetError(new Error(kBadRequest, "cant get view executor."));
        return;
    }

    Rect bounds;
    Error* error = NULL;

    session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::GetBounds,
            base::Unretained(executor.get()),
            &bounds,
            &error));

    if (error) {
        response->SetError(error);
        return;
    }
    base::DictionaryValue* size = new base::DictionaryValue();
    size->SetInteger("width", bounds.width());
    size->SetInteger("height", bounds.height());
    response->SetValue(size);
}

void WindowSizeCommand::ExecutePost(Response* const response) {
    // Path segment: "/session/$sessionId/window/$windowHandle/size"
    ViewId window_id;
    if (!GetWindowId(GetPathVariable(4), session_->current_view(), &window_id, response))
        return;

    ExecutorPtr executor(ViewCmdExecutorFactory::GetInstance()->CreateExecutor(session_, window_id));
    if (NULL == executor.get()) {
        response->SetError(new Error(kBadRequest, "cant get view executor."));
        return;
    }

    int width, height;
    if (!GetIntegerParameter("width", &width) ||
        !GetIntegerParameter("height", &height)) {
        response->SetError(new Error(
            kBadRequest,
            "Missing or invalid 'width' or 'height' parameters"));
        return;
    }

    Rect bounds;
    Error* error = NULL;
    session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::GetBounds,
            base::Unretained(executor.get()),
            &bounds,
            &error));
    if (!error) {
        bounds = Rect(bounds.origin(), Size(width, height));

        session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::SetBounds,
            base::Unretained(executor.get()),
            bounds,
            &error));
    }
    if (error) {
        response->SetError(error);
        return;
    }
}

WindowPositionCommand::WindowPositionCommand(
    const std::vector<std::string>& path_segments,
    const base::DictionaryValue* parameters)
    : WebDriverCommand(path_segments, parameters) {
}

WindowPositionCommand::~WindowPositionCommand() {
}

bool WindowPositionCommand::DoesGet() const {
    return true;
}

bool WindowPositionCommand::DoesPost() const {
    return true;
}

void WindowPositionCommand::ExecuteGet(Response* const response) {
    // Path segment: "/session/$sessionId/window/$windowHandle/position"
    ViewId window_id;
    if (!GetWindowId(GetPathVariable(4), session_->current_view(), &window_id, response))
        return;

    ExecutorPtr executor(ViewCmdExecutorFactory::GetInstance()->CreateExecutor(session_, window_id));
    if (NULL == executor.get()) {
        response->SetError(new Error(kBadRequest, "cant get view executor."));
        return;
    }

    Rect bounds;
    Error* error = NULL;
    session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::GetBounds,
            base::Unretained(executor.get()),
            &bounds,
            &error));
    if (error) {
        response->SetError(error);
        return;
    }
    base::DictionaryValue* size = new base::DictionaryValue();
    size->SetInteger("x", bounds.x());
    size->SetInteger("y", bounds.y());
    response->SetValue(size);
}

void WindowPositionCommand::ExecutePost(Response* const response) {
    // Path segment: "/session/$sessionId/window/$windowHandle/position"
    ViewId window_id;
    if (!GetWindowId(GetPathVariable(4), session_->current_view(), &window_id, response))
        return;

    ExecutorPtr executor(ViewCmdExecutorFactory::GetInstance()->CreateExecutor(session_, window_id));
    if (NULL == executor.get()) {
        response->SetError(new Error(kBadRequest, "cant get view executor."));
        return;
    }

    int x, y;
    if (!GetIntegerParameter("x", &x) ||
        !GetIntegerParameter("y", &y)) {
        response->SetError(new Error(
            kBadRequest,
            "Missing or invalid 'x' or 'y' parameters"));
        return;
    }
    Rect bounds;
    Error* error = NULL;
    session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::GetBounds,
            base::Unretained(executor.get()),
            &bounds,
            &error));
    if (!error) {
        bounds = Rect(Point(x, y), bounds.size());
        session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::SetBounds,
            base::Unretained(executor.get()),
            bounds,
            &error));
    }
    if (error) {
        response->SetError(error);
        return;
    }
}

WindowMaximizeCommand::WindowMaximizeCommand(
    const std::vector<std::string>& path_segments,
    const base::DictionaryValue* parameters)
    : WebDriverCommand(path_segments, parameters) {
}

WindowMaximizeCommand::~WindowMaximizeCommand() {
}

bool WindowMaximizeCommand::DoesPost() const {
    return true;
}

void WindowMaximizeCommand::ExecutePost(Response* const response) {
    // Path segment: "/session/$sessionId/window/$windowHandle/maximize"
    ViewId window_id;
    if (!GetWindowId(GetPathVariable(4), session_->current_view(), &window_id, response))
        return;

    ExecutorPtr executor(ViewCmdExecutorFactory::GetInstance()->CreateExecutor(session_, window_id));
    if (NULL == executor.get()) {
        response->SetError(new Error(kBadRequest, "cant get view executor."));
        return;
    }

    Error* error = NULL;
    session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::Maximize,
            base::Unretained(executor.get()),
            &error));
    if (error) {
        response->SetError(error);
        return;
    }
}

}  // namespace webdriver
