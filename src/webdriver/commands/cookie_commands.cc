// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "commands/cookie_commands.h"

#include <string>
#include <vector>

#include "base/memory/scoped_ptr.h"
#include "base/string_split.h"
#include "base/string_util.h"
#include "base/values.h"
#include "base/bind.h"
#include "commands/response.h"
#include "webdriver_session.h"
#include "webdriver_view_executor.h"

namespace webdriver {

CookieCommand::CookieCommand(const std::vector<std::string>& path_segments,
                             const DictionaryValue* const parameters)
    : ViewCommand(path_segments, parameters) {}

CookieCommand::~CookieCommand() {}

bool CookieCommand::DoesDelete() const {
    return true;
}

bool CookieCommand::DoesGet() const {
    return true;
}

bool CookieCommand::DoesPost() const {
    return true;
}

void CookieCommand::ExecuteGet(Response* const response) {
    std::string url;
    ListValue* cookies;
    Error* error = NULL;

    session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::GetURL,
            base::Unretained(executor_.get()),
            &url,
            &error));

    if (!error) {
        session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::GetCookies,
            base::Unretained(executor_.get()),
            url,
            &cookies,
            &error));
    }

    if (error) {
        response->SetError(error);
        return;
    }
    response->SetValue(cookies);
}

void CookieCommand::ExecutePost(Response* const response) {
    const DictionaryValue* cookie_dict;
    if (!GetDictionaryParameter("cookie", &cookie_dict)) {
        response->SetError(new Error(
            kBadRequest, "Missing or invalid |cookie| parameter"));
        return;
    }
    scoped_ptr<DictionaryValue> cookie_dict_copy(cookie_dict->DeepCopy());

    std::string domain;
    if (cookie_dict_copy->GetString("domain", &domain)) {
        std::vector<std::string> split_domain;
        base::SplitString(domain, ':', &split_domain);
        if (split_domain.size() > 2) {
            response->SetError(new Error(
                kInvalidCookieDomain, "Cookie domain has too many colons"));
            return;
        } else if (split_domain.size() == 2) {
            // Remove the port number.
            cookie_dict_copy->SetString("domain", split_domain[0]);
        }
    }

    std::string url;
    Error* error = NULL;

    session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::GetURL,
            base::Unretained(executor_.get()),
            &url,
            &error));

    if (!error) {
        session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::SetCookie,
            base::Unretained(executor_.get()),
            url,
            cookie_dict_copy.get(),
            &error));
    }

    if (error) {
        response->SetError(error);
        return;
    }
}

void CookieCommand::ExecuteDelete(Response* const response) {
    std::string url;
    Error* error = NULL;

    session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::GetURL,
            base::Unretained(executor_.get()),
            &url,
            &error));

    ListValue* unscoped_cookies = NULL;
    if (!error) {
        session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::GetCookies,
            base::Unretained(executor_.get()),
            url,
            &unscoped_cookies,
            &error));
    }

    scoped_ptr<ListValue> cookies(unscoped_cookies);
    if (error) {
        response->SetError(error);
        return;
    }

    for (size_t i = 0; i < cookies->GetSize(); ++i) {
        DictionaryValue* cookie_dict;
        if (!cookies->GetDictionary(i, &cookie_dict)) {
            response->SetError(new Error(
                kUnknownError, "GetCookies returned non-dict type"));
            return;
        }
        std::string name;
        if (!cookie_dict->GetString("name", &name)) {
            response->SetError(new Error(
                kUnknownError,
                "GetCookies returned cookie with missing or invalid 'name'"));
            return;
        }
        session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::DeleteCookie,
            base::Unretained(executor_.get()),
            url,
            name,
            &error));
        if (error) {
            response->SetError(error);
            return;
        }
    }
}

NamedCookieCommand::NamedCookieCommand(
    const std::vector<std::string>& path_segments,
    const DictionaryValue* const parameters)
    : ViewCommand(path_segments, parameters) {}

NamedCookieCommand::~NamedCookieCommand() {}

bool NamedCookieCommand::Init(Response* const response) {
    if (!ViewCommand::Init(response))
        return false;

    // There should be at least 5 segments to match
    // /session/:sessionId/cookie/:name
    cookie_name_ = GetPathVariable(4);
    if (cookie_name_ == "") {
        response->SetError(new Error(kBadRequest, "No cookie name specified"));
        return false;
    }

    return true;
}

bool NamedCookieCommand::DoesDelete() const {
    return true;
}

void NamedCookieCommand::ExecuteDelete(Response* const response) {
    std::string url;
    Error* error = NULL;

    session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::GetURL,
            base::Unretained(executor_.get()),
            &url,
            &error));

    if (!error) {
        session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::DeleteCookie,
            base::Unretained(executor_.get()),
            url,
            cookie_name_,
            &error));
    }
    if (error) {
        response->SetError(error);
        return;
    }
}

}  // namespace webdriver
