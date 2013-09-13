#include "extension_qt/qml_web_view_executor.h"

#include "webdriver_logging.h"
#include "webdriver_session.h"
#include "q_key_converter.h"
#include "extension_qt/declarative_item_view_handle.h"
#include "extension_qt/event_dispatcher.h"
#include "extension_qt/wd_event_dispatcher.h"
#include "web_view_util.h"

#include <QtCore/QDebug>

namespace webdriver {

const ViewType QmlWebViewCmdExecutorCreator::QML_WEB_VIEW_TYPE = 0x23f1;

static QDeclarativeWebView* _getDeclarativeWebView(Session* session, const ViewId& viewId) {
    ViewHandle* viewHandle =  session->GetViewHandle(viewId);
    if (NULL == viewHandle) 
        return NULL;

    QDeclarativeItemViewHandle* qViewHandle = dynamic_cast<QDeclarativeItemViewHandle*>(viewHandle);
    if (NULL == qViewHandle)
        return NULL;

    return qobject_cast<QDeclarativeWebView*>(qViewHandle->get());
}

QmlWebViewCmdExecutorCreator::QmlWebViewCmdExecutorCreator()
    : ViewCmdExecutorCreator() { }

QmlWebViewCmdExecutorCreator::~QmlWebViewCmdExecutorCreator() {}

ViewCmdExecutor* QmlWebViewCmdExecutorCreator::CreateExecutor(Session* session, ViewId viewId) const {
    QDeclarativeWebView* pWebView = _getDeclarativeWebView(session, viewId);
    if (NULL != pWebView) {
        session->logger().Log(kFineLogLevel, "Qml Web executor for view("+viewId.id()+")");
        return new QmlWebViewCmdExecutor(session, viewId);
    }

    return NULL;
}

bool QmlWebViewCmdExecutorCreator::CanHandleView(Session* session, ViewId viewId, ViewType* viewType) const {
    QDeclarativeWebView* pWebView = _getDeclarativeWebView(session, viewId);

    if (NULL != pWebView) {
        if (NULL != viewType) *viewType = QML_WEB_VIEW_TYPE;
        return true;
    }

    return false;
}    

QmlWebViewCmdExecutor::QmlWebViewCmdExecutor(Session* session, ViewId viewId)
    : ViewCmdExecutor(session, viewId) {

}

QmlWebViewCmdExecutor::~QmlWebViewCmdExecutor() {

};

QDeclarativeWebView* QmlWebViewCmdExecutor::getView(const ViewId& viewId, Error** error) {
    QDeclarativeWebView* pView = _getDeclarativeWebView(session_, view_id_);

    if (NULL == pView) {
        session_->logger().Log(kWarningLogLevel, "checkView - no such view("+viewId.id()+")");
        *error = new Error(kNoSuchWindow);
        return NULL;
    }

    return pView;
}

void QmlWebViewCmdExecutor::CanHandleUrl(const std::string& url, bool* can, Error **error) {
    QDeclarativeWebView* pWebView = getView(view_id_, error);
    if (NULL == pWebView)
        return;
    
    *can = QWebViewUtil::isUrlSupported(pWebView->page(), url, error);
}

void QmlWebViewCmdExecutor::GetTitle(std::string* title, Error **error) {
    // TODO:
}

void QmlWebViewCmdExecutor::GetWindowName(std::string* name, Error ** error) {
    // TODO:
}

void QmlWebViewCmdExecutor::GetBounds(Rect *bounds, Error **error) {
    // TODO:
}
    
void QmlWebViewCmdExecutor::GetScreenShot(std::string* png, Error** error) {
    // TODO:
}

void QmlWebViewCmdExecutor::GoForward(Error** error) {
    // TODO:
}

void QmlWebViewCmdExecutor::GoBack(Error** error) {
    // TODO:
}

void QmlWebViewCmdExecutor::Reload(Error** error) {
    // TODO:
}

void QmlWebViewCmdExecutor::GetSource(std::string* source, Error** error) {
    // TODO:
}

void QmlWebViewCmdExecutor::SendKeys(const string16& keys, Error** error) {
    // TODO:
}

void QmlWebViewCmdExecutor::SendKeys(const ElementId& element, const string16& keys, Error** error) {
    // TODO:
}

void QmlWebViewCmdExecutor::MouseDoubleClick(Error** error) {
    // TODO:
}

void QmlWebViewCmdExecutor::MouseButtonUp(Error** error) {
    // TODO:
}

void QmlWebViewCmdExecutor::MouseButtonDown(Error** error) {
    // TODO:
}

void QmlWebViewCmdExecutor::MouseClick(MouseButton button, Error** error) {
    // TODO:
}

void QmlWebViewCmdExecutor::MouseMove(const int x_offset, const int y_offset, Error** error) {
    // TODO:
}

void QmlWebViewCmdExecutor::MouseMove(const ElementId& element, int x_offset, const int y_offset, Error** error) {
    // TODO:
}

void QmlWebViewCmdExecutor::MouseMove(const ElementId& element, Error** error) {
    // TODO:
}

void QmlWebViewCmdExecutor::ClickElement(const ElementId& element, Error** error) {
    // TODO:
}

void QmlWebViewCmdExecutor::GetAttribute(const ElementId& element, const std::string& key, base::Value** value, Error** error) {
    // TODO:
}

void QmlWebViewCmdExecutor::ClearElement(const ElementId& element, Error** error) {
    // TODO:
}

void QmlWebViewCmdExecutor::IsElementDisplayed(const ElementId& element, bool ignore_opacity, bool* is_displayed, Error** error) {
    // TODO:
}

void QmlWebViewCmdExecutor::IsElementEnabled(const ElementId& element, bool* is_enabled, Error** error) {
    // TODO:
}

void QmlWebViewCmdExecutor::ElementEquals(const ElementId& element1, const ElementId& element2, bool* is_equal, Error** error) {
    // TODO:
}

void QmlWebViewCmdExecutor::GetElementLocation(const ElementId& element, Point* location, Error** error) {
    // TODO:
}

void QmlWebViewCmdExecutor::GetElementLocationInView(const ElementId& element, Point* location, Error** error) {
    // TODO:
}

void QmlWebViewCmdExecutor::GetElementTagName(const ElementId& element, std::string* tag_name, Error** error) {
    // TODO:
}

void QmlWebViewCmdExecutor::IsOptionElementSelected(const ElementId& element, bool* is_selected, Error** error) {
    // TODO:
}

void QmlWebViewCmdExecutor::SetOptionElementSelected(const ElementId& element, bool selected, Error** error) {
    // TODO:
}

void QmlWebViewCmdExecutor::GetElementSize(const ElementId& element, Size* size, Error** error) {
    // TODO:
}

void QmlWebViewCmdExecutor::ElementSubmit(const ElementId& element, Error** error) {
    // TODO:
}

void QmlWebViewCmdExecutor::GetElementText(const ElementId& element, std::string* element_text, Error** error) {
    // TODO:
}

void QmlWebViewCmdExecutor::GetElementCssProperty(const ElementId& element, const std::string& property, base::Value** value, Error** error) {
    // TODO:
}

void QmlWebViewCmdExecutor::FindElement(const ElementId& root_element, const std::string& locator, const std::string& query, ElementId* element, Error** error) {
    // TODO:
}

void QmlWebViewCmdExecutor::FindElements(const ElementId& root_element, const std::string& locator, const std::string& query, std::vector<ElementId>* elements, Error** error) {
    // TODO:
}

void QmlWebViewCmdExecutor::ActiveElement(ElementId* element, Error** error) {
    // TODO:
}

void QmlWebViewCmdExecutor::SwitchTo(Error** error) {
    // TODO:
}

void QmlWebViewCmdExecutor::SwitchToFrameWithNameOrId(const std::string& name_or_id, Error** error) {
    // TODO:
}

void QmlWebViewCmdExecutor::SwitchToFrameWithIndex(int index, Error** error) {
    // TODO:
}

void QmlWebViewCmdExecutor::SwitchToFrameWithElement(const ElementId& element, Error** error) {
    // TODO:
}

void QmlWebViewCmdExecutor::SwitchToTopFrame(Error** error) {
    // TODO:
}

void QmlWebViewCmdExecutor::SwitchToTopFrameIfCurrentFrameInvalid(Error** error) {
    // TODO:
}

void QmlWebViewCmdExecutor::NavigateToURL(const std::string& url, bool sync, Error** error) {
    // TODO:
}

void QmlWebViewCmdExecutor::GetURL(std::string* url, Error** error) {
    // TODO:
}

void QmlWebViewCmdExecutor::ExecuteScript(const std::string& script, const base::ListValue* const args, base::Value** value, Error** error) {
    // TODO:
}

void QmlWebViewCmdExecutor::ExecuteAsyncScript(const std::string& script, const base::ListValue* const args, base::Value** value, Error** error) {
    // TODO:
}

void QmlWebViewCmdExecutor::GetAppCacheStatus(int* status, Error** error) {
    // TODO:
}

void QmlWebViewCmdExecutor::GetCookies(const std::string& url, base::ListValue** cookies, Error** error) {
    // TODO:
}

void QmlWebViewCmdExecutor::SetCookie(const std::string& url, base::DictionaryValue* cookie_dict, Error** error) {
    // TODO:
}

void QmlWebViewCmdExecutor::DeleteCookie(const std::string& url, const std::string& cookie_name, Error** error) {
    // TODO:
}

void QmlWebViewCmdExecutor::GetStorageKeys(StorageType type, base::ListValue** keys, Error** error) {
    // TODO:
}

void QmlWebViewCmdExecutor::SetStorageItem(StorageType type, const std::string& key, const std::string& value, Error** error) {
    // TODO:
}

void QmlWebViewCmdExecutor::ClearStorage(StorageType type, Error** error) {
    // TODO:
}

void QmlWebViewCmdExecutor::GetStorageItem(StorageType type, const std::string& key, std::string* value, Error** error) {
    // TODO:
}

void QmlWebViewCmdExecutor::RemoveStorageItem(StorageType type, const std::string& key, std::string* value, Error** error) {
    // TODO:
}

void QmlWebViewCmdExecutor::GetStorageSize(StorageType type, int* size, Error** error) {
    // TODO:
}

void QmlWebViewCmdExecutor::TouchClick(const ElementId& element, Error **error) {
    // TODO:
}

void QmlWebViewCmdExecutor::TouchDoubleClick(const ElementId& element, Error **error) {
    // TODO:
}

void QmlWebViewCmdExecutor::TouchDown(const int &x, const int &y, Error **error) {
    // TODO:
}

void QmlWebViewCmdExecutor::TouchUp(const int &x, const int &y, Error **error) {
    // TODO:
}

void QmlWebViewCmdExecutor::TouchMove(const int &x, const int &y, Error **error) {
    // TODO:
}

void QmlWebViewCmdExecutor::TouchLongClick(const ElementId& element, Error **error) {
    // TODO:
}

void QmlWebViewCmdExecutor::TouchScroll(const int &xoffset, const int &yoffset, Error **error) {
    // TODO:
}

void QmlWebViewCmdExecutor::TouchScroll(const ElementId &element, const int &xoffset, const int &yoffset, Error **error) {
    // TODO:
}

void QmlWebViewCmdExecutor::TouchFlick(const int &xSpeed, const int &ySpeed, Error **error) {
    // TODO:
}

void QmlWebViewCmdExecutor::TouchFlick(const ElementId &element, const int &xoffset, const int &yoffset, const int &speed, Error **error) {
    // TODO:
}

void QmlWebViewCmdExecutor::GetBrowserLog(base::ListValue** browserLog, Error **error) {
    // TODO:
}

void QmlWebViewCmdExecutor::GetPlayerState(const ElementId& element, PlayerState*, Error**) {
    // TODO:
}

void QmlWebViewCmdExecutor::SetPlayerState(const ElementId& element, PlayerState, Error**) {
    // TODO:
}

void QmlWebViewCmdExecutor::GetPlayerVolume(const ElementId& element, double*, Error**) {
    // TODO:
}

void QmlWebViewCmdExecutor::SetPlayerVolume(const ElementId& element, double, Error**) {
    // TODO:
}

void QmlWebViewCmdExecutor::GetPlayingPosition(const ElementId& element, double*, Error**) {
    // TODO:
}

void QmlWebViewCmdExecutor::SetPlayingPosition(const ElementId& element, double, Error**) {
    // TODO:
}

void QmlWebViewCmdExecutor::SetMute(const ElementId& element, bool, Error**) {
    // TODO:
}

void QmlWebViewCmdExecutor::GetMute(const ElementId& element, bool*, Error**) {
    // TODO:
}

void QmlWebViewCmdExecutor::VisualizerSource(std::string* source, Error** error) {
    // TODO:
}

void QmlWebViewCmdExecutor::VisualizerShowPoint(Error** error) {
    // TODO:
}

void QmlWebViewCmdExecutor::GetOrientation(std::string *orientation, Error **error) {
    // TODO:
}

} // namespace webdriver     
