// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "webdriver_session.h"

#include <sstream>
#include <vector>

#include "base/bind.h"
#include "base/callback.h"
//#include "base/command_line.h"
#include "base/file_path.h"
#include "base/file_util.h"
#include "base/json/json_reader.h"
#include "base/json/json_writer.h"
#include "base/memory/scoped_ptr.h"
#include "base/message_loop_proxy.h"
//#include "base/process.h"
//#include "base/process_util.h"
#include "base/string_number_conversions.h"
#include "base/string_split.h"
#include "base/string_util.h"
#include "base/stringprintf.h"
#include "base/synchronization/waitable_event.h"
//#include "base/test/test_timeouts.h"
#include "base/threading/platform_thread.h"
#include "base/time.h"
#include "base/utf_string_conversions.h"
#include "base/values.h"
#include "webdriver_error.h"
#include "webdriver_session_manager.h"
#include "webdriver_view_runner.h"
#include "webdriver_util.h"

#if !defined(OS_WIN)
#include <sys/types.h>
#include <dirent.h>
#endif

namespace webdriver {

Session::Session()
    : session_log_(new InMemoryLog()),
      logger_(kAllLogLevel),
      id_(GenerateRandomID()),
      current_view_id_(ViewId()),
      current_frame_path_(FramePath()),
      thread_(id_.c_str()),
      async_script_timeout_(0),
      implicit_wait_(0),
      view_runner_(ViewRunner::CreateRunner())
{
    SessionManager::GetInstance()->Add(this);
    logger_.AddHandler(session_log_.get());
    if (FileLog::Get())
        logger_.AddHandler(FileLog::Get());
    if (StdOutLog::Get())
        logger_.AddHandler(StdOutLog::Get());
}

Session::~Session() {
    SessionManager::GetInstance()->Remove(id_);
}

Error* Session::Init(const DictionaryValue* capabilities_dict) {

    if (!thread_.Start()) {
        delete this;
        return new Error(kUnknownError, "Cannot start session thread");
    }

//  qtask.moveToThread(QApplication::instance()->thread());

    if (!temp_dir_.CreateUniqueTempDir()) {
        delete this;
        return new Error(
            kUnknownError, "Unable to create temp directory");
    }
    logger_.Log(kFineLogLevel,
                    "Initializing session with capabilities " +
                    JsonStringifyForDisplay(capabilities_dict));

    CapabilitiesParser parser(capabilities_dict, logger_, &capabilities_);
    Error* error = parser.Parse();
    if (error) {
        delete this;
        return error;
    }
    logger_.set_min_log_level(capabilities_.log_levels[LogType::kDriver]);

    if (error)
        Terminate();

    return error;
}

Error* Session::BeforeExecuteCommand() {
    // TODO: review
    return NULL;
    /*
    Error* error = AfterExecuteCommand();
    if (!error) {
        scoped_ptr<Error> switch_error(SwitchToTopFrameIfCurrentFrameInvalid());
        if (switch_error.get()) {
            std::string text;
            scoped_ptr<Error> alert_error(GetAlertMessage(&text));
            if (alert_error.get()) {
                // Only return a frame checking error if a modal dialog is not present.
                // TODO(kkania): This is ugly. Fix.
                return switch_error.release();
            }
        }
    }
    return error;
    */
}

Error* Session::AfterExecuteCommand() {
    Error* error = NULL;
    if (!capabilities_.load_async) {
        // TODO: handle this
        //error = WaitForAllViewsToStopLoading();
    }
    return error;
}

void Session::Terminate() {
    logger_.Log(kInfoLogLevel, "Session("+id_+") terminate.");

    // TODO: cleanup ressources
    delete this;
}

const std::string& Session::id() const {
    return id_;
}

const FramePath& Session::current_frame() const {
    return current_frame_path_;
}

const ViewId& Session::current_view() const {
    return current_view_id_;
}

void Session::set_current_view(const ViewId& viewId) {
    current_view_id_ = viewId;
}

void Session::set_async_script_timeout(int timeout_ms) {
    async_script_timeout_ = timeout_ms;
}

int Session::async_script_timeout() const {
    return async_script_timeout_;
}

void Session::set_implicit_wait(int timeout_ms) {
    implicit_wait_ = timeout_ms;
}

int Session::implicit_wait() const {
    return implicit_wait_;
}

const Point& Session::get_mouse_position() const {
    return mouse_position_;
}

const Logger& Session::logger() const {
    return logger_;
}

const FilePath& Session::temp_dir() const {
    return temp_dir_.path();
}

const Capabilities& Session::capabilities() const {
    return capabilities_;
}

base::ListValue* Session::GetLog() const {
    base::ListValue* ret_val = session_log_->entries_list()->DeepCopy();
    session_log_->clear_entries_list();
    return ret_val;
}

void Session::RunSessionTask(const base::Closure& task) {
    base::WaitableEvent done_event(false, false);
    thread_.message_loop_proxy()->PostTask(FROM_HERE, base::Bind(
        &Session::RunClosureOnSessionThread,
        base::Unretained(this),
        task,
        &done_event));
    // See SetCookie for why it is essential that we wait here.
    done_event.Wait();
}

void Session::GetViews(std::vector<ViewId>* views) const {
    if (NULL == views) return;

    ViewsMap::const_iterator view;
    
    for (view = views_.begin(); view != views_.end(); ++view) {
        views->push_back(ViewId(view->first));
    }
}

ViewHandle Session::GetViewHandle(const ViewId& viewId) const {
    ViewsMap::const_iterator it;

    it = views_.find(viewId.id());
    if (it == views_.end())
        return INVALID_HANDLE;
    return it->second;
}

bool Session::AddNewView(const ViewHandle handle, ViewId* viewId) {
    ViewId newView(GenerateRandomID());

    views_[newView.id()] = handle;

    *viewId = newView;

    return true;   
}

void Session::RemoveView(const ViewId& viewId) {
    elements_.erase(viewId.id());
    views_.erase(viewId.id());
}

ElementHandle Session::GetElementHandle(const ViewId& viewId, const ElementId& elementId) const {
    ViewsElementsMap::const_iterator it_view;

    it_view = elements_.find(viewId.id());
    if (it_view == elements_.end())
        return INVALID_HANDLE;

    ElementsMap::const_iterator it_el;
    it_el = it_view->second.find(elementId.id());
    if (it_el == it_view->second.end())
        return INVALID_HANDLE;

    return it_el->second;
}

void Session::RemoveElement(const ViewId& viewId, const ElementId& elementId) {
    ViewsElementsMap::iterator it_view;

    it_view = elements_.find(viewId.id());
    if (it_view == elements_.end())
        return;

    it_view->second.erase(elementId.id());
}

void Session::RunClosureOnSessionThread(const base::Closure& task,
                                        base::WaitableEvent* done_event) {
    view_runner_->RunClosure(task, done_event);
//    QMetaObject::invokeMethod(&qtask, "runTask", Qt::BlockingQueuedConnection, Q_ARG(const base::Closure&, task));
//    done_event->Signal();
}

}  // namespace webdriver
