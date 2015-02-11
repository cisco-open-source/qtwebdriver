// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "webdriver_session.h"

#include <sstream>
#include <vector>

#include "base/bind.h"
#include "base/callback.h"
#include "base/sys_info.h"
#include "base/file_path.h"
#include "base/file_util.h"
#include "base/json/json_reader.h"
#include "base/json/json_writer.h"
#include "base/memory/scoped_ptr.h"
#include "base/message_loop_proxy.h"
#include "base/string_number_conversions.h"
#include "base/string_split.h"
#include "base/string_util.h"
#include "base/stringprintf.h"
#include "base/synchronization/waitable_event.h"
#include "base/threading/platform_thread.h"
#include "base/time.h"
#include "base/utf_string_conversions.h"
#include "base/values.h"
#include "webdriver_error.h"
#include "webdriver_session_manager.h"
#include "webdriver_view_runner.h"
#include "webdriver_util.h"
#include "webdriver_view_executor.h"
#include "commands/set_timeout_commands.h"

#if !defined(OS_WIN)
#include <sys/types.h>
#include <dirent.h>
#endif

namespace webdriver {

Session::Session()
    : session_log_(new InMemoryLog()),
      session_perf_log_(new PerfLog()),
      logger_(kAllLogLevel),
      id_(GenerateRandomID()),
      current_view_id_(ViewId()),
      current_frame_path_(FramePath()),
      thread_(id_.c_str()),
      page_load_timeout_(SetTimeoutCommand::DEFAULT_TIMEOUT),
      async_script_timeout_(0),
      implicit_wait_(0),
      desired_caps_(NULL),
      required_caps_(NULL),
      view_runner_(ViewRunner::CreateRunner()),
      sticky_modifiers_(0)
{
    SessionManager::GetInstance()->Add(this);
    logger_.AddHandler(session_log_.get());
    if (FileLog::Get())
        logger_.AddHandler(FileLog::Get());
    if (StdOutLog::Get())
        logger_.AddHandler(StdOutLog::Get());
    life_cycle_actions_ = make_scoped_ptr(SessionLifeCycleActions::CreateLifeCycleActions(this));
}

Session::~Session() {
    SessionManager::GetInstance()->Remove(id_);
}

bool Session::InitActualCapabilities() {
    // Standard capabilities defined at
    // http://code.google.com/p/selenium/wiki/JsonWireProtocol#Capabilities_JSON_Object
    capabilities_.caps->SetString(Capabilities::kBrowserName, "QtWebkit");
    // TODO: get version of QT webkit?
    //capabilities_value->SetString("version", session->GetBrowserVersion());

    capabilities_.caps->SetString(Capabilities::kPlatform, base::SysInfo::OperatingSystemName());
    capabilities_.caps->SetBoolean(Capabilities::kJavascriptEnabled, true);
    capabilities_.caps->SetBoolean(Capabilities::kTakesScreenshot, true);
    capabilities_.caps->SetBoolean(Capabilities::kTakesElementScreenshot, true);
    capabilities_.caps->SetBoolean(Capabilities::kRemotePlayerEnabled, true);
    capabilities_.caps->SetBoolean(Capabilities::kHandlesAlerts, true);
    capabilities_.caps->SetBoolean(Capabilities::kDatabaseEnabled, false);
    capabilities_.caps->SetBoolean(Capabilities::kLocationContextEnabled, false);
    capabilities_.caps->SetBoolean(Capabilities::kApplicationCacheEnabled, true);
    capabilities_.caps->SetBoolean(Capabilities::kBrowserConnectionEnabled, true);
    capabilities_.caps->SetBoolean(Capabilities::kCssSelectorsEnabled, true);
    capabilities_.caps->SetBoolean(Capabilities::kWebStorageEnabled, true);
    capabilities_.caps->SetBoolean(Capabilities::kRotatable, false);
    capabilities_.caps->SetBoolean(Capabilities::kAcceptSslCerts, false);
    capabilities_.caps->SetBoolean(Capabilities::kNativeEvents, true);

    ViewCmdExecutorFactory* executorFactory = ViewCmdExecutorFactory::GetInstance();
    const ViewCmdExecutorFactory::SupportedViewTypesList typesList = executorFactory->getSupportedViewTypesList();
    base::DictionaryValue *hybrid = new base::DictionaryValue;
    ViewCmdExecutorFactory::SupportedViewTypesList::const_iterator iter = typesList.begin();
    for (; iter != typesList.end(); iter++) {
        hybrid->SetBoolean(*iter, true);
    }
    capabilities_.caps->Set(Capabilities::kHybrid, hybrid);

    bool reuse_ui;
    if (required_caps_.get()) {
        if (!required_caps_->GetBoolean(Capabilities::kReuseUI, &reuse_ui))
            desired_caps_->GetBoolean(Capabilities::kReuseUI, &reuse_ui);
    } else if (desired_caps_.get()) {
        desired_caps_->GetBoolean(Capabilities::kReuseUI, &reuse_ui);
    } else {
        reuse_ui = false;
    }
    capabilities_.caps->SetBoolean(Capabilities::kReuseUI, reuse_ui);

    return true;
}

bool Session::CheckRequiredCapabilities(const base::DictionaryValue* capabilities_dict) {
    if (!CheckRequiredPlatform(capabilities_dict))
        return false;

    if (!CheckRequiredBrowser(capabilities_dict))
        return false;

    if(!CheckRequiredViewType(capabilities_dict))
        return false;

    if (!CheckRequiredCapabilityBoolean(capabilities_dict, Capabilities::kJavascriptEnabled))
        return false;

    if (!CheckRequiredCapabilityBoolean(capabilities_dict, Capabilities::kTakesScreenshot))
        return false;

    if (!CheckRequiredCapabilityBoolean(capabilities_dict, Capabilities::kTakesElementScreenshot))
        return false;

    if (!CheckRequiredCapabilityBoolean(capabilities_dict, Capabilities::kRemotePlayerEnabled))
        return false;

    if (!CheckRequiredCapabilityBoolean(capabilities_dict, Capabilities::kHandlesAlerts))
        return false;

    if (!CheckRequiredCapabilityBoolean(capabilities_dict, Capabilities::kDatabaseEnabled))
        return false;

    if (!CheckRequiredCapabilityBoolean(capabilities_dict, Capabilities::kLocationContextEnabled))
        return false;

    if (!CheckRequiredCapabilityBoolean(capabilities_dict, Capabilities::kApplicationCacheEnabled))
        return false;

    if (!CheckRequiredCapabilityBoolean(capabilities_dict, Capabilities::kBrowserConnectionEnabled))
        return false;

    if (!CheckRequiredCapabilityBoolean(capabilities_dict, Capabilities::kCssSelectorsEnabled))
        return false;

    if (!CheckRequiredCapabilityBoolean(capabilities_dict, Capabilities::kWebStorageEnabled))
        return false;

    if (!CheckRequiredCapabilityBoolean(capabilities_dict, Capabilities::kRotatable))
        return false;

    if (!CheckRequiredCapabilityBoolean(capabilities_dict, Capabilities::kNativeEvents))
        return false;

    return true;
}

bool Session::CheckRequiredBrowser(const base::DictionaryValue* capabilities_dict) {
    std::string required_browser;
    std::string actual_browser;

    if (capabilities_dict->GetString(Capabilities::kBrowserName, &required_browser)) {
        capabilities_.caps->GetString(Capabilities::kBrowserName, &actual_browser);

        if (StringToLowerASCII(actual_browser) != StringToLowerASCII(required_browser)) {
            logger_.Log(kWarningLogLevel,
                "check failed - required browser("+required_browser+"), actual browser("+actual_browser+").");
            return false;
        }
    }

    return true;
}

bool Session::CheckRequiredPlatform(const base::DictionaryValue* capabilities_dict) {
    std::string required_platform;
    std::string actual_platform;

    if (capabilities_dict->GetString(Capabilities::kPlatform, &required_platform)) {
        capabilities_.caps->GetString(Capabilities::kPlatform, &actual_platform);

        if (StringToUpperASCII(required_platform) == "ANY")
            return true;

        if (StringToLowerASCII(actual_platform) != StringToLowerASCII(required_platform)) {
            logger_.Log(kWarningLogLevel,
                "check failed - required platform("+required_platform+"), actual platform("+actual_platform+").");
            return false;
        }
    }

    return true;
}

bool Session::CheckRequiredViewType(const base::DictionaryValue *capabilities_dict) {

    const base::DictionaryValue *required_hybrid_caps;
    base::DictionaryValue *actual_hybrid_caps;

    if (capabilities_dict->GetDictionary(Capabilities::kHybrid, &required_hybrid_caps)) {
        if (!capabilities_.caps->GetDictionary(Capabilities::kHybrid, &actual_hybrid_caps)) {
            return false;
        }
        base::DictionaryValue::key_iterator key_iter = required_hybrid_caps->begin_keys();
        for (; key_iter != required_hybrid_caps->end_keys(); ++key_iter) {
            bool actual_value;
            bool required_value;
            if (required_hybrid_caps->GetBoolean(*key_iter, &required_value)) {
                if (!actual_hybrid_caps->GetBoolean(*key_iter, &actual_value)) {
                    logger_.Log(kWarningLogLevel, "check failed - view type doesn't set as bool..");
                    return false;
                }
                if (required_value != actual_value) {
                    logger_.Log(kWarningLogLevel, "check failed - view type doesn't match...");
                    return false;
                }
            }
        }
    }

    return true;
}

bool Session::CheckRequiredCapabilityBoolean(const base::DictionaryValue* capabilities_dict, const std::string& cap_name) {
    bool actual_value; 
    bool required_value;
    if (capabilities_dict->GetBoolean(cap_name, &required_value)) {
        if (required_value) {
            if (capabilities_.caps->GetBoolean(cap_name, &actual_value)) {
                if (actual_value) 
                    return true;

                logger_.Log(kWarningLogLevel, "actual capability("+cap_name+") is \"false\" but required \"true\".");
                return false;
            }

            logger_.Log(kWarningLogLevel, "required capability("+cap_name+") is not present in actual caps.");
            return false;
        }
    }    

    return true;
}

Error* Session::Init(const base::DictionaryValue* desired_capabilities_dict,
                    const base::DictionaryValue* required_capabilities_dict) {

    if (!thread_.Start()) {
        delete this;
        return new Error(kUnknownError, "Cannot start session thread");
    }

    if (!temp_dir_.CreateUniqueTempDir()) {
        delete this;
        return new Error(
            kUnknownError, "Unable to create temp directory");
    }

    desired_caps_.reset(desired_capabilities_dict->DeepCopy());

    logger_.Log(kFineLogLevel,
                    "Initializing session with desired capabilities " +
                    JsonStringifyForDisplay(desired_capabilities_dict));

    (void) InitActualCapabilities();

    if (required_capabilities_dict) {
        required_caps_.reset(required_capabilities_dict->DeepCopy());

        logger_.Log(kFineLogLevel,
                    "Initializing session with required capabilities " +
                    JsonStringifyForDisplay(required_capabilities_dict));

        if (!CheckRequiredCapabilities(required_capabilities_dict)) {
            logger_.Log(kWarningLogLevel, "Required caps check failed.");
            delete this;
            return new Error(kUnknownError, "Required caps check failed.");
        }
    }

    CapabilitiesParser parser(desired_capabilities_dict, logger_, &capabilities_);
    Error* error = parser.Parse();
    if (error) {
        delete this;
        return error;
    }
    logger_.set_min_log_level(capabilities_.log_levels[LogType::kDriver]);
    if (capabilities_.log_levels[LogType::kPerformance] != kOffLogLevel) {
        session_perf_log_->set_min_log_level(capabilities_.log_levels[LogType::kPerformance]);
    }

    error = life_cycle_actions_->PostInit(desired_capabilities_dict, required_capabilities_dict);

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

    life_cycle_actions_->BeforeTerminate();

    delete this;
}

const std::string& Session::id() const {
    return id_;
}

const FramePath& Session::current_frame() const {
    return current_frame_path_;
}

void Session::set_current_frame(const FramePath& frame) {
    current_frame_path_ = frame;
}

const ViewId& Session::current_view() const {
    return current_view_id_;
}

void Session::set_current_view(const ViewId& viewId) {
    current_view_id_ = viewId;
}

void Session::set_page_load_timeout(int timeout_ms) {
    page_load_timeout_ = timeout_ms;
}

int Session::page_load_timeout() const {
    return page_load_timeout_;
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

void Session::set_mouse_position(const Point& point) {
    mouse_position_ = point;
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

void Session::AddPerfLogEntry(LogLevel level, const std::string& message) {
    if (!session_perf_log_.get()) {
        return;
    }
    base::Time time = base::Time::Now();
    session_perf_log_->Log(level, time, message);
}

LogLevel Session::GetMinPerfLogLevel() const {
    return session_perf_log_->min_log_level();
}

base::ListValue* Session::GetPerfLog() const {
    base::ListValue* ret_val = session_perf_log_->entries_list()->DeepCopy();
    session_perf_log_->clear_entries_list();
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

ViewHandle* Session::GetViewHandle(const ViewId& viewId) const {
    ViewsMap::const_iterator it;

    it = views_.find(viewId.id());
    if (it == views_.end())
        return NULL;
    return it->second.get();
}

bool Session::AddNewView(ViewHandle* handle, ViewId* viewId) {
    ViewId newView(GenerateRandomID());

    // TODO: check if view id already exist and return false

    views_[newView.id()] = ViewHandlePtr(handle);

    *viewId = newView;

    return true;   
}

bool Session::ReplaceViewHandle(const ViewId& viewId, ViewHandle* handle) {
    ViewsMap::iterator it;

    it = views_.find(viewId.id());
    if (it == views_.end())
        return false;

    it->second = ViewHandlePtr(handle);

    return true;
}

ViewId Session::GetViewForHandle(ViewHandle* handle) const {
    ViewId view_to_return;
    ViewsMap::const_iterator it;

    for (it = views_.begin(); it != views_.end(); ++it) {
        if (handle->equals((it->second).get())) {
            view_to_return = ViewId(it->first);
            break;
        }
    }

    return view_to_return;
};

void Session::RemoveView(const ViewId& viewId) {
    elements_.erase(viewId.id());
    views_.erase(viewId.id());
}

void Session::UpdateViews(const std::set<ViewId>& views) {
    ViewsMap::iterator it;
    ViewId vi;

    for (it = views_.begin(); it != views_.end(); ++it) {
        vi = ViewId(it->first);
		
        if (vi.is_valid() && 0 == views.count(vi)) {
            // invalidate handle
            RemoveView(vi);
        }
    }
}

ElementHandle* Session::GetElementHandle(const ViewId& viewId, const ElementId& elementId) const {
    ViewsElementsMap::const_iterator it_view;

    it_view = elements_.find(viewId.id());
    if (it_view == elements_.end())
        return NULL;

    ElementsMap::const_iterator it_el;
    it_el = it_view->second.find(elementId.id());
    if (it_el == it_view->second.end())
        return NULL;

    return (it_el->second).get();
}

ElementId Session::GetElementIdForHandle(const ViewId& viewId, const ElementHandle* handle) const {
    ViewsElementsMap::const_iterator viewIt;

    viewIt = elements_.find(viewId.id());
    if (viewIt == elements_.end())
        return ElementId();

    const ElementsMap& elements = viewIt->second;
    for (ElementsMap::const_iterator elementIt = elements.begin(); elementIt != elements.end(); ++elementIt) {
        if (elementIt->second->equals(handle)) {
            return ElementId(elementIt->first);
        }
    }

    return ElementId();
}

bool Session::AddElement(const ViewId& viewId, ElementHandle* handle, ElementId* elementId) {
    ElementId targetElement(GenerateRandomID());
    ElementsMap& elements = elements_[viewId.id()];
    elements[targetElement.id()] = ElementHandlePtr(handle);
    *elementId = targetElement;
    return true;
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


SessionLifeCycleActions::CreateLifeCycleActionsMethod SessionLifeCycleActions::create = &(SessionLifeCycleActions::createLifeCycleActions<SessionLifeCycleActions>);

SessionLifeCycleActions::SessionLifeCycleActions(Session* session)
    : session_(session) {}

Error* SessionLifeCycleActions::PostInit(const base::DictionaryValue* desired_capabilities_dict,
                const base::DictionaryValue* required_capabilities_dict) {
    session_->logger().Log(kFineLogLevel, "Default PostInit action: do nothing.");
    return NULL;
}

void SessionLifeCycleActions::BeforeTerminate(void) {
    session_->logger().Log(kFineLogLevel, "Default BeforeTerminate action: do nothing.");
}

SessionLifeCycleActions* SessionLifeCycleActions::CreateLifeCycleActions(Session* session) {
    return create(session);
}

}  // namespace webdriver
