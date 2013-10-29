// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "commands/target_locator_commands.h"

#include "base/string_number_conversions.h"
#include "base/values.h"
#include "base/bind.h"
#include "commands/response.h"
#include "webdriver_element_id.h"
#include "webdriver_error.h"
#include "webdriver_session.h"
#include "webdriver_util.h"
#include "webdriver_view_executor.h"
#include "webdriver_view_enumerator.h"

using base::Value;

namespace webdriver {

typedef scoped_ptr<ViewCmdExecutor> ExecutorPtr;        

WindowHandleCommand::WindowHandleCommand(
    const std::vector<std::string>& path_segments,
    const DictionaryValue* parameters)
    : WebDriverCommand(path_segments, parameters) {}

WindowHandleCommand::~WindowHandleCommand() {}

bool WindowHandleCommand::DoesGet() const {
    return true;
}

void WindowHandleCommand::ExecuteGet(Response* const response) {
    response->SetValue(Value::CreateStringValue(session_->current_view().id()));
}

WindowHandlesCommand::WindowHandlesCommand(
    const std::vector<std::string>& path_segments,
    const DictionaryValue* parameters)
    : WebDriverCommand(path_segments, parameters) {}

WindowHandlesCommand::~WindowHandlesCommand() {}

bool WindowHandlesCommand::DoesGet() const {
    return true;
}

void WindowHandlesCommand::ExecuteGet(Response* const response) {
    std::vector<ViewId> views;

    session_->RunSessionTask(base::Bind(
            &ViewEnumerator::EnumerateViews,
            session_,
            &views));

    base::ListValue* id_list = new base::ListValue();
    for (size_t i = 0; i < views.size(); ++i) {
        id_list->Append(Value::CreateStringValue(views[i].id()));
    }
    response->SetValue(id_list);
}

WindowCommand::WindowCommand(
    const std::vector<std::string>& path_segments,
    const DictionaryValue* parameters)
    : WebDriverCommand(path_segments, parameters) {}

WindowCommand::~WindowCommand() {}

bool WindowCommand::DoesPost() const {
    return true;
}

bool WindowCommand::DoesDelete() const {
    return true;
}

void WindowCommand::ExecutePost(Response* const response) {
    std::string name;
    ViewId new_view;
    Error* error = NULL;

    if (!GetStringParameter("name", &name)) {
        response->SetError(new Error(
            kBadRequest, "Missing or invalid 'name' parameter"));
        return;
    }

    if (DoesViewExist(ViewId(name))) {
        new_view = ViewId(name);
    }

    if (!new_view.is_valid()) {
        // enumerate all views
        std::vector<ViewId> views;
        session_->RunSessionTask(base::Bind(
                &ViewEnumerator::EnumerateViews,
                session_,
                &views));
        for (size_t i = 0; i < views.size(); ++i) {
            std::string window_name;

            error = GetViewTitle(views[i], &window_name);
            if (error)
                break;

            if (name == window_name) {
                new_view = views[i];
                break;
            }
        }
    }

    if (new_view.is_valid()) {
        error = SwitchToView(new_view);
    } else {
        error = new Error(kNoSuchWindow);
    }

    if (error)
        response->SetError(error);
}

Error* WindowCommand::GetViewTitle(const ViewId& viewId, std::string* title) {
    Error* error = NULL;
    ExecutorPtr executor(ViewCmdExecutorFactory::GetInstance()->CreateExecutor(session_, viewId));

    if (NULL == executor.get()) {
        return new Error(kBadRequest, "cant get view executor.");
    }

    session_->RunSessionTask(base::Bind(
                &ViewCmdExecutor::GetWindowName,
                base::Unretained(executor.get()),
                title,
                &error));

    return error;
}

Error* WindowCommand::SwitchToView(const ViewId& viewId) {
    Error* error = NULL;
    ExecutorPtr executor(ViewCmdExecutorFactory::GetInstance()->CreateExecutor(session_, viewId));

    if (NULL == executor.get()) {
        return new Error(kBadRequest, "cant get view executor.");
    }

    session_->RunSessionTask(base::Bind(
                &ViewCmdExecutor::SwitchTo,
                base::Unretained(executor.get()),
                &error));

    return error;
}

bool WindowCommand::DoesViewExist(const ViewId& viewId) {
    ExecutorPtr executor(ViewCmdExecutorFactory::GetInstance()->CreateExecutor(session_, viewId));

    if (NULL == executor.get()) {
        return false;
    }
    
    return true;
}

void WindowCommand::ExecuteDelete(Response* const response) {
    Error* error = NULL;
    ExecutorPtr executor(ViewCmdExecutorFactory::GetInstance()->CreateExecutor(session_, session_->current_view()));

    if (NULL == executor.get()) {
        response->SetError(new Error(kBadRequest, "cant get view executor."));
        return;
    }

    session_->RunSessionTask(base::Bind(
                &ViewCmdExecutor::Close,
                base::Unretained(executor.get()),
                &error));

    if (error) {
        response->SetError(error);
        return;
    }

    // terminate session if no views found
    std::vector<ViewId> views;

    session_->RunSessionTask(base::Bind(
            &ViewEnumerator::EnumerateViews,
            session_,
            &views));

    if (views.empty()) {
        // Session manages its own lifetime, so do not call delete.
        session_->Terminate();
    }
}

bool WindowCommand::ShouldRunPreAndPostCommandHandlers() {
  return false;
}

SwitchFrameCommand::SwitchFrameCommand(
    const std::vector<std::string>& path_segments,
    const DictionaryValue* parameters)
    : ViewCommand(path_segments, parameters) {}

SwitchFrameCommand::~SwitchFrameCommand() {}

bool SwitchFrameCommand::DoesPost() const {
  return true;
}

void SwitchFrameCommand::ExecutePost(Response* const response) {
    std::string id;
    int index = 0;
    ElementId element;
    Error* error = NULL;

    if (GetStringParameter("id", &id)) {
        session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::SwitchToFrameWithNameOrId,
            base::Unretained(executor_.get()),
            id,
            &error));
    } else if (GetIntegerParameter("id", &index)) {
        session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::SwitchToFrameWithIndex,
            base::Unretained(executor_.get()),
            index,
            &error));
    } else if (GetWebElementParameter("id", &element)) {
        session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::SwitchToFrameWithElement,
            base::Unretained(executor_.get()),
            element,
            &error));
    } else if (IsNullParameter("id") || !HasParameter("id")) {
        // Treat null 'id' and no 'id' as the same.
        // See http://code.google.com/p/selenium/issues/detail?id=1479.
        session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::SwitchToTopFrame,
            base::Unretained(executor_.get()),
            &error));
    } else {
        error = new Error(kBadRequest, "Invalid 'id' parameter");
    }
    if (error)
        response->SetError(error);
}

bool SwitchFrameCommand::GetWebElementParameter(const std::string& key,
                                                ElementId* out) const {
    const DictionaryValue* value;
    if (!GetDictionaryParameter(key, &value))
        return false;

    ElementId id(value);
    if (!id.is_valid())
        return false;

    *out = id;
    return true;
}

ActiveElementCommand::ActiveElementCommand(
    const std::vector<std::string>& path_segments,
    const DictionaryValue* parameters)
    : ViewCommand(path_segments, parameters) {}

ActiveElementCommand::~ActiveElementCommand() {}

bool ActiveElementCommand::DoesPost() const {
    return true;
}

void ActiveElementCommand::ExecutePost(Response* const response) {
    ElementId element;
    Error* error = NULL;

    session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::ActiveElement,
            base::Unretained(executor_.get()),
            &element,
            &error));

    if (error) {
        response->SetError(error);
        return;
    }
    response->SetValue(element.ToValue());
}

}  // namespace webdriver
