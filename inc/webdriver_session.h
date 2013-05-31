// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WEBDRIVER_WEBDRIVER_SESSION_H_
#define WEBDRIVER_WEBDRIVER_SESSION_H_

#include <map>
#include <string>
#include <vector>

#include "base/callback_forward.h"
#include "base/file_path.h"
#include "base/memory/scoped_ptr.h"
#include "base/scoped_temp_dir.h"
#include "base/string16.h"
#include "base/threading/thread.h"
#include "frame_path.h"
#include "webdriver_basic_types.h"
#include "webdriver_view_id.h"
#include "webdriver_element_id.h"
#include "webdriver_capabilities_parser.h"
#include "webdriver_element_id.h"
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

typedef std::map<std::string, ElementHandle> ElementsMap;
typedef std::map<std::string, ElementsMap> ViewsElementsMap;
typedef std::map<std::string, ViewHandle> ViewsMap;

enum StorageType {
    kLocalStorageType = 0,
    kSessionStorageType
};

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
    Error* Init(const base::DictionaryValue* capabilities_dict);

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

    void set_async_script_timeout(int timeout_ms);
    int async_script_timeout() const;

    void set_implicit_wait(int timeout_ms);
    int implicit_wait() const;

    const Point& get_mouse_position() const;

    void set_mouse_position(const Point& point);

    const Logger& logger() const;

    const FilePath& temp_dir() const;

    const Capabilities& capabilities() const;

    /// Gets list of enumerated viewIds
    /// @param[out] views vector of viewIds to return
    void GetViews(std::vector<ViewId>* views) const;

    /// Get ViewHandle for given ViewId
    /// @param viewId requested viewId
    /// @return viewHandle for given viewId
    ViewHandle GetViewHandle(const ViewId& viewId) const;

    /// Put ViewHandle in map and return new ViewId for this handle
    /// @param handle handle to put
    /// @param[out] viewId returned viewId
    /// @return true if ok
    bool AddNewView(const ViewHandle handle, ViewId* viewId);

    /// Invalidate viewIdRemove it from map
    /// @param viewId requested view
    void RemoveView(const ViewId& viewId);

    /// Get elementHandle for given elementId in specific view
    /// @param viewId requested view
    /// @param elementId requested element
    /// @return elementHandle
    ElementHandle GetElementHandle(const ViewId& viewId, const ElementId& elementId) const;

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
    typedef std::map<std::string, ElementHandle> ElementsMap;
    typedef std::map<std::string, ElementsMap> ViewsElementsMap;
    typedef std::map<std::string, ViewHandle> ViewsMap;

    void RunClosureOnSessionThread(
        const base::Closure& task,
        base::WaitableEvent* done_event);

    scoped_ptr<InMemoryLog> session_log_;
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

    // Timeout (in ms) for asynchronous script execution.
    int async_script_timeout_;

    // Time (in ms) of how long to wait while searching for a single element.
    int implicit_wait_;

    // Last mouse position. Advanced APIs need this value.
    Point mouse_position_;

  // Chrome does not have an individual method for setting the prompt text
  // of an alert. Instead, when the WebDriver client wants to set the text,
  // we store it here and pass the text when the alert is accepted or
  // dismissed. This text should only be used if |has_alert_prompt_text_|
  // is true, so that the default prompt text is not overridden.
  std::string alert_prompt_text_;
  //bool has_alert_prompt_text_;

    // Temporary directory containing session data.
    ScopedTempDir temp_dir_;
    Capabilities capabilities_;

    scoped_ptr<ViewRunner> view_runner_;

    DISALLOW_COPY_AND_ASSIGN(Session);
};

}  // namespace webdriver

#endif  // WEBDRIVER_WEBDRIVER_SESSION_H_
