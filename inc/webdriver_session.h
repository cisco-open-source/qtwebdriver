// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

/*! \page page_sessions Sessions

Session class represents object that handles all information related to WD session.
For each WD session new webdriver::Session object created and registered in webdriver::SessionManager.
Valuable information stored in Session:
- mapping ViewId into real window implementation.
- mapping ElementId into real element implemenation.
- session's wide options - timeouts, capabilities.
- running context for all operations.

<h2>Running context</h2>
Most operations on views can be performed on separated thread(ie QT UI thread).
For this purpose Session has RunSessionTask() method. And custom runnnig context 
can be added with webdriver::ViewRunner class:
\code
webdriver::ViewRunner::RegisterCustomRunner<webdriver::QViewRunner>();
\endcode

\code
session->RunSessionTask(base::Bind(
            &ViewFactory::CreateViewByClassName,
            base::Unretained(ViewFactory::GetInstance()),
            session->logger(),
            window_class,
            &viewHandle));
\endcode

<h2>Custom capabilities handling</h2>
Base capabilities are handled in webdriver::Session::Init() method.
But there is way to extend this behavior with webdriver::SessionLifeCycleActions class.
There are two methods - PostInit() and BeforeTerminate(). These methods 
automatically included in session's init/terminate sequence. Customizer 
can implement subclass of SessionLifeCycleActions and register it.
\code
webdriver::SessionLifeCycleActions::RegisterCustomLifeCycleActions<webdriver::QSessionLifeCycleActions>();
\endcode
Beside capabilities handling there can be any other actions to be run on 
session's create or session's terminate.

*/
#ifndef WEBDRIVER_WEBDRIVER_SESSION_H_
#define WEBDRIVER_WEBDRIVER_SESSION_H_

#include <map>
#include <string>
#include <vector>
#include <set>

#include "base/callback_forward.h"
#include "base/file_path.h"
#include "base/memory/scoped_ptr.h"
#include "base/scoped_temp_dir.h"
#include "base/string16.h"
#include "base/threading/thread.h"
#include "base/values.h"
#include "frame_path.h"
#include "webdriver_basic_types.h"
#include "webdriver_view_id.h"
#include "webdriver_element_id.h"
#include "webdriver_capabilities_parser.h"
#include "webdriver_logging.h"

class FilePath;

namespace base {
class DictionaryValue;
class ListValue;
class Value;
class WaitableEvent;
}

namespace webdriver {

class Error;
class ValueParser;
class ViewRunner;
class SessionLifeCycleActions;

/// Every connection made by WebDriver maps to a session object.
/// This object creates the browser instance and keeps track of the
/// state necessary to control the browser created.
/// A session manages its own lifetime.
class Session {
public:
    /// Adds this Session to the SessionManager. The session manages its own
    /// lifetime. Call Terminate, not delete, if you need to quit.
    Session();

    /// Removes this Session from the SessionManager.
    ~Session();

    /// Initializes the session with the given capabilities.
    Error* Init(const base::DictionaryValue* desired_capabilities_dict,
                const base::DictionaryValue* required_capabilities_dict);

    /// Should be called before executing a command.
    Error* BeforeExecuteCommand();

    /// Should be called after executing a command.
    Error* AfterExecuteCommand();

    /// Terminates this session and deletes itself.
    void Terminate();

  // Waits for all views to stop loading. Returns true on success.
  //Error* WaitForAllViewsToStopLoading();

    /// Returns a copy of the current log entries. Caller is responsible for
    /// returned value.
    base::ListValue* GetLog() const;

    void AddPerfLogEntry(LogLevel level, const std::string& message);

    /// Return current minimum LogLevel for Performance Logging, by default kOffLogLevel.
    LogLevel GetMinPerfLogLevel() const;

    /// Returns a copy of the current performance log entries.
    base::ListValue* GetPerfLog() const;

    /// Runs task in session's context
    /// @param task task to run
    void RunSessionTask(const base::Closure& task);

    /// Get session id
    /// @return session id
    const std::string& id() const;

    const FramePath& current_frame() const;

    void set_current_frame(const FramePath& frame);

    const ViewId& current_view() const;

    void set_current_view(const ViewId& viewId);

    void set_page_load_timeout(int timeout_ms);
    int page_load_timeout() const;

    void set_async_script_timeout(int timeout_ms);
    int async_script_timeout() const;

    void set_implicit_wait(int timeout_ms);
    int implicit_wait() const;

    const Point& get_mouse_position() const;

    void set_mouse_position(const Point& point);

    int get_sticky_modifiers() const { return sticky_modifiers_; }

    void set_sticky_modifiers(int mods) { sticky_modifiers_ = mods; }

    const DictionaryValue* get_desired_caps() const { return desired_caps_.get(); }
    const DictionaryValue* get_required_caps() const { return required_caps_.get(); }

    const Logger& logger() const;

    const FilePath& temp_dir() const;

    const Capabilities& capabilities() const;

    /// Get ViewHandle for given ViewId
    /// @param viewId requested viewId
    /// @return viewHandle for given viewId
    ViewHandle* GetViewHandle(const ViewId& viewId) const;

    /// Put ViewHandle in map and return new ViewId for this handle
    /// @param handle handle to put, no need to delete.
    /// @param[out] viewId returned viewId
    /// @return true if added, false if viewId already exist
    bool AddNewView(ViewHandle* handle, ViewId* viewId);

    /// Change mapping of viewId to handle
    /// @param viewId viewId to change
    /// @param handle new handle to map
    /// @return true if replaced, false if viewId not found
    bool ReplaceViewHandle(const ViewId& viewId, ViewHandle* handle);

    /// Check if some view maps handle
    /// @param handle to check
    /// @return valid viewId if found
    ViewId GetViewForHandle(ViewHandle* handle) const;

    /// Invalidate viewIdRemove it from map
    /// @param viewId requested view
    void RemoveView(const ViewId& viewId);

    /// Invalidate all views except passed
    /// @param views set of valid views
    void UpdateViews(const std::set<ViewId>& views);

    /// Get elementHandle for given elementId in specific view
    /// @param viewId requested view
    /// @param elementId requested element
    /// @return elementHandle
    ElementHandle* GetElementHandle(const ViewId& viewId, const ElementId& elementId) const;

    /// Get elementId for given element handle in specific view
    /// @param viewId requested view
    /// @param handle requested element handle
    /// @return elementId
    ElementId GetElementIdForHandle(const ViewId& viewId, const ElementHandle* handle) const;

    /// Add element mapping
    /// @param viewId target view
    /// @param handle pointer to element handle, no need to delete
    /// @param elementId returned elementId
    /// @return true if ok
    bool AddElement(const ViewId& viewId, ElementHandle* handle, ElementId* elementId);

    /// Invalidate elementId in specific view. Remove it from map
    /// @param viewId requested view
    /// @param elementId element to invalidate
    void RemoveElement(const ViewId& viewId, const ElementId& elementId);

    // TODO: review this logic with frames and refactor if possible
    // Vector of the |ElementId|s for each frame of the current target frame
    // path. The first refers to the first frame element in the root document.
    // If the target frame is window.top, this will be empty.
    std::vector<ElementId> frame_elements_;

private:
    typedef scoped_refptr<ElementHandle> ElementHandlePtr;
    typedef std::map<std::string, ElementHandlePtr> ElementsMap;
    typedef std::map<std::string, ElementsMap> ViewsElementsMap;
    typedef std::map<std::string, ViewHandlePtr> ViewsMap;

    bool InitActualCapabilities();
    bool CheckRequiredCapabilities(const base::DictionaryValue* capabilities_dict);
    bool CheckRequiredBrowser(const base::DictionaryValue* capabilities_dict);
    bool CheckRequiredPlatform(const base::DictionaryValue* capabilities_dict);
    bool CheckRequiredViewType(const base::DictionaryValue* capabilities_dict);
    bool CheckRequiredCapabilityBoolean(const base::DictionaryValue* capabilities_dict, const std::string& cap_name);

    void RunClosureOnSessionThread(
        const base::Closure& task,
        base::WaitableEvent* done_event);

    scoped_ptr<InMemoryLog> session_log_;
    scoped_ptr<PerfLog> session_perf_log_;
    Logger logger_;

    // session id
    const std::string id_;
    ViewId current_view_id_;
    FramePath current_frame_path_;

    // for each viewId contains mapping elementId on elementHandle
    ViewsElementsMap elements_;
    // contains mapping viewId on viewHandle
    ViewsMap views_;

    base::Thread thread_;

    // Timeout (in ms) for page loads.
    int page_load_timeout_;

    // Timeout (in ms) for asynchronous script execution.
    int async_script_timeout_;

    // Time (in ms) of how long to wait while searching for a single element.
    int implicit_wait_;

    // Last mouse position. Advanced APIs need this value.
    Point mouse_position_;

    // Temporary directory containing session data.
    ScopedTempDir temp_dir_;
    Capabilities capabilities_;
    scoped_ptr<DictionaryValue> desired_caps_;
    scoped_ptr<DictionaryValue> required_caps_;

    scoped_ptr<ViewRunner> view_runner_;
    scoped_ptr<SessionLifeCycleActions> life_cycle_actions_;

    // Current state of all modifier keys.
    int sticky_modifiers_;

    DISALLOW_COPY_AND_ASSIGN(Session);
};

/// base class for session lifecycle actions.
/// Contains default imlementation.
class SessionLifeCycleActions {
protected:
    typedef SessionLifeCycleActions* (*CreateLifeCycleActionsMethod)(Session*);
public:
    SessionLifeCycleActions(Session* session);
    virtual ~SessionLifeCycleActions() {};

    /// action after usual session Init().
    virtual Error* PostInit(const base::DictionaryValue* desired_capabilities_dict,
                const base::DictionaryValue* required_capabilities_dict);

    /// action before session terminated.
    virtual void BeforeTerminate(void);

    /// Create SessionLifeCycleActions
    /// @return new SessionLifeCycleActions object.
    static SessionLifeCycleActions* CreateLifeCycleActions(Session* session);

    /// Register custom SessionLifeCycleActions.
    /// @tparam C subclass of SessionLifeCycleActions with custom implementation
    template <class C>
    static void RegisterCustomLifeCycleActions(void) {
        create = &createLifeCycleActions<C>;
    }

protected:
    Session* session_;
    template <class C>
    static SessionLifeCycleActions* createLifeCycleActions(Session* session) { return static_cast<SessionLifeCycleActions*>(new C(session));}

    static CreateLifeCycleActionsMethod create;
private:

    DISALLOW_COPY_AND_ASSIGN(SessionLifeCycleActions);
};



}  // namespace webdriver

#endif  // WEBDRIVER_WEBDRIVER_SESSION_H_
