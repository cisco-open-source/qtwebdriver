/****************************************************************************
**
** Copyright © 1992-2014 Cisco and/or its affiliates. All rights reserved.
** All rights reserved.
** 
** $CISCO_BEGIN_LICENSE:LGPL$
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** $CISCO_END_LICENSE$
**
****************************************************************************/

/*! \page page_views Views management and actions

<h1>View executor</h1>

For separating session and view commands executor concept is used.
webdriver::ViewCmdExecutor is pure virtual class that represents all 
actions that can be done on single view of specific type in one session.
So for each view type there should be separated view executor implemented by customer.
Exexcutor can be created only by webdriver::ViewCmdExecutorCreator which knows
if it can create executor for passed view.
All executor creators are registered in webdriver::ViewCmdExecutorFactory.
This singleton is an entry point to get executor for view.
\code
Error* error = NULL;
ExecutorPtr executor(ViewCmdExecutorFactory::GetInstance()->CreateExecutor(session, viewId));

if (NULL == executor.get()) {
    // handle error
}

session->RunSessionTask(base::Bind(
            &ViewCmdExecutor::GetWindowName,
            base::Unretained(executor.get()),
            title,
            &error));
\endcode

\remark ViewCmdExecutorFactory return only first found executor that can operate wit view, so if two executors can handle same view type - registration order is important.

Example of registration:
\code
webdriver::ViewCmdExecutorFactory::GetInstance()->AddViewCmdExecutorCreator(new webdriver::QWebViewCmdExecutorCreator());
webdriver::ViewCmdExecutorFactory::GetInstance()->AddViewCmdExecutorCreator(new webdriver::QWidgetViewCmdExecutorCreator());
\endcode

*/
#ifndef WEBDRIVER_VIEW_EXECUTOR_H
#define WEBDRIVER_VIEW_EXECUTOR_H

#include <string>
#include <vector>
#include <map>

#include "base/basictypes.h"
#include "base/string16.h"
#include "webdriver_view_id.h"
#include "webdriver_element_id.h"
#include "webdriver_basic_types.h"

namespace base {
class Value;    
}

namespace webdriver {

class Session;
class Error;
class Rect;
class Point;
class Size;

/// base class for custom view's executors
class ViewCmdExecutor{
public:
    explicit ViewCmdExecutor(Session* session, ViewId viewId);
    virtual ~ViewCmdExecutor();

    virtual void CanHandleUrl(const std::string& url, bool* can, Error **error) = 0;
    virtual void GetTitle(std::string* title, Error **error) = 0;
    virtual void GetWindowName(std::string* name, Error ** error) = 0;
    virtual void GetBounds(Rect *bounds, Error **error) = 0;
    virtual void SetBounds(const Rect& bounds, Error** error) = 0;
    virtual void Maximize(Error** error) = 0;
    virtual void GoForward(Error** error) = 0;
    virtual void GoBack(Error** error) = 0;
    virtual void Reload(Error** error) = 0;
    virtual void GetScreenShot(std::string* png, Error** error) = 0;
    virtual void GetElementScreenShot(const ElementId& element, std::string* png, Error** error) = 0;
    virtual void GetSource(std::string* source, Error** error) = 0;
    virtual void SendKeys(const string16& keys, Error** error) = 0;
    virtual void SendKeys(const ElementId& element, const string16& keys, Error** error) = 0;
    virtual void MouseDoubleClick(Error** error) = 0;
    virtual void MouseButtonUp(Error** error) = 0;
    virtual void MouseButtonDown(Error** error) = 0;
    virtual void MouseClick(MouseButton button, Error** error) = 0;
    virtual void MouseWheel(const int delta, Error** error) = 0;
    /// move mouse to position: cur_point + offset
    virtual void MouseMove(const int x_offset, const int y_offset, Error** error) = 0;
    /// Move the mouse by an offset of the specificed element
    virtual void MouseMove(const ElementId& element, int x_offset, const int y_offset, Error** error) = 0;
    /// the mouse will be moved to the center of the element
    virtual void MouseMove(const ElementId& element, Error** error) = 0;
    virtual void ClickElement(const ElementId& element, Error** error) = 0;
    virtual void GetAttribute(const ElementId& element, const std::string& key, base::Value** value, Error** error) = 0;
    virtual void ClearElement(const ElementId& element, Error** error) = 0;
    virtual void IsElementDisplayed(const ElementId& element, bool ignore_opacity, bool* is_displayed, Error** error) = 0;
    virtual void IsElementEnabled(const ElementId& element, bool* is_enabled, Error** error) = 0;
    virtual void ElementEquals(const ElementId& element1, const ElementId& element2, bool* is_equal, Error** error) = 0;
    virtual void GetElementLocation(const ElementId& element, Point* location, Error** error) = 0;
    virtual void GetElementLocationInView(const ElementId& element, Point* location, Error** error) = 0;
    virtual void GetElementTagName(const ElementId& element, std::string* tag_name, Error** error) = 0;
    virtual void IsOptionElementSelected(const ElementId& element, bool* is_selected, Error** error) = 0;
    virtual void SetOptionElementSelected(const ElementId& element, bool selected, Error** error) = 0;
    virtual void GetElementSize(const ElementId& element, Size* size, Error** error) = 0;
    virtual void ElementSubmit(const ElementId& element, Error** error) = 0;
    virtual void GetElementText(const ElementId& element, std::string* element_text, Error** error) = 0;
    virtual void GetElementCssProperty(const ElementId& element, const std::string& property, base::Value** value, Error** error) = 0;
    virtual void FindElement(const ElementId& root_element, const std::string& locator, const std::string& query, ElementId* element, Error** error) = 0;
    virtual void FindElements(const ElementId& root_element, const std::string& locator, const std::string& query, std::vector<ElementId>* elements, Error** error) = 0;
    virtual void ActiveElement(ElementId* element, Error** error) = 0;
    virtual void Close(Error** error) = 0;
    /// set view as current
    virtual void SwitchTo(Error** error) = 0;
    /// Switches the frame used by default. |name_or_id| is either the name or id
    /// of a frame element.
    virtual void SwitchToFrameWithNameOrId(const std::string& name_or_id, Error** error) = 0;
    /// Switches the frame used by default. |index| is the zero-based frame index.
    virtual void SwitchToFrameWithIndex(int index, Error** error) = 0;
    /// Switches to the frame identified by the given |element|. The element must
    /// be either an IFRAME or FRAME element.
    virtual void SwitchToFrameWithElement(const ElementId& element, Error** error) = 0;
    /// Switches the target frame to the topmost frame.
    virtual void SwitchToTopFrame(Error** error) = 0;
    virtual void SwitchToTopFrameIfCurrentFrameInvalid(Error** error) = 0;
    virtual void NavigateToURL(const std::string& url, bool sync, Error** error) = 0;
    virtual void GetURL(std::string* url, Error** error) = 0;
    virtual void ExecuteScript(const std::string& script, const base::ListValue* const args, base::Value** value, Error** error) = 0;
    virtual void ExecuteAsyncScript(const std::string& script, const base::ListValue* const args, base::Value** value, Error** error) = 0;
    virtual void GetAppCacheStatus(int* status, Error** error) = 0;
    virtual void GetAlertMessage(std::string* text, Error** error) = 0;
    virtual void SetAlertPromptText(const std::string& alert_prompt_text, Error** error) = 0;
    virtual void AcceptOrDismissAlert(bool accept, Error** error) = 0;
    virtual void GetCookies(const std::string& url, base::ListValue** cookies, Error** error) = 0;
    virtual void SetCookie(const std::string& url, base::DictionaryValue* cookie_dict, Error** error) = 0;
    virtual void DeleteCookie(const std::string& url, const std::string& cookie_name, Error** error) = 0;
    virtual void GetStorageKeys(StorageType type, base::ListValue** keys, Error** error) = 0;
    virtual void SetStorageItem(StorageType type, const std::string& key, const std::string& value, Error** error) = 0;
    virtual void ClearStorage(StorageType type, Error** error) = 0;
    virtual void GetStorageItem(StorageType type, const std::string& key, std::string* value, Error** error) = 0;
    virtual void RemoveStorageItem(StorageType type, const std::string& key, std::string* value, Error** error) = 0;
    virtual void GetStorageSize(StorageType type, int* size, Error** error) = 0;
    /// Gets the current geolocation.
    virtual void GetGeoLocation(base::DictionaryValue** geolocation, Error** error) = 0;
    /// Overrides the current geolocation.
    virtual void SetGeoLocation(const base::DictionaryValue* geolocation, Error** error) = 0;
    /// Touch commands emulation
    virtual void TouchClick(const ElementId& element, Error **error) = 0;
    virtual void TouchDoubleClick(const ElementId& element, Error **error) = 0;
    virtual void TouchDown(const int &x, const int &y, Error **error) = 0;
    virtual void TouchUp(const int &x, const int &y, Error **error) = 0;
    virtual void TouchMove(const int &x, const int &y, Error **error) = 0;
    virtual void TouchLongClick(const ElementId& element, Error **error) = 0;
    virtual void TouchScroll(const int &xoffset, const int &yoffset, Error **error) = 0;
    virtual void TouchScroll(const ElementId &element, const int &xoffset, const int &yoffset, Error **error) = 0;
    virtual void TouchFlick(const int &xSpeed, const int &ySpeed, Error **error) = 0;
    virtual void TouchFlick(const ElementId &element, const int &xoffset, const int &yoffset, const int &speed, Error **error) = 0;
    virtual void GetBrowserLog(base::ListValue** browserLog, Error **error) {}
    virtual void GetPlayerState(const ElementId& element, PlayerState *state, Error** error)  = 0;
    virtual void SetPlayerState(const ElementId& element, const PlayerState state, Error** error) = 0;
    virtual void GetPlayerVolume(const ElementId& element, double *level, Error** error) = 0;
    virtual void SetPlayerVolume(const ElementId& element, const double level, Error** error) = 0;
    virtual void GetPlayingPosition(const ElementId& element, double* reletivePos, Error** error) = 0;
    virtual void SetPlayingPosition(const ElementId& element, const double reletivePos, Error** error) = 0;
    virtual void SetMute(const ElementId& element, bool, Error**) = 0;
    virtual void GetMute(const ElementId& element, bool*, Error**) = 0;
    virtual void SetPlaybackSpeed(const ElementId& element, double, Error**) = 0;
    virtual void GetPlaybackSpeed(const ElementId& element, double*, Error**) = 0;
    virtual void SetOrientation(const std::string &orientation, Error **error) = 0;
    virtual void GetOrientation(std::string *orientation, Error **error) = 0;

    virtual void VisualizerSource(std::string* source, Error** error) = 0;
    virtual void VisualizerShowPoint(Error** error) = 0;

    virtual void IsOnline(bool *online,Error **error) = 0;
    virtual void SetOnline(bool online, Error **error) = 0;
	
	virtual void TouchPinchZoom(const ElementId &element, const double &scale, Error **error) = 0;
    virtual void TouchPinchRotate(const ElementId &element, const int &angle, Error **error) = 0;

protected:
    Session* session_;
    ViewId view_id_;

private:
    DISALLOW_COPY_AND_ASSIGN(ViewCmdExecutor);
};

/// base class for custom cmd executor creators
class ViewCmdExecutorCreator {
public:
    ViewCmdExecutorCreator();
    virtual ~ViewCmdExecutorCreator(){}

    /// custom method, creates executor for specified view
    /// @param session pointer to session
    /// @param viewId view to operate on
    /// @return new ViewCmdExecutor object, NULL - if cant handle
    virtual ViewCmdExecutor* CreateExecutor(Session* session, ViewId viewId) const = 0;

    /// check if view suppported
    /// @param session pointer to session
    /// @param viewId view to operate on
    /// @param[out] viewType returned type of view
    /// @return true - if there is executor for such view
    virtual bool CanHandleView(Session* session, ViewId viewId, ViewType* viewType = NULL) const = 0;

    /// @return supported view type for current executor
    virtual std::string GetViewTypeName() const = 0;

private:

    DISALLOW_COPY_AND_ASSIGN(ViewCmdExecutorCreator);
};

/// This class used for managing commands related to view
class ViewCmdExecutorFactory
{
public:
    /// Returns the singleton instance.
    static ViewCmdExecutorFactory* GetInstance();

    /// creates executor for specified view
    /// @param session pointer to session
    /// @param viewId view to operate on
    /// @return new executor, NULL - if cant create.
    ViewCmdExecutor* CreateExecutor(Session* session, ViewId viewId) const;

    template <class C>
    C* CreateExecutor(Session* session, ViewId viewId) const {
        return dynamic_cast<C>(CreateExecutor(session, viewId));
    }

    /// creates executor for current view in session
    /// @param session pointer to session
    /// @return new ViewCmdExecutor object, NULL - if cant handle
    ViewCmdExecutor* CreateExecutor(Session* session) const;

    /// check if view suppported
    /// @param session pointer to session
    /// @param viewId view to operate on
    /// @param[out] viewType returned type of view
    /// @return true - if there is executor for such view
    bool CanHandleView(Session* session, ViewId viewId, ViewType* viewType) const;

    /// add new view's eexcutor creator
    /// @param creator pointer to custom creator. No need to delete object
    void AddViewCmdExecutorCreator(ViewCmdExecutorCreator* creator);

    typedef std::vector<std::string> SupportedViewTypesList;
    /// @return pointer to vector supported view types
    const SupportedViewTypesList& getSupportedViewTypesList() const;

private:
    typedef ViewCmdExecutorCreator* ViewCmdExecutorCreatorPtr;
    typedef std::vector<ViewCmdExecutorCreatorPtr> CreatorsList;
    CreatorsList creators_;
    SupportedViewTypesList typeslist_;

    ViewCmdExecutorFactory();
    ~ViewCmdExecutorFactory(){}
    static ViewCmdExecutorFactory* instance;
};

}  // namespace webdriver

#endif  // WEBDRIVER_VIEW_EXECUTOR_H
