// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "commands/mouse_commands.h"

#include "base/values.h"
#include "base/bind.h"
//#include "value_conversion_util.h"
#include "commands/response.h"
#include "webdriver_basic_types.h"
#include "webdriver_element_id.h"
#include "webdriver_error.h"
#include "webdriver_session.h"
#include "webdriver_util.h"
#include "webdriver_view_executor.h"

namespace webdriver {

MoveAndClickCommand::MoveAndClickCommand(
    const std::vector<std::string>& path_segments,
    const DictionaryValue* const parameters)
    : ElementCommand(path_segments, parameters) {}

MoveAndClickCommand::~MoveAndClickCommand() {}

bool MoveAndClickCommand::DoesPost() const {
  return true;
}

void MoveAndClickCommand::ExecutePost(Response* response) {
    Error* error = NULL;

    session_->RunSessionTask(base::Bind(
                &ViewCmdExecutor::ClickElement,
                base::Unretained(executor_.get()),
                element,
                &error));

    if (error) {
        response->SetError(error);
    }

    return;
}

HoverCommand::HoverCommand(const std::vector<std::string>& path_segments,
                           const DictionaryValue* const parameters)
    : ElementCommand(path_segments, parameters) {}

HoverCommand::~HoverCommand() {}

bool HoverCommand::DoesPost() const {
    return true;
}

void HoverCommand::ExecutePost(Response* response) {
    Error* error = NULL;

    typedef void (ViewCmdExecutor::*MouseMove)(const ElementId&, Error**);
    MouseMove mouseMove = static_cast<MouseMove>(&ViewCmdExecutor::MouseMove);

    session_->RunSessionTask(base::Bind(
                mouseMove,
                base::Unretained(executor_.get()),
                element,
                &error));

    if (error) {
        response->SetError(error);
        return;
    }
}

AdvancedMouseCommand::AdvancedMouseCommand(
    const std::vector<std::string>& path_segments,
    const DictionaryValue* const parameters)
    : ViewCommand(path_segments, parameters) {}

AdvancedMouseCommand::~AdvancedMouseCommand() {}

bool AdvancedMouseCommand::DoesPost() const {
    return true;
}

MoveToCommand::MoveToCommand(
    const std::vector<std::string>& path_segments,
    const DictionaryValue* const parameters)
    : AdvancedMouseCommand(path_segments, parameters), has_element_(false),
      has_offset_(false), x_offset_(0), y_offset_(0) {}

MoveToCommand::~MoveToCommand() {}

bool MoveToCommand::Init(Response* const response) {
    if (!AdvancedMouseCommand::Init(response))
        return false;

    std::string element_name;
    has_element_ = GetStringParameter("element", &element_name);

    if (has_element_) {
        element_ = ElementId(element_name);
    }

    has_offset_ = GetIntegerParameter("xoffset", &x_offset_) &&
        GetIntegerParameter("yoffset", &y_offset_);

    if (!has_element_ && !has_offset_) {
        response->SetError(new Error(
            kBadRequest, "Invalid command arguments"));
        return false;
    }

    return true;
}

void MoveToCommand::ExecutePost(Response* const response) {
    Error* error = NULL;

    if (has_element_) {
        if (has_offset_) {
            typedef void (ViewCmdExecutor::*MouseMove)(const ElementId&, int, const int, Error**);
            MouseMove mouseMove = static_cast<MouseMove>(&ViewCmdExecutor::MouseMove);

            session_->RunSessionTask(base::Bind(
                mouseMove,
                base::Unretained(executor_.get()),
                element_,
                x_offset_,
                y_offset_,
                &error));
        } else {
            typedef void (ViewCmdExecutor::*MouseMove)(const ElementId&, Error**);
            MouseMove mouseMove = static_cast<MouseMove>(&ViewCmdExecutor::MouseMove);

            session_->RunSessionTask(base::Bind(
                mouseMove,
                base::Unretained(executor_.get()),
                element_,
                &error));
        }
    } else {
        if (has_offset_) {
            typedef void (ViewCmdExecutor::*MouseMove)(int, const int, Error**);
            MouseMove mouseMove = static_cast<MouseMove>(&ViewCmdExecutor::MouseMove);

            session_->RunSessionTask(base::Bind(
                mouseMove,
                base::Unretained(executor_.get()),
                x_offset_,
                y_offset_,
                &error));
        } else {
            error = new Error(kBadRequest, "Invalid command arguments");
        }
    }
        
    if (error) {
        response->SetError(error);
        return;
    }
}

ClickCommand::ClickCommand(const std::vector<std::string>& path_segments,
                           const DictionaryValue* const parameters)
    : AdvancedMouseCommand(path_segments, parameters) {}

ClickCommand::~ClickCommand() {}

bool ClickCommand::Init(Response* const response) {
    if (!AdvancedMouseCommand::Init(response))
        return false;

    if (!GetIntegerParameter("button", &button_)) {
        response->SetError(new Error(kBadRequest, "Missing mouse button argument"));
        return false;
    }

    return true;
}

void ClickCommand::ExecutePost(Response* const response) {
    Error* error = NULL;

    session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::MouseClick,
            base::Unretained(executor_.get()),
            static_cast<MouseButton>(button_),
            &error));

    if (error) {
        response->SetError(error);
        return;
    }
}

ButtonDownCommand::ButtonDownCommand(
    const std::vector<std::string>& path_segments,
    const DictionaryValue* const parameters)
    : AdvancedMouseCommand(path_segments, parameters) {}

ButtonDownCommand::~ButtonDownCommand() {}

void ButtonDownCommand::ExecutePost(Response* const response) {
    Error* error = NULL;

    session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::MouseButtonDown,
            base::Unretained(executor_.get()),
            &error));

    if (error) {
        response->SetError(error);
        return;
    }
}

ButtonUpCommand::ButtonUpCommand(const std::vector<std::string>& path_segments,
                                 const DictionaryValue* const parameters)
    : AdvancedMouseCommand(path_segments, parameters) {}

ButtonUpCommand::~ButtonUpCommand() {}

void ButtonUpCommand::ExecutePost(Response* const response) {
    Error* error = NULL;

    session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::MouseButtonUp,
            base::Unretained(executor_.get()),
            &error));

    if (error) {
        response->SetError(error);
        return;
    }
}

DoubleClickCommand::DoubleClickCommand(
    const std::vector<std::string>& path_segments,
    const DictionaryValue* const parameters)
    : AdvancedMouseCommand(path_segments, parameters) {}

DoubleClickCommand::~DoubleClickCommand() {}

void DoubleClickCommand::ExecutePost(Response* const response) {
    Error* error = NULL;

    session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::MouseDoubleClick,
            base::Unretained(executor_.get()),
            &error));

    if (error) {
        response->SetError(error);
        return;
    }
}

WheelCommand::WheelCommand(
    const std::vector<std::string> &path_segments,
    const base::DictionaryValue * const parameters)
    : AdvancedMouseCommand(path_segments, parameters) {}

WheelCommand::~WheelCommand() {}

bool WheelCommand::Init(Response* const response) {
    if (!AdvancedMouseCommand::Init(response))
        return false;

    if (!GetIntegerParameter("ticks", &ticks_)) {
        response->SetError(new Error(kBadRequest, "Missing ticks argument"));
        return false;
    }

    return true;
}

void WheelCommand::ExecutePost(Response * const response) {
    Error* error = NULL;

    // Most mouse types work in steps of 15 degrees,
    // in which case the delta value is a multiple of 15 * 8 = 120;
    const int STEP_WHEEL = 120;
    int delta = ticks_ * STEP_WHEEL;

    session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::MouseWheel,
            base::Unretained(executor_.get()),
            delta,
            &error));

    if (error) {
        response->SetError(error);
        return;
    }
}

}  // namespace webdriver
