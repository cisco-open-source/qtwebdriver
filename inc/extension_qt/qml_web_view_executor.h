#ifndef WEBDRIVER_QML_WEB_VIEW_EXECUTOR_H
#define WEBDRIVER_QML_WEB_VIEW_EXECUTOR_H

#include <string>
#include <vector>
#include <map>

#include "webdriver_view_executor.h"
#include "webdriver_error.h"
#include "extension_qt/qdeclarativewebview.h"

#include <QtCore/QDebug>

namespace webdriver {

class QmlWebViewCmdExecutorCreator : public ViewCmdExecutorCreator  {
public:
    static const ViewType QML_WEB_VIEW_TYPE; 

    QmlWebViewCmdExecutorCreator();
    virtual ~QmlWebViewCmdExecutorCreator();

    virtual ViewCmdExecutor* CreateExecutor(Session* session, ViewId viewId) const;
    virtual bool CanHandleView(Session* session, ViewId viewId, ViewType* viewType = NULL) const;
private:

    DISALLOW_COPY_AND_ASSIGN(QmlWebViewCmdExecutorCreator);
};      

#define NOT_SUPPORTED_IMPL      {*error = new Error(kCommandNotSupported, "Current view doesnt support this command.");}

class QmlWebViewCmdExecutor : public ViewCmdExecutor {
public:
    explicit QmlWebViewCmdExecutor(Session* session, ViewId viewId);
    virtual ~QmlWebViewCmdExecutor();

    virtual void CanHandleUrl(const std::string& url, bool* can, Error **error);
    virtual void GetTitle(std::string* title, Error **error);
    virtual void GetWindowName(std::string* name, Error ** error);
    virtual void GetBounds(Rect *bounds, Error **error);
    virtual void SetBounds(const Rect& bounds, Error** error) NOT_SUPPORTED_IMPL;
    virtual void Maximize(Error** error) NOT_SUPPORTED_IMPL;
    virtual void GetScreenShot(std::string* png, Error** error);
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
    virtual void TouchClick(const ElementId& element, Error **error);
    virtual void TouchDoubleClick(const ElementId& element, Error **error);
    virtual void TouchDown(const int &x, const int &y, Error **error);
    virtual void TouchUp(const int &x, const int &y, Error **error);
    virtual void TouchMove(const int &x, const int &y, Error **error);
    virtual void TouchLongClick(const ElementId& element, Error **error);
    virtual void TouchScroll(const int &xoffset, const int &yoffset, Error **error);
    virtual void TouchScroll(const ElementId &element, const int &xoffset, const int &yoffset, Error **error);
    virtual void TouchFlick(const int &xSpeed, const int &ySpeed, Error **error);
    virtual void TouchFlick(const ElementId &element, const int &xoffset, const int &yoffset, const int &speed, Error **error);
    virtual void GetBrowserLog(base::ListValue** browserLog, Error **error);
    virtual void GetPlayerState(const ElementId& element, PlayerState*, Error**);
    virtual void SetPlayerState(const ElementId& element, PlayerState, Error**);
    virtual void GetPlayerVolume(const ElementId& element, double*, Error**);
    virtual void SetPlayerVolume(const ElementId& element, double, Error**);
    virtual void GetPlayingPosition(const ElementId& element, double*, Error**);
    virtual void SetPlayingPosition(const ElementId& element, double, Error**);
    virtual void SetMute(const ElementId& element, bool, Error**);
    virtual void GetMute(const ElementId& element, bool*, Error**);
    virtual void VisualizerSource(std::string* source, Error** error);
    virtual void VisualizerShowPoint(Error** error);
    virtual void GetAlertMessage(std::string* text, Error** error) NOT_SUPPORTED_IMPL;
    virtual void SetAlertPromptText(const std::string& alert_prompt_text, Error** error) NOT_SUPPORTED_IMPL;
    virtual void AcceptOrDismissAlert(bool accept, Error** error) NOT_SUPPORTED_IMPL;
    virtual void SetOrientation(const std::string &orientation, Error **error) NOT_SUPPORTED_IMPL;
    virtual void GetOrientation(std::string *orientation, Error **error);

protected:
    QDeclarativeWebView* getView(const ViewId& viewId, Error** error);
    Rect ConvertQRectToRect(const QRect &rect);
    QRect ConvertRectToQRect(const Rect &rect);
    QPoint ConvertPointToQPoint(const Point &p);
    Qt::MouseButton ConvertMouseButtonToQtMouseButton(MouseButton button);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    QTouchDevice touchDevice;
#endif

private:
    DISALLOW_COPY_AND_ASSIGN(QmlWebViewCmdExecutor);
};

}  // namespace webdriver

#endif  // WEBDRIVER_QML_WEB_VIEW_EXECUTOR_H
