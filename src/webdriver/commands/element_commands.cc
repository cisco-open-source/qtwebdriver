// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "commands/element_commands.h"

#include "base/base64.h"
#include "base/file_util.h"
#include "base/format_macros.h"
#include "base/memory/scoped_ptr.h"
#include "base/string_split.h"
#include "base/string_util.h"
#include "base/stringprintf.h"
#include "base/utf_string_conversions.h"
#include "base/values.h"
#include "base/bind.h"
#include "commands/response.h"
#include "webdriver_basic_types.h"
#include "webdriver_error.h"
#include "webdriver_session.h"
#include "webdriver_util.h"
#include "webdriver_view_executor.h"

namespace webdriver {

///////////////////// ElementCommand ////////////////////

ElementCommand::ElementCommand(
    const std::vector<std::string>& path_segments,
    const DictionaryValue* const parameters)
    : ViewCommand(path_segments, parameters),
      path_segments_(path_segments) {}

ElementCommand::~ElementCommand() {}

bool ElementCommand::Init(Response* const response) {
    if (!ViewCommand::Init(response))
        return false;

    // There should be at least 5 segments to match
    // "/session/$session/element/$id"
    if (path_segments_.size() < 5) {
        response->SetError(new Error(kBadRequest, "Path segments is less than 5"));
        return false;
    }

    // We cannot verify the ID is valid until we execute the command and
    // inject the ID into the in-page cache.
    element = ElementId(path_segments_.at(4));
    return true;
}

///////////////////// ElementAttributeCommand ////////////////////

ElementAttributeCommand::ElementAttributeCommand(
    const std::vector<std::string>& path_segments,
    const DictionaryValue* parameters)
    : ElementCommand(path_segments, parameters) {}

ElementAttributeCommand::~ElementAttributeCommand() {}

bool ElementAttributeCommand::DoesGet() const {
  return true;
}

void ElementAttributeCommand::ExecuteGet(Response* const response) {
    // There should be at least 7 segments to match
    // "/session/$session/element/$id/attribute/$name"
    if (path_segments_.size() < 7) {
        response->SetError(new Error(kBadRequest, "Path segments is less than 7"));
        return;
    }

    const std::string key = path_segments_.at(6);
    Value* value;

    Error* error = NULL;

    session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::GetAttribute,
            base::Unretained(executor_.get()),
            element,
            key,
            &value,
            &error));

    if (error) {
        response->SetError(error);
        return;
    }

    response->SetValue(value);
}

///////////////////// ElementClearCommand ////////////////////

ElementClearCommand::ElementClearCommand(
    const std::vector<std::string>& path_segments,
    const DictionaryValue* parameters)
    : ElementCommand(path_segments, parameters) {}

ElementClearCommand::~ElementClearCommand() {}

bool ElementClearCommand::DoesPost() const {
    return true;
}

void ElementClearCommand::ExecutePost(Response* const response) {
    Error* error = NULL;

    session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::ClearElement,
            base::Unretained(executor_.get()),
            element,
            &error));

    if (error) {
        response->SetError(error);
        return;
    }
}

///////////////////// ElementCssCommand ////////////////////

ElementCssCommand::ElementCssCommand(
    const std::vector<std::string>& path_segments,
    const DictionaryValue* parameters)
    : ElementCommand(path_segments, parameters) {}

ElementCssCommand::~ElementCssCommand() {}

bool ElementCssCommand::DoesGet() const {
    return true;
}

void ElementCssCommand::ExecuteGet(Response* const response) {
    // There should be at least 7 segments to match
    // "/session/$session/element/$id/css/$propertyName"
    if (path_segments_.size() < 7) {
        response->SetError(new Error(kBadRequest, "Path segments is less than 7"));
        return;
    }



    const std::string key = path_segments_.at(6);
    Value* value;

    Error* error = NULL;

    session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::GetElementCssProperty,
            base::Unretained(executor_.get()),
            element,
            key,
            &value,
            &error));

    if (error) {
        response->SetError(error);
        return;
    }

    response->SetValue(value);
}

///////////////////// ElementDisplayedCommand ////////////////////

ElementDisplayedCommand::ElementDisplayedCommand(
    const std::vector<std::string>& path_segments,
    const DictionaryValue* parameters)
    : ElementCommand(path_segments, parameters) {}

ElementDisplayedCommand::~ElementDisplayedCommand() {}

bool ElementDisplayedCommand::DoesGet() const {
    return true;
}

void ElementDisplayedCommand::ExecuteGet(Response* const response) {
    bool is_displayed;

    Error* error = NULL;

    session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::IsElementDisplayed,
            base::Unretained(executor_.get()),
            element,
            false,
            &is_displayed,
            &error));
    
    if (error) {
        response->SetError(error);
        return;
    }
    response->SetValue(Value::CreateBooleanValue(is_displayed));
}

///////////////////// ElementEnabledCommand ////////////////////

ElementEnabledCommand::ElementEnabledCommand(
    const std::vector<std::string>& path_segments,
    const DictionaryValue* parameters)
    : ElementCommand(path_segments, parameters) {}

ElementEnabledCommand::~ElementEnabledCommand() {}

bool ElementEnabledCommand::DoesGet() const {
    return true;
}

void ElementEnabledCommand::ExecuteGet(Response* const response) {
    bool is_enabled;

    Error* error = NULL;

    session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::IsElementEnabled,
            base::Unretained(executor_.get()),
            element,
            &is_enabled,
            &error));

    if (error) {
        response->SetError(error);
        return;
    }
    response->SetValue(Value::CreateBooleanValue(is_enabled));
}

///////////////////// ElementEqualsCommand ////////////////////

ElementEqualsCommand::ElementEqualsCommand(
    const std::vector<std::string>& path_segments,
    const DictionaryValue* parameters)
    : ElementCommand(path_segments, parameters) {}

ElementEqualsCommand::~ElementEqualsCommand() {}

bool ElementEqualsCommand::DoesGet() const {
    return true;
}

void ElementEqualsCommand::ExecuteGet(Response* const response) {
    // There should be at least 7 segments to match
    // "/session/$session/element/$id/equals/$other"
    if (path_segments_.size() < 7) {
        response->SetError(new Error(kBadRequest, "Path segments is less than 7"));
        return;
    }

    ElementId other_element(path_segments_.at(6));
    bool is_equals;

    Error* error = NULL;

    session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::ElementEquals,
            base::Unretained(executor_.get()),
            element,
            other_element,
            &is_equals,
            &error));

    if (error) {
        response->SetError(error);
        return;
    }

    response->SetValue(Value::CreateBooleanValue(is_equals));
}

///////////////////// ElementLocationCommand ////////////////////

ElementLocationCommand::ElementLocationCommand(
    const std::vector<std::string>& path_segments,
    const DictionaryValue* parameters)
    : ElementCommand(path_segments, parameters) {}

ElementLocationCommand::~ElementLocationCommand() {}

bool ElementLocationCommand::DoesGet() const {
    return true;
}

void ElementLocationCommand::ExecuteGet(Response* const response) {
    Point location;

    Error* error = NULL;

    session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::GetElementLocation,
            base::Unretained(executor_.get()),
            element,
            &location,
            &error));

    if (error) {
        response->SetError(error);
        return;
    }
    DictionaryValue* coord_dict = new DictionaryValue();
    coord_dict->SetInteger("x", location.x());
    coord_dict->SetInteger("y", location.y());
    response->SetValue(coord_dict);
}

///////////////////// ElementLocationInViewCommand ////////////////////

ElementLocationInViewCommand::ElementLocationInViewCommand(
    const std::vector<std::string>& path_segments,
    const DictionaryValue* parameters)
    : ElementCommand(path_segments, parameters) {}

ElementLocationInViewCommand::~ElementLocationInViewCommand() {}

bool ElementLocationInViewCommand::DoesGet() const {
    return true;
}

void ElementLocationInViewCommand::ExecuteGet(Response* const response) {
    Point location;

    Error* error = NULL;

    session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::GetElementLocationInView,
            base::Unretained(executor_.get()),
            element,
            &location,
            &error));

    if (error) {
        response->SetError(error);
        return;
    }
    DictionaryValue* coord_dict = new DictionaryValue();
    coord_dict->SetInteger("x", location.x());
    coord_dict->SetInteger("y", location.y());
    response->SetValue(coord_dict);
}

///////////////////// ElementNameCommand ////////////////////

ElementNameCommand::ElementNameCommand(
    const std::vector<std::string>& path_segments,
    const DictionaryValue* parameters)
    : ElementCommand(path_segments, parameters) {}

ElementNameCommand::~ElementNameCommand() {}

bool ElementNameCommand::DoesGet() const {
    return true;
}

void ElementNameCommand::ExecuteGet(Response* const response) {
    std::string tag_name;

    Error* error = NULL;

    session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::GetElementTagName,
            base::Unretained(executor_.get()),
            element,
            &tag_name,
            &error));

    if (error) {
        response->SetError(error);
        return;
    }
    response->SetValue(Value::CreateStringValue(tag_name));
}

///////////////////// ElementSelectedCommand ////////////////////

ElementSelectedCommand::ElementSelectedCommand(
    const std::vector<std::string>& path_segments,
    const DictionaryValue* parameters)
    : ElementCommand(path_segments, parameters) {}

ElementSelectedCommand::~ElementSelectedCommand() {}

bool ElementSelectedCommand::DoesGet() const {
    return true;
}

bool ElementSelectedCommand::DoesPost() const {
    return true;
}

void ElementSelectedCommand::ExecuteGet(Response* const response) {
    bool is_selected;

    Error* error = NULL;

    session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::IsOptionElementSelected,
            base::Unretained(executor_.get()),
            element,
            &is_selected,
            &error));

    if (error) {
        response->SetError(error);
        return;
    }
    response->SetValue(Value::CreateBooleanValue(is_selected));
}

void ElementSelectedCommand::ExecutePost(Response* const response) {
    Error* error = NULL;

    session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::SetOptionElementSelected,
            base::Unretained(executor_.get()),
            element,
            true,
            &error));

    if (error) {
        response->SetError(error);
        return;
    }
}

///////////////////// ElementSizeCommand ////////////////////

ElementSizeCommand::ElementSizeCommand(
    const std::vector<std::string>& path_segments,
    const DictionaryValue* parameters)
    : ElementCommand(path_segments, parameters) {}

ElementSizeCommand::~ElementSizeCommand() {}

bool ElementSizeCommand::DoesGet() const {
    return true;
}

void ElementSizeCommand::ExecuteGet(Response* const response) {
    Size size;

    Error* error = NULL;

    session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::GetElementSize,
            base::Unretained(executor_.get()),
            element,
            &size,
            &error));

    if (error) {
        response->SetError(error);
        return;
    }
    DictionaryValue* dict = new DictionaryValue();
    dict->SetInteger("width", size.width());
    dict->SetInteger("height", size.height());
    response->SetValue(dict);
}

///////////////////// ElementSubmitCommand ////////////////////

ElementSubmitCommand::ElementSubmitCommand(
    const std::vector<std::string>& path_segments,
    const DictionaryValue* parameters)
    : ElementCommand(path_segments, parameters) {}

ElementSubmitCommand::~ElementSubmitCommand() {}

bool ElementSubmitCommand::DoesPost() const {
    return true;
}

void ElementSubmitCommand::ExecutePost(Response* const response) {
    Error* error = NULL;

    session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::ElementSubmit,
            base::Unretained(executor_.get()),
            element,
            &error));

    if (error) {
        response->SetError(error);
        return;
    }
}

///////////////////// ElementValueCommand ////////////////////

ElementValueCommand::ElementValueCommand(
    const std::vector<std::string>& path_segments,
    const DictionaryValue* parameters)
    : ElementCommand(path_segments, parameters) {}

ElementValueCommand::~ElementValueCommand() {}

bool ElementValueCommand::DoesPost() const {
    return true;
}

void ElementValueCommand::ExecutePost(Response* const response) {
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

    typedef void (ViewCmdExecutor::*SendKeys)(const ElementId&, const string16&, Error**);
    SendKeys sendKeys = static_cast<SendKeys>(&ViewCmdExecutor::SendKeys);
    session_->RunSessionTask(base::Bind(
            sendKeys,
            base::Unretained(executor_.get()),
            element,
            keys,
            &error));

    if (error) {
        response->SetError(error);
        return;
    }
}

///////////////////// ElementTextCommand ////////////////////

ElementTextCommand::ElementTextCommand(
    const std::vector<std::string>& path_segments,
    const DictionaryValue* parameters)
    : ElementCommand(path_segments, parameters) {}

ElementTextCommand::~ElementTextCommand() {}

bool ElementTextCommand::DoesGet() const {
    return true;
}

void ElementTextCommand::ExecuteGet(Response* const response) {
    std::string element_text;

    Error* error = NULL;

    session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::GetElementText,
            base::Unretained(executor_.get()),
            element,
            &element_text,
            &error));

    if (error) {
        response->SetError(error);
        return;
    }
    response->SetValue(Value::CreateStringValue(element_text));
}

///////////////////// ElementScreenshotCommand ////////////////////

ElementScreenshotCommand::ElementScreenshotCommand(
    const std::vector<std::string>& path_segments,
    const DictionaryValue* parameters)
    : ElementCommand(path_segments, parameters) {}

ElementScreenshotCommand::~ElementScreenshotCommand() {}

bool ElementScreenshotCommand::DoesGet() const {
    return true;
}

void ElementScreenshotCommand::ExecuteGet(Response* const response) {
    std::string raw_bytes;

    Error* error = NULL;

    session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::GetElementScreenShot,
            base::Unretained(executor_.get()),
            element,
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
