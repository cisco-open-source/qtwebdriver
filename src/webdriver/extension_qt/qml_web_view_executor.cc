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

#include "extension_qt/qml_web_view_executor.h"

#include "base/stringprintf.h"

#include "webdriver_logging.h"
#include "webdriver_session.h"
#include "q_key_converter.h"
#include "extension_qt/declarative_item_view_handle.h"
#include "extension_qt/event_dispatcher.h"
#include "extension_qt/wd_event_dispatcher.h"
#include "common_util.h"
#include "web_view_util.h"
#include "qwebkit_proxy.h"

#include <QtCore/QDebug>
#include <QtGui/QStyleOptionGraphicsItem>
#include <QtGui/QApplication>

#define _USE_MATH_DEFINES
#include <math.h>

namespace webdriver {

#define CHECK_VIEW_EXISTANCE    \
    if ((NULL == view_) || (NULL == container_)) { \
        session_->logger().Log(kWarningLogLevel, "checkView - no such qml web view("+view_id_.id()+")"); \
        *error = new Error(kNoSuchWindow); \
        return; \
    } \
    webkitProxy_.reset(new QWebkitProxy(session_, (view_)?view_->page():NULL));

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

static QDeclarativeView* _getDeclarativeView(Session* session, const ViewId& viewId) {
    ViewHandle* viewHandle =  session->GetViewHandle(viewId);
    if (NULL == viewHandle) 
        return NULL;

    QDeclarativeItemViewHandle* qViewHandle = dynamic_cast<QDeclarativeItemViewHandle*>(viewHandle);
    if (NULL == qViewHandle)
        return NULL;

    return qobject_cast<QDeclarativeView*>(qViewHandle->getContainer());
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

std::string QmlWebViewCmdExecutorCreator::GetViewTypeName() const {
    return "qmlweb";
}

QmlWebViewCmdExecutor::QmlWebViewCmdExecutor(Session* session, ViewId viewId)
    : ViewCmdExecutor(session, viewId) {

    view_ = _getDeclarativeWebView(session, viewId);
    container_ = _getDeclarativeView(session, viewId);
    //webkitProxy_ = new QWebkitProxy(session, (view_)?view_->page():NULL);
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
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->GetTitle(title);
}

void QmlWebViewCmdExecutor::GetWindowName(std::string* name, Error ** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->GetWindowName(name);

    session_->logger().Log(kFineLogLevel, "GetWindowName - "+*name);
}

void QmlWebViewCmdExecutor::GetBounds(Rect *bounds, Error **error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->GetBounds(bounds);
}
    
void QmlWebViewCmdExecutor::GetScreenShot(std::string* png, Error** error) {
    CHECK_VIEW_EXISTANCE
    
    QImage image(view_->boundingRect().size().toSize(), QImage::Format_RGB32);
    image.fill(QColor(0, 0, 0).rgb());
    QPainter painter(&image);
    QStyleOptionGraphicsItem styleOption;
    qobject_cast<QGraphicsObject*>(view_)->paint(&painter, &styleOption);
    painter.end();

    const FilePath::CharType kPngFileName[] = FILE_PATH_LITERAL("./screen.png");
    FilePath path = session_->temp_dir().Append(kPngFileName);;

#if defined(OS_WIN)
    session_->logger().Log(kInfoLogLevel, "Save screenshot to - " + path.MaybeAsASCII());
#elif defined(OS_POSIX)
    session_->logger().Log(kInfoLogLevel, "Save screenshot to - " + path.value());
#endif

#if defined(OS_POSIX)
    if (!image.save(path.value().c_str())) 
#elif defined(OS_WIN)
    if (!image.save(QString::fromUtf16((ushort*)path.value().c_str())))
#endif // OS_WIN
    {
        *error = new Error(kUnknownError, "screenshot was not captured");
        return;
    }

    if (!file_util::ReadFileToString(path, png))
        *error = new Error(kUnknownError, "Could not read screenshot file");
}

void QmlWebViewCmdExecutor::GoForward(Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->GoForward();
}

void QmlWebViewCmdExecutor::GoBack(Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->GoBack();
}

void QmlWebViewCmdExecutor::Reload(Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->Reload();
}

void QmlWebViewCmdExecutor::GetSource(std::string* source, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->GetSource(source);
}

void QmlWebViewCmdExecutor::SendKeys(const string16& keys, Error** error) {
    CHECK_VIEW_EXISTANCE

    std::string err_msg;
    std::vector<QKeyEvent> key_events;
    int modifiers = session_->get_sticky_modifiers();

    if (!QKeyConverter::ConvertKeysToWebKeyEvents(keys,
                               session_->logger(),
                               false,
                               &modifiers,
                               &key_events,
                               &err_msg)) {
        session_->logger().Log(kSevereLogLevel, "SendKeys - cant convert keys:"+err_msg);
        *error = new Error(kUnknownError, "SendKeys - cant convert keys:"+err_msg);
        return;
    }

    session_->set_sticky_modifiers(modifiers);

    std::vector<QKeyEvent>::iterator it = key_events.begin();
    while (it != key_events.end()) {

        bool consumed = WDEventDispatcher::getInstance()->dispatch(&(*it));

        if (!consumed)
            qApp->sendEvent(view_->page(), &(*it));
        ++it;
    }
}

void QmlWebViewCmdExecutor::SendKeys(const ElementId& element, const string16& keys, Error** error) {
    CHECK_VIEW_EXISTANCE

    bool is_displayed = false;
    *error = webkitProxy_->IsElementDisplayed(element, true, &is_displayed);
    if (*error)
        return;
    
    if (!is_displayed) {
        *error = new Error(kElementNotVisible);
        return;
    }

    bool is_enabled = false;
    *error = webkitProxy_->IsElementEnabled(element, &is_enabled);
    if (*error)
        return;
    
    if (!is_enabled) {
        *error = new Error(kInvalidElementState);
        return;
    }

    *error = webkitProxy_->SetActiveElement(element);
    if (*error)
        return;

    std::string err_msg;
    std::vector<QKeyEvent> key_events;
    int modifiers = Qt::NoModifier;

    if (!QKeyConverter::ConvertKeysToWebKeyEvents(keys,
                               session_->logger(),
                               true,
                               &modifiers,
                               &key_events,
                               &err_msg)) {
        session_->logger().Log(kSevereLogLevel, "ElementSendKeys - cant convert keys:"+err_msg);
        *error = new Error(kUnknownError, "ElementSendKeys - cant convert keys:"+err_msg);
        return;
    }

    std::vector<QKeyEvent>::iterator it = key_events.begin();
    while (it != key_events.end()) {
        qApp->sendEvent(view_->page(), &(*it));
        ++it;
    }
}

void QmlWebViewCmdExecutor::MouseDoubleClick(Error** error) {
    CHECK_VIEW_EXISTANCE

    QPoint point = QCommonUtil::ConvertPointToQPoint(session_->get_mouse_position());
    
    QMouseEvent *dbEvent = new QMouseEvent(QEvent::MouseButtonDblClick, point, Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
    QMouseEvent *releaseEvent = new QMouseEvent(QEvent::MouseButtonRelease, point, Qt::LeftButton, Qt::NoButton, Qt::NoModifier);

    QApplication::postEvent(view_->page(), dbEvent);
    QApplication::postEvent(view_->page(), releaseEvent);
}

void QmlWebViewCmdExecutor::MouseButtonUp(Error** error) {
    CHECK_VIEW_EXISTANCE

    QPoint point = QCommonUtil::ConvertPointToQPoint(session_->get_mouse_position());
    
    QMouseEvent *releaseEvent = new QMouseEvent(QEvent::MouseButtonRelease, point, Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
    QApplication::postEvent(view_->page(), releaseEvent);
}

void QmlWebViewCmdExecutor::MouseButtonDown(Error** error) {
    CHECK_VIEW_EXISTANCE

    QPoint point = QCommonUtil::ConvertPointToQPoint(session_->get_mouse_position());
    
    QMouseEvent *pressEvent = new QMouseEvent(QEvent::MouseButtonPress, point, Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
    QApplication::sendEvent(view_->page(), pressEvent);
}

void QmlWebViewCmdExecutor::MouseClick(MouseButton button, Error** error) {
    CHECK_VIEW_EXISTANCE

    QPoint point = QCommonUtil::ConvertPointToQPoint(session_->get_mouse_position());
    Qt::MouseButton mouseButton = QCommonUtil::ConvertMouseButtonToQtMouseButton(button);

    QMouseEvent *pressEvent = new QMouseEvent(QEvent::MouseButtonPress, point, mouseButton, Qt::NoButton, Qt::NoModifier);
    QMouseEvent *releaseEvent = new QMouseEvent(QEvent::MouseButtonRelease, point, mouseButton, Qt::NoButton, Qt::NoModifier);

    QApplication::postEvent(view_->page(), pressEvent);
    QApplication::postEvent(view_->page(), releaseEvent);
}

void QmlWebViewCmdExecutor::MouseWheel(const int delta, Error **error) {
    CHECK_VIEW_EXISTANCE

    QPoint point = QCommonUtil::ConvertPointToQPoint(session_->get_mouse_position());

    QWheelEvent *wheelEvent = new QWheelEvent(point, delta, Qt::NoButton, Qt::NoModifier);

    QApplication::postEvent(view_->page(), wheelEvent);
}

void QmlWebViewCmdExecutor::MouseMove(const int x_offset, const int y_offset, Error** error) {
    CHECK_VIEW_EXISTANCE

    Point prev_pos = session_->get_mouse_position();
    prev_pos.Offset(x_offset, y_offset);

    QPoint point = QCommonUtil::ConvertPointToQPoint(prev_pos);
    
    QMouseEvent *moveEvent = new QMouseEvent(QEvent::MouseMove, point, Qt::NoButton, Qt::NoButton, Qt::NoModifier);
    QApplication::postEvent(view_->page(), moveEvent);

    session_->logger().Log(kFineLogLevel, base::StringPrintf("mouse move to: %d, %d",
                            (int)point.x(),
                            (int)point.y()));

    session_->set_mouse_position(prev_pos);
}

void QmlWebViewCmdExecutor::MouseMove(const ElementId& element, int x_offset, const int y_offset, Error** error) {
    CHECK_VIEW_EXISTANCE

    Point location;
    *error = webkitProxy_->GetElementLocationInView(element, &location);
    if (*error)
        return;

    location.Offset(x_offset, y_offset);

    QPoint point = QCommonUtil::ConvertPointToQPoint(location);
    
    QMouseEvent *moveEvent = new QMouseEvent(QEvent::MouseMove, point, Qt::NoButton, Qt::NoButton, Qt::NoModifier);
    QApplication::postEvent(view_->page(), moveEvent);

    session_->logger().Log(kFineLogLevel, base::StringPrintf("mouse move to: %d, %d",
                            (int)point.x(),
                            (int)point.y()));

    session_->set_mouse_position(location);
}

void QmlWebViewCmdExecutor::MouseMove(const ElementId& element, Error** error) {
    CHECK_VIEW_EXISTANCE

    Point location;

    // element is specified, calculate the coordinate.
    *error = webkitProxy_->GetElementLocationInView(element, &location);
    if (*error)
        return;
    
    // calculate the half of the element size and translate by it.
    Size size;
    *error = webkitProxy_->GetElementSize(element, &size);
    if (*error)
        return;

    location.Offset(size.width() / 2, size.height() / 2);

    QPoint point = QCommonUtil::ConvertPointToQPoint(location);
    
    QMouseEvent *moveEvent = new QMouseEvent(QEvent::MouseMove, point, Qt::NoButton, Qt::NoButton, Qt::NoModifier);
    QApplication::postEvent(view_->page(), moveEvent);

    session_->logger().Log(kFineLogLevel, base::StringPrintf("mouse move to: %d, %d",
                            (int)point.x(),
                            (int)point.y()));
    
    session_->set_mouse_position(location);
}

void QmlWebViewCmdExecutor::ClickElement(const ElementId& element, Error** error) {
    CHECK_VIEW_EXISTANCE

    std::string tag_name;
    *error = webkitProxy_->GetElementTagName(element, &tag_name);
    if (*error)
        return;

    if (tag_name == "option") {
        bool can_be_toggled;
        *error = webkitProxy_->IsElementCanBeToggled(element, &can_be_toggled);
        if (*error)
            return;

        if (can_be_toggled) {
            *error = webkitProxy_->ToggleOptionElement(element);
        } else {
            *error = webkitProxy_->SetOptionElementSelected(element, true);
        }
    } else {
        Point location;

        *error = webkitProxy_->GetClickableLocation(element, &location);
        if (!(*error)) {
            // consider truncation, round up value
            location.setX(ceil(location.x()));
            location.setY(ceil(location.y()));

            session_->logger().Log(kFineLogLevel,
                base::StringPrintf("ClickElement at pos (%f, %f).", location.x(), location.y()));
            session_->set_mouse_position(location);
            MouseClick(kLeftButton, error);
        }
    }
}

void QmlWebViewCmdExecutor::GetAttribute(const ElementId& element, const std::string& key, base::Value** value, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->GetAttribute(element, key, value);
}

void QmlWebViewCmdExecutor::ClearElement(const ElementId& element, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->ClearElement(element);
}

void QmlWebViewCmdExecutor::IsElementDisplayed(const ElementId& element, bool ignore_opacity, bool* is_displayed, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->IsElementDisplayed(element, ignore_opacity, is_displayed);
}

void QmlWebViewCmdExecutor::IsElementEnabled(const ElementId& element, bool* is_enabled, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->IsElementEnabled(element, is_enabled);
}

void QmlWebViewCmdExecutor::ElementEquals(const ElementId& element1, const ElementId& element2, bool* is_equal, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->ElementEquals(element1, element2, is_equal);
}

void QmlWebViewCmdExecutor::GetElementLocation(const ElementId& element, Point* location, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->GetElementLocation(element, location);
}

void QmlWebViewCmdExecutor::GetElementLocationInView(const ElementId& element, Point* location, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->GetElementLocationInView(element, location);
}

void QmlWebViewCmdExecutor::GetElementTagName(const ElementId& element, std::string* tag_name, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->GetElementTagName(element, tag_name);
}

void QmlWebViewCmdExecutor::IsOptionElementSelected(const ElementId& element, bool* is_selected, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->IsOptionElementSelected(element, is_selected);
}

void QmlWebViewCmdExecutor::SetOptionElementSelected(const ElementId& element, bool selected, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->SetOptionElementSelected(element, selected);
}

void QmlWebViewCmdExecutor::GetElementSize(const ElementId& element, Size* size, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->GetElementSize(element, size);
}

void QmlWebViewCmdExecutor::ElementSubmit(const ElementId& element, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->ElementSubmit(element);
}

void QmlWebViewCmdExecutor::GetElementText(const ElementId& element, std::string* element_text, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->GetElementText(element, element_text);
}

void QmlWebViewCmdExecutor::GetElementCssProperty(const ElementId& element, const std::string& property, base::Value** value, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->GetElementCssProperty(element, property, value);
}

void QmlWebViewCmdExecutor::FindElement(const ElementId& root_element, const std::string& locator, const std::string& query, ElementId* element, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->FindElement(root_element, locator, query, element);
}

void QmlWebViewCmdExecutor::FindElements(const ElementId& root_element, const std::string& locator, const std::string& query, std::vector<ElementId>* elements, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->FindElements(root_element, locator, query, elements);
}

void QmlWebViewCmdExecutor::ActiveElement(ElementId* element, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->ActiveElement(element);
}

void QmlWebViewCmdExecutor::SwitchTo(Error** error) {
    CHECK_VIEW_EXISTANCE

    session_->set_current_view(view_id_);

    session_->logger().Log(kInfoLogLevel, "SwitchTo - set current view ("+view_id_.id()+")");

    *error = webkitProxy_->SwitchTo();
}

void QmlWebViewCmdExecutor::SwitchToFrameWithNameOrId(const std::string& name_or_id, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->SwitchToFrameWithNameOrId(name_or_id);
}

void QmlWebViewCmdExecutor::SwitchToFrameWithIndex(int index, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->SwitchToFrameWithIndex(index);
}

void QmlWebViewCmdExecutor::SwitchToFrameWithElement(const ElementId& element, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->SwitchToFrameWithElement(element);
}

void QmlWebViewCmdExecutor::SwitchToTopFrame(Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->SwitchToTopFrame();
}

void QmlWebViewCmdExecutor::SwitchToTopFrameIfCurrentFrameInvalid(Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->SwitchToTopFrameIfCurrentFrameInvalid();
}

void QmlWebViewCmdExecutor::NavigateToURL(const std::string& url, bool sync, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->NavigateToURL(url, sync);

    if (sync) {
        session_->logger().Log(kFineLogLevel, "Web sync load - " + url);
    } else {
        session_->logger().Log(kFineLogLevel, "Web async load - " + url);
    }
}

void QmlWebViewCmdExecutor::GetURL(std::string* url, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->GetURL(url);
}

void QmlWebViewCmdExecutor::ExecuteScript(const std::string& script, const base::ListValue* const args, base::Value** value, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->ExecuteScript(script, args, value);
}

void QmlWebViewCmdExecutor::ExecuteAsyncScript(const std::string& script, const base::ListValue* const args, base::Value** value, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->ExecuteAsyncScript(script, args, value);
}

void QmlWebViewCmdExecutor::GetAppCacheStatus(int* status, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->GetAppCacheStatus(status);
}

void QmlWebViewCmdExecutor::GetCookies(const std::string& url, base::ListValue** cookies, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->GetCookies(url, cookies);
}

void QmlWebViewCmdExecutor::SetCookie(const std::string& url, base::DictionaryValue* cookie_dict, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->SetCookie(url, cookie_dict);
}

void QmlWebViewCmdExecutor::DeleteCookie(const std::string& url, const std::string& cookie_name, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->DeleteCookie(url, cookie_name);
}

void QmlWebViewCmdExecutor::GetStorageKeys(StorageType type, base::ListValue** keys, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->GetStorageKeys(type, keys);
}

void QmlWebViewCmdExecutor::SetStorageItem(StorageType type, const std::string& key, const std::string& value, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->SetStorageItem(type, key, value);
}

void QmlWebViewCmdExecutor::ClearStorage(StorageType type, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->ClearStorage(type);
}

void QmlWebViewCmdExecutor::GetStorageItem(StorageType type, const std::string& key, std::string* value, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->GetStorageItem(type, key, value);
}

void QmlWebViewCmdExecutor::RemoveStorageItem(StorageType type, const std::string& key, std::string* value, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->RemoveStorageItem(type, key, value);
}

void QmlWebViewCmdExecutor::GetStorageSize(StorageType type, int* size, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->GetStorageSize(type, size);
}

void QmlWebViewCmdExecutor::GetBrowserLog(base::ListValue** browserLog, Error **error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->GetBrowserLog(browserLog);
}

void QmlWebViewCmdExecutor::GetPlayerState(const ElementId& element, PlayerState* state, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->GetPlayerState(element, state);
}

void QmlWebViewCmdExecutor::SetPlayerState(const ElementId& element, PlayerState state, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->SetPlayerState(element, state);
}

void QmlWebViewCmdExecutor::GetPlayerVolume(const ElementId& element, double* volume, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->GetPlayerVolume(element, volume);
}

void QmlWebViewCmdExecutor::SetPlayerVolume(const ElementId& element, double volume, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->SetPlayerVolume(element, volume);
}

void QmlWebViewCmdExecutor::GetPlayingPosition(const ElementId& element, double* position, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->GetPlayingPosition(element, position);
}

void QmlWebViewCmdExecutor::SetPlayingPosition(const ElementId& element, double position, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->SetPlayingPosition(element, position);
}

void QmlWebViewCmdExecutor::SetMute(const ElementId& element, bool mute, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->SetMute(element, mute);
}

void QmlWebViewCmdExecutor::GetMute(const ElementId& element, bool* mute, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->GetMute(element, mute);
}

void QmlWebViewCmdExecutor::SetPlaybackSpeed(const ElementId &element, double speed, Error **error)
{
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->SetPlaybackSpeed(element, speed);
}

void QmlWebViewCmdExecutor::GetPlaybackSpeed(const ElementId &element, double *speed, Error **error)
{
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->GetPlaybackSpeed(element, speed);
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

void QmlWebViewCmdExecutor::SetOnline(bool online, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->SetOnline(online);
}

void QmlWebViewCmdExecutor::IsOnline(bool* online, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->IsOnline(online);
}

} // namespace webdriver     
