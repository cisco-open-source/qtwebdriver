// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "commands/create_session.h"

#include <string>

#include "base/file_path.h"
#include "base/scoped_temp_dir.h"
#include "base/values.h"
#include "base/bind.h"
#include "commands/response.h"
#include "webdriver_error.h"
#include "webdriver_server.h"
#include "webdriver_session.h"
#include "webdriver_session_manager.h"
#include "webdriver_view_executor.h"
#include "webdriver_view_enumerator.h"
#include "webdriver_view_factory.h"
#include "webdriver_util.h"

namespace webdriver {

typedef scoped_ptr<ViewCmdExecutor> ExecutorPtr;  

CreateSession::CreateSession(const std::vector<std::string>& path_segments,
                             const DictionaryValue* const parameters)
    : Command(path_segments, parameters) {}

CreateSession::~CreateSession() {}

bool CreateSession::DoesPost() const { return true; }

void CreateSession::ExecutePost(Response* const response) {
    const DictionaryValue* desired_caps_dict;
    const DictionaryValue* required_caps_dict = NULL;
    if (!GetDictionaryParameter("desiredCapabilities", &desired_caps_dict)) {
        response->SetError(new Error(
            kBadRequest, "Missing or invalid 'desiredCapabilities'"));
        return;
    }

    // get optional required capabilities
    (void)GetDictionaryParameter("requiredCapabilities", &required_caps_dict);

    std::map<std::string, Session*> sessionMap = SessionManager::GetInstance()->GetSessions();
    if (sessionMap.size() > 0) {
        // session map can consist only single session at the moment
        Session* prev_session = sessionMap.begin()->second;
        bool reuse_ui = false;
        if (required_caps_dict) {
            if (!required_caps_dict->GetBoolean(Capabilities::kReuseUI, &reuse_ui))
                desired_caps_dict->GetBoolean(Capabilities::kReuseUI, &reuse_ui);
        } else {
            desired_caps_dict->GetBoolean(Capabilities::kReuseUI, &reuse_ui);
        }

        if (reuse_ui) {
            prev_session->Terminate();
        } else {
            response->SetError(new Error(kUnknownError, "Cannot start session. WD support only one session at the moment"));
            return;
        }
    }

    // Session manages its own liftime, so do not call delete.
    Session* session = new Session();
    Error* error = session->Init(desired_caps_dict, required_caps_dict);
    if (error) {
        response->SetError(error);
        return;
    }

    std::string browser_start_window;
    std::string window_class;
    ViewId startView;
    bool wereRequiredCaps = false;

    if (NULL != required_caps_dict) {
        if (required_caps_dict->GetString(Capabilities::kBrowserStartWindow, &browser_start_window)) {
            wereRequiredCaps = true;
            FindAndAttachView(session, browser_start_window, &startView);
        } else if (required_caps_dict->GetString(Capabilities::kBrowserClass, &window_class)) {
            wereRequiredCaps = true;
            CreateViewByClassName(session, window_class, &startView);
        }
    }

    if (!wereRequiredCaps) {
        if (desired_caps_dict->GetString(Capabilities::kBrowserStartWindow, &browser_start_window)) {
            FindAndAttachView(session, browser_start_window, &startView);
        }

        if (!startView.is_valid()) {
            if (desired_caps_dict->GetString(Capabilities::kBrowserClass, &window_class)) {
                CreateViewByClassName(session, window_class, &startView);
            } else {
                CreateViewByClassName(session, "", &startView);
            }
        }
    }

    if (!startView.is_valid()) {
        session->logger().Log(kSevereLogLevel, "Session("+session->id()+") no view ids.");
        response->SetError(new Error(kUnknownError, "No view ids after initialization"));
        session->Terminate();
        return;   
    }

    session->logger().Log(kInfoLogLevel, "Session("+session->id()+") view: "+startView.id());
    // set current view
    error = SwitchToView(session, startView);
    if (error) {
        response->SetError(error);
        session->Terminate();
        return;
    }

    scoped_ptr<Error> ignore_err(SetWindowBounds(desired_caps_dict, session, startView));

    if (ignore_err != NULL) {
        session->logger().Log(kWarningLogLevel, "Error in CreateSession::SetWindowBounds. Can't create window with desired capabilities");
    }

    // Redirect to a relative URI. Although prohibited by the HTTP standard,
    // this is what the IEDriver does. Finding the actual IP address is
    // difficult, and returning the hostname causes perf problems with the python
    // bindings on Windows.
    std::ostringstream stream;
    stream << Server::GetInstance()->url_base() << "/session/"
            << session->id();
    response->SetStatus(kSeeOther);
    response->SetValue(Value::CreateStringValue(stream.str()));
}

bool CreateSession::FindAndAttachView(Session* session, const std::string& name, ViewId* viewId) {
    // enumerate all views
    session->logger().Log(kFineLogLevel, "Trying to attach to window - "+name);

    std::vector<ViewId> views;
    std::string window_name;

    session->RunSessionTask(base::Bind(
        &ViewEnumerator::EnumerateViews,
        session,
        &views));

    // find view to attach
    std::vector<ViewId>::const_iterator it = views.begin();
    scoped_ptr<Error> ignore_error(NULL);

    while (it != views.end()) {
        ignore_error.reset(GetViewTitle(session, *it, &window_name));
        if (NULL != ignore_error.get()) break;

        if (name == "*") {
            *viewId = *it;
            return true;
        }

        if (window_name == name) {
            *viewId = *it;
            return true;
        }
            
        ++it;
    }

    session->logger().Log(kWarningLogLevel, "Searching window - "+name+" - not found.");

    return false;
}

bool CreateSession::CreateViewByClassName(Session* session, const std::string& name, ViewId* viewId) {
    session->logger().Log(kFineLogLevel, "Trying to create window - "+name);

    ViewHandle* viewHandle = NULL;

    // create view

    typedef void (ViewFactory::*CreateViewByClassName)(const Logger&, const std::string&, ViewHandle**) const;
    CreateViewByClassName createViewByClassName = static_cast<CreateViewByClassName>(&ViewFactory::CreateViewByClassName);

    session->RunSessionTask(base::Bind(
        createViewByClassName,
        base::Unretained(ViewFactory::GetInstance()),
        session->logger(),
        name,
        &viewHandle));

    if (NULL != viewHandle) {
        session->AddNewView(viewHandle, viewId);
        if (!viewId->is_valid()) {
            viewHandle->Release();
            session->logger().Log(kSevereLogLevel, "Cant add view handle to session.");
            return false;
        }
        return true;
    }

    return false;
}

Error* CreateSession::SwitchToView(Session* session, const ViewId& viewId) {
    Error* error = NULL;
    ExecutorPtr executor(ViewCmdExecutorFactory::GetInstance()->CreateExecutor(session, viewId));

    if (NULL == executor.get()) {
        return new Error(kBadRequest, "cant get view executor.");
    }

    session->logger().Log(kFineLogLevel, "start view ("+viewId.id()+")");

    session->RunSessionTask(base::Bind(
                &ViewCmdExecutor::SwitchTo,
                base::Unretained(executor.get()),
                &error));

    return error;
}

Error* CreateSession::GetViewTitle(Session* session, const ViewId& viewId, std::string* title) {
    Error* error = NULL;
    ExecutorPtr executor(ViewCmdExecutorFactory::GetInstance()->CreateExecutor(session, viewId));

    if (NULL == executor.get()) {
        return new Error(kBadRequest, "cant get view executor.");
    }

    session->RunSessionTask(base::Bind(
                &ViewCmdExecutor::GetWindowName,
                base::Unretained(executor.get()),
                title,
                &error));

    return error;
}

Error* CreateSession::SetWindowBounds(const DictionaryValue* desired_caps_dict,Session* session, ViewId startView) {
    Error* error = NULL;
    ExecutorPtr executor(ViewCmdExecutorFactory::GetInstance()->CreateExecutor(session, startView));
    if (NULL == executor.get()) {
        error = new Error(kBadRequest, "cant get view executor.");
        session->logger().Log(kWarningLogLevel, "Can't get view executor.");
        return error;
    }

    bool maximized;
    if (desired_caps_dict->GetBoolean(Capabilities::kMaximize, &maximized)) {
        if (maximized) {
            session->RunSessionTask(base::Bind(
                    &ViewCmdExecutor::Maximize,
                    base::Unretained(executor.get()),
                    &error));
            if (error) {
                session->logger().Log(kWarningLogLevel, "Can't maximized window ");
            }
            return error;
        }
    }

    Rect currentbounds;
    session->RunSessionTask(base::Bind(
            &ViewCmdExecutor::GetBounds,
            base::Unretained(executor.get()),
            &currentbounds,
            &error));

    if (error) {
        session->logger().Log(kWarningLogLevel, "Can't get current rect ");
        return error;
    }

    int x, y, w, h;
    bool changed = false;
    std::string window_size;
    if (desired_caps_dict->GetString(Capabilities::kWindowSize, &window_size)) {
        if (GetTwoIntsFromString(window_size, w, h)) {
            x = currentbounds.x();
            y = currentbounds.y();
            changed = true;
        } else {
            session->logger().Log(kInfoLogLevel, "Wrong parameter kWindowSize ");
            return error;
        }
    }

    std::string window_position;
    if (desired_caps_dict->GetString(Capabilities::kWindowPosition, &window_position)) {
        if (GetTwoIntsFromString(window_position, x, y)) {
            if (!changed) {
                w = currentbounds.width();
                h = currentbounds.height();
            }
            changed = true;
        } else {
            session->logger().Log(kInfoLogLevel, "Wrong parameter kWindowPosition");
            return error;
        }
    }

    Rect desiredbounds(x, y, w, h);
    if (changed) {
        session->RunSessionTask(base::Bind(
                &ViewCmdExecutor::SetBounds,
                base::Unretained(executor.get()),
                desiredbounds,
                &error));

        if (error) {
            session->logger().Log(kWarningLogLevel, "Can't create window with desired bounds ");
            return error;
        }
    }
    return error;
}

}  // namespace webdriver
