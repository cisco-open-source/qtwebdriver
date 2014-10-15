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

#ifndef WEBDRIVER_GRAPHICS_WEB_VIEW_EXECUTOR_H
#define WEBDRIVER_GRAPHICS_WEB_VIEW_EXECUTOR_H

#include <string>
#include <vector>
#include <map>

#include "base/memory/scoped_ptr.h"
#include "webdriver_view_executor.h"
#include "webdriver_error.h"

#include <QtCore/QDebug>

class QGraphicsWebView;

namespace webdriver {

class QWebkitProxy;    

class GraphicsWebViewCmdExecutorCreator : public ViewCmdExecutorCreator  {
public:
    static const ViewType GRAPHICS_WEB_VIEW_TYPE; 

    GraphicsWebViewCmdExecutorCreator();
    virtual ~GraphicsWebViewCmdExecutorCreator();

    virtual ViewCmdExecutor* CreateExecutor(Session* session, ViewId viewId) const;
    virtual bool CanHandleView(Session* session, ViewId viewId, ViewType* viewType = NULL) const;
    virtual std::string GetViewTypeName() const;

private:

    DISALLOW_COPY_AND_ASSIGN(GraphicsWebViewCmdExecutorCreator);
};      

#define NOT_SUPPORTED_IMPL      {*error = new Error(kCommandNotSupported, "Current view doesnt support this command.");}

class GraphicsWebViewCmdExecutor : public ViewCmdExecutor {
public:
    explicit GraphicsWebViewCmdExecutor(Session* session, ViewId viewId);
    virtual ~GraphicsWebViewCmdExecutor();

    virtual void CanHandleUrl(const std::string& url, bool* can, Error **error);
    virtual void GetTitle(std::string* title, Error **error);
    virtual void GetWindowName(std::string* name, Error ** error);
    virtual void GetBounds(Rect *bounds, Error **error);
    virtual void SetBounds(const Rect& bounds, Error** error) NOT_SUPPORTED_IMPL;
    virtual void Maximize(Error** error) NOT_SUPPORTED_IMPL;
    virtual void GetScreenShot(std::string* png, Error** error);
    virtual void GetElementScreenShot(const ElementId& element, std::string* png, Error** error);
    virtual void GoForward(Error** error);
    virtual void GoBack(Error** error);
    virtual void Reload(Error** error);
    virtual void GetSource(std::string* source, Error** error);
    virtual void SendKeys(const string16& keys, Error** error);
    virtual void SendKeys(const ElementId& element, const string16& keys, Error** error);
    virtual void Close(Error** error) NOT_SUPPORTED_IMPL;
    virtual void MouseDoubleClick(Error** error);
    virtual void MouseButtonUp(Error** error);
    virtual void MouseButtonDown(Error** error);
    virtual void MouseClick(MouseButton button, Error** error);
    virtual void MouseWheel(const int delta, Error **error);
    virtual void MouseMove(const int x_offset, const int y_offset, Error** error);
    virtual void MouseMove(const ElementId& element, int x_offset, const int y_offset, Error** error);
    virtual void MouseMove(const ElementId& element, Error** error);
    virtual void ClickElement(const ElementId& element, Error** error);
    virtual void GetAttribute(const ElementId& element, const std::string& key, base::Value** value, Error** error);
    virtual void ClearElement(const ElementId& element, Error** error);
    virtual void IsElementDisplayed(const ElementId& element, bool ignore_opacity, bool* is_displayed, Error** error);
    virtual void IsElementEnabled(const ElementId& element, bool* is_enabled, Error** error);
    virtual void ElementEquals(const ElementId& element1, const ElementId& element2, bool* is_equal, Error** error);
    virtual void GetElementLocation(const ElementId& element, Point* location, Error** error);
    virtual void GetElementLocationInView(const ElementId& element, Point* location, Error** error);
    virtual void GetElementTagName(const ElementId& element, std::string* tag_name, Error** error);
    virtual void IsOptionElementSelected(const ElementId& element, bool* is_selected, Error** error);
    virtual void SetOptionElementSelected(const ElementId& element, bool selected, Error** error);
    virtual void GetElementSize(const ElementId& element, Size* size, Error** error);
    virtual void ElementSubmit(const ElementId& element, Error** error);
    virtual void GetElementText(const ElementId& element, std::string* element_text, Error** error);
    virtual void GetElementCssProperty(const ElementId& element, const std::string& property, base::Value** value, Error** error);
    virtual void FindElement(const ElementId& root_element, const std::string& locator, const std::string& query, ElementId* element, Error** error);
    virtual void FindElements(const ElementId& root_element, const std::string& locator, const std::string& query, std::vector<ElementId>* elements, Error** error);
    virtual void ActiveElement(ElementId* element, Error** error);
    virtual void SwitchTo(Error** error);
    virtual void SwitchToFrameWithNameOrId(const std::string& name_or_id, Error** error);
    virtual void SwitchToFrameWithIndex(int index, Error** error);
    virtual void SwitchToFrameWithElement(const ElementId& element, Error** error);
    virtual void SwitchToTopFrame(Error** error);
    virtual void SwitchToTopFrameIfCurrentFrameInvalid(Error** error);
    virtual void NavigateToURL(const std::string& url, bool sync, Error** error);
    virtual void GetURL(std::string* url, Error** error);
    virtual void ExecuteScript(const std::string& script, const base::ListValue* const args, base::Value** value, Error** error);
    virtual void ExecuteAsyncScript(const std::string& script, const base::ListValue* const args, base::Value** value, Error** error);
    virtual void GetAppCacheStatus(int* status, Error** error);
    virtual void GetCookies(const std::string& url, base::ListValue** cookies, Error** error);
    virtual void SetCookie(const std::string& url, base::DictionaryValue* cookie_dict, Error** error);
    virtual void DeleteCookie(const std::string& url, const std::string& cookie_name, Error** error);
    virtual void GetStorageKeys(StorageType type, base::ListValue** keys, Error** error);
    virtual void SetStorageItem(StorageType type, const std::string& key, const std::string& value, Error** error);
    virtual void ClearStorage(StorageType type, Error** error);
    virtual void GetStorageItem(StorageType type, const std::string& key, std::string* value, Error** error);
    virtual void RemoveStorageItem(StorageType type, const std::string& key, std::string* value, Error** error);
    virtual void GetStorageSize(StorageType type, int* size, Error** error);
    virtual void GetGeoLocation(base::DictionaryValue** geolocation, Error** error) NOT_SUPPORTED_IMPL;
    virtual void SetGeoLocation(const base::DictionaryValue* geolocation, Error** error) NOT_SUPPORTED_IMPL;
    virtual void TouchClick(const ElementId& element, Error **error) NOT_SUPPORTED_IMPL;
    virtual void TouchDoubleClick(const ElementId& element, Error **error) NOT_SUPPORTED_IMPL;
    virtual void TouchDown(const int &x, const int &y, Error **error) NOT_SUPPORTED_IMPL;
    virtual void TouchUp(const int &x, const int &y, Error **error) NOT_SUPPORTED_IMPL;
    virtual void TouchMove(const int &x, const int &y, Error **error) NOT_SUPPORTED_IMPL;
    virtual void TouchLongClick(const ElementId& element, Error **error) NOT_SUPPORTED_IMPL;
    virtual void TouchScroll(const int &xoffset, const int &yoffset, Error **error) NOT_SUPPORTED_IMPL;
    virtual void TouchScroll(const ElementId &element, const int &xoffset, const int &yoffset, Error **error) NOT_SUPPORTED_IMPL;
    virtual void TouchFlick(const int &xSpeed, const int &ySpeed, Error **error) NOT_SUPPORTED_IMPL;
    virtual void TouchFlick(const ElementId &element, const int &xoffset, const int &yoffset, const int &speed, Error **error) NOT_SUPPORTED_IMPL;
    virtual void GetBrowserLog(base::ListValue** browserLog, Error **error);
    virtual void GetPlayerState(const ElementId& element, PlayerState*, Error**);
    virtual void SetPlayerState(const ElementId& element, PlayerState, Error**);
    virtual void GetPlayerVolume(const ElementId& element, double*, Error**);
    virtual void SetPlayerVolume(const ElementId& element, double, Error**);
    virtual void GetPlayingPosition(const ElementId& element, double*, Error**);
    virtual void SetPlayingPosition(const ElementId& element, double, Error**);
    virtual void SetMute(const ElementId& element, bool, Error**);
    virtual void GetMute(const ElementId& element, bool*, Error**);
    virtual void SetPlaybackSpeed(const ElementId& element, double, Error**);
    virtual void GetPlaybackSpeed(const ElementId& element, double*, Error**);
    virtual void VisualizerSource(std::string* source, Error** error);
    virtual void VisualizerShowPoint(Error** error);
    virtual void GetAlertMessage(std::string* text, Error** error) NOT_SUPPORTED_IMPL;
    virtual void SetAlertPromptText(const std::string& alert_prompt_text, Error** error) NOT_SUPPORTED_IMPL;
    virtual void AcceptOrDismissAlert(bool accept, Error** error) NOT_SUPPORTED_IMPL;
    virtual void SetOrientation(const std::string &orientation, Error **error) NOT_SUPPORTED_IMPL;
    virtual void GetOrientation(std::string *orientation, Error **error);
    virtual void SetOnline(bool, Error** error);
    virtual void IsOnline(bool*, Error** error);
    virtual void TouchPinchZoom(const ElementId &element, const double &scale, Error **error) NOT_SUPPORTED_IMPL;
    virtual void TouchPinchRotate(const ElementId &element, const int &angle, Error **error) NOT_SUPPORTED_IMPL;

protected:
    QGraphicsWebView* getView(const ViewId& viewId, Error** error);
    void saveScreenshot(QImage& image, std::string* png, Error** error);

private:
    scoped_ptr<QWebkitProxy> webkitProxy_;
    QGraphicsWebView* view_;

    DISALLOW_COPY_AND_ASSIGN(GraphicsWebViewCmdExecutor);
};

}  // namespace webdriver

#endif  // WEBDRIVER_GRAPHICS_WEB_VIEW_EXECUTOR_H
