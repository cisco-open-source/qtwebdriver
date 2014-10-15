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

#include "extension_qt/graphics_web_view_executor.h"

#include "base/stringprintf.h"

#include "webdriver_logging.h"
#include "webdriver_session.h"
#include "q_key_converter.h"
#include "extension_qt/graphics_web_view_handle.h"
#include "extension_qt/event_dispatcher.h"
#include "extension_qt/wd_event_dispatcher.h"
#include "common_util.h"
#include "web_view_util.h"
#include "qwebkit_proxy.h"

#include <QtCore/QDebug>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets/QStyleOptionGraphicsItem>
#include <QtWidgets/QApplication>
#else
#include <QtGui/QStyleOptionGraphicsItem>
#include <QtGui/QApplication>
#endif

#define _USE_MATH_DEFINES
#include <math.h>

namespace webdriver {

#define CHECK_VIEW_EXISTANCE    \
    if (NULL == view_) { \
        session_->logger().Log(kWarningLogLevel, "checkView - no such graphics web view("+view_id_.id()+")"); \
        *error = new Error(kNoSuchWindow); \
        return; \
    } \
    webkitProxy_.reset(new QWebkitProxy(session_, (view_)?view_->page():NULL));

const ViewType GraphicsWebViewCmdExecutorCreator::GRAPHICS_WEB_VIEW_TYPE = 0x73c1;

static QGraphicsWebView* _getGraphicsWebView(Session* session, const ViewId& viewId) {
    ViewHandle* viewHandle =  session->GetViewHandle(viewId);
    if (NULL == viewHandle) 
        return NULL;

    GraphicsWebViewHandle* qViewHandle = dynamic_cast<GraphicsWebViewHandle*>(viewHandle);
    if (NULL == qViewHandle)
        return NULL;

    return qobject_cast<QGraphicsWebView*>(qViewHandle->get());
}

GraphicsWebViewCmdExecutorCreator::GraphicsWebViewCmdExecutorCreator()
    : ViewCmdExecutorCreator() { }

GraphicsWebViewCmdExecutorCreator::~GraphicsWebViewCmdExecutorCreator() {}

ViewCmdExecutor* GraphicsWebViewCmdExecutorCreator::CreateExecutor(Session* session, ViewId viewId) const {
    QGraphicsWebView* pWebView = _getGraphicsWebView(session, viewId);
    if (NULL != pWebView) {
        session->logger().Log(kFineLogLevel, "Graphics Web executor for view("+viewId.id()+")");
        return new GraphicsWebViewCmdExecutor(session, viewId);
    }

    return NULL;
}

bool GraphicsWebViewCmdExecutorCreator::CanHandleView(Session* session, ViewId viewId, ViewType* viewType) const {
    QGraphicsWebView* pWebView = _getGraphicsWebView(session, viewId);

    if (NULL != pWebView) {
        if (NULL != viewType) *viewType = GRAPHICS_WEB_VIEW_TYPE;
        return true;
    }

    return false;
}

std::string GraphicsWebViewCmdExecutorCreator::GetViewTypeName() const {
    return "graphicsWebView";
}

GraphicsWebViewCmdExecutor::GraphicsWebViewCmdExecutor(Session* session, ViewId viewId)
    : ViewCmdExecutor(session, viewId) {

    view_ = _getGraphicsWebView(session, viewId);
}

GraphicsWebViewCmdExecutor::~GraphicsWebViewCmdExecutor() {

};

QGraphicsWebView* GraphicsWebViewCmdExecutor::getView(const ViewId& viewId, Error** error) {
    QGraphicsWebView* pView = _getGraphicsWebView(session_, view_id_);

    if (NULL == pView) {
        session_->logger().Log(kWarningLogLevel, "checkView - no such view("+viewId.id()+")");
        *error = new Error(kNoSuchWindow);
        return NULL;
    }

    return pView;
}

void GraphicsWebViewCmdExecutor::CanHandleUrl(const std::string& url, bool* can, Error **error) {
    QGraphicsWebView* pWebView = getView(view_id_, error);
    if (NULL == pWebView)
        return;
    
    *can = QWebViewUtil::isUrlSupported(pWebView->page(), url, error);
}

void GraphicsWebViewCmdExecutor::GetTitle(std::string* title, Error **error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->GetTitle(title);
}

void GraphicsWebViewCmdExecutor::GetWindowName(std::string* name, Error ** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->GetWindowName(name);

    session_->logger().Log(kFineLogLevel, "GetWindowName - "+*name);
}

void GraphicsWebViewCmdExecutor::GetBounds(Rect *bounds, Error **error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->GetBounds(bounds);
}
    
void GraphicsWebViewCmdExecutor::GetScreenShot(std::string* png, Error** error) {
    CHECK_VIEW_EXISTANCE
    
    QImage image(view_->boundingRect().size().toSize(), QImage::Format_RGB32);
    image.fill(QColor(0, 0, 0).rgb());
    QPainter painter(&image);
    QStyleOptionGraphicsItem styleOption;
    qobject_cast<QGraphicsObject*>(view_)->paint(&painter, &styleOption);
    painter.end();

    saveScreenshot(image, png, error);
}

void GraphicsWebViewCmdExecutor::GetElementScreenShot(const ElementId& element, std::string* png, Error** error) {
    CHECK_VIEW_EXISTANCE

    Point location;
    *error = webkitProxy_->GetElementLocationInView(element, &location);
    if (*error)
        return;

    Size size;
    *error = webkitProxy_->GetElementSize(element, &size);
    if (*error)
        return;

    QImage image(view_->boundingRect().size().toSize(), QImage::Format_RGB32);
    image.fill(QColor(0, 0, 0).rgb());
    QPainter painter(&image);
    QStyleOptionGraphicsItem styleOption;
    qobject_cast<QGraphicsObject*>(view_)->paint(&painter, &styleOption);
    painter.end();

    QRect viewRect = image.rect();
    QRect elementRect(location.x(), location.y(), size.width(), size.height());
    QRect intersectedRect = viewRect.intersected(elementRect);

    session_->logger().Log(kFineLogLevel, base::StringPrintf("GetElementScreenShot, view: (%2d;%2d : %4d;%4d)", viewRect.x(), viewRect.y(), viewRect.width(), viewRect.height()));
    session_->logger().Log(kFineLogLevel, base::StringPrintf("GetElementScreenShot, elem: (%2d;%2d : %4d;%4d)", elementRect.x(), elementRect.y(), elementRect.width(), elementRect.height()));
    session_->logger().Log(kFineLogLevel, base::StringPrintf("GetElementScreenShot,  res: (%2d;%2d : %4d;%4d)", intersectedRect.x(), intersectedRect.y(), intersectedRect.width(), intersectedRect.height()));

    if ((0 == intersectedRect.width()) || (0 == intersectedRect.height())) {
        *error = new Error(kMoveTargetOutOfBounds);
        session_->logger().Log(kWarningLogLevel, "GetElementScreenShot, element is not in view.");
        return;
    }

    QImage elementImage = image.copy(intersectedRect);

    saveScreenshot(elementImage, png, error);
}

void GraphicsWebViewCmdExecutor::saveScreenshot(QImage& image, std::string* png, Error** error) {
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

void GraphicsWebViewCmdExecutor::GoForward(Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->GoForward();
}

void GraphicsWebViewCmdExecutor::GoBack(Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->GoBack();
}

void GraphicsWebViewCmdExecutor::Reload(Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->Reload();
}

void GraphicsWebViewCmdExecutor::GetSource(std::string* source, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->GetSource(source);
}

void GraphicsWebViewCmdExecutor::SendKeys(const string16& keys, Error** error) {
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

void GraphicsWebViewCmdExecutor::SendKeys(const ElementId& element, const string16& keys, Error** error) {
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

void GraphicsWebViewCmdExecutor::MouseDoubleClick(Error** error) {
    CHECK_VIEW_EXISTANCE

    QPoint point = QCommonUtil::ConvertPointToQPoint(session_->get_mouse_position());
    
    QMouseEvent *dbEvent = new QMouseEvent(QEvent::MouseButtonDblClick, point, Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
    QMouseEvent *releaseEvent = new QMouseEvent(QEvent::MouseButtonRelease, point, Qt::LeftButton, Qt::NoButton, Qt::NoModifier);

    QApplication::postEvent(view_->page(), dbEvent);
    QApplication::postEvent(view_->page(), releaseEvent);
}

void GraphicsWebViewCmdExecutor::MouseButtonUp(Error** error) {
    CHECK_VIEW_EXISTANCE

    QPoint point = QCommonUtil::ConvertPointToQPoint(session_->get_mouse_position());
    
    QMouseEvent *releaseEvent = new QMouseEvent(QEvent::MouseButtonRelease, point, Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
    QApplication::postEvent(view_->page(), releaseEvent);
}

void GraphicsWebViewCmdExecutor::MouseButtonDown(Error** error) {
    CHECK_VIEW_EXISTANCE

    QPoint point = QCommonUtil::ConvertPointToQPoint(session_->get_mouse_position());
    
    QMouseEvent *pressEvent = new QMouseEvent(QEvent::MouseButtonPress, point, Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
    QApplication::sendEvent(view_->page(), pressEvent);
}

void GraphicsWebViewCmdExecutor::MouseClick(MouseButton button, Error** error) {
    CHECK_VIEW_EXISTANCE

    QPoint point = QCommonUtil::ConvertPointToQPoint(session_->get_mouse_position());
    Qt::MouseButton mouseButton = QCommonUtil::ConvertMouseButtonToQtMouseButton(button);

    QMouseEvent *pressEvent = new QMouseEvent(QEvent::MouseButtonPress, point, mouseButton, Qt::NoButton, Qt::NoModifier);
    QMouseEvent *releaseEvent = new QMouseEvent(QEvent::MouseButtonRelease, point, mouseButton, Qt::NoButton, Qt::NoModifier);

    QApplication::postEvent(view_->page(), pressEvent);
    QApplication::postEvent(view_->page(), releaseEvent);
}

void GraphicsWebViewCmdExecutor::MouseWheel(const int delta, Error** error) {
    CHECK_VIEW_EXISTANCE

    QPoint point = QCommonUtil::ConvertPointToQPoint(session_->get_mouse_position());

    QWheelEvent *wheelEvent = new QWheelEvent(point, delta, Qt::NoButton, Qt::NoModifier);

    QApplication::postEvent(view_->page(), wheelEvent);
}

void GraphicsWebViewCmdExecutor::MouseMove(const int x_offset, const int y_offset, Error** error) {
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

void GraphicsWebViewCmdExecutor::MouseMove(const ElementId& element, int x_offset, const int y_offset, Error** error) {
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

void GraphicsWebViewCmdExecutor::MouseMove(const ElementId& element, Error** error) {
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

void GraphicsWebViewCmdExecutor::ClickElement(const ElementId& element, Error** error) {
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

void GraphicsWebViewCmdExecutor::GetAttribute(const ElementId& element, const std::string& key, base::Value** value, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->GetAttribute(element, key, value);
}

void GraphicsWebViewCmdExecutor::ClearElement(const ElementId& element, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->ClearElement(element);
}

void GraphicsWebViewCmdExecutor::IsElementDisplayed(const ElementId& element, bool ignore_opacity, bool* is_displayed, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->IsElementDisplayed(element, ignore_opacity, is_displayed);
}

void GraphicsWebViewCmdExecutor::IsElementEnabled(const ElementId& element, bool* is_enabled, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->IsElementEnabled(element, is_enabled);
}

void GraphicsWebViewCmdExecutor::ElementEquals(const ElementId& element1, const ElementId& element2, bool* is_equal, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->ElementEquals(element1, element2, is_equal);
}

void GraphicsWebViewCmdExecutor::GetElementLocation(const ElementId& element, Point* location, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->GetElementLocation(element, location);
}

void GraphicsWebViewCmdExecutor::GetElementLocationInView(const ElementId& element, Point* location, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->GetElementLocationInView(element, location);
}

void GraphicsWebViewCmdExecutor::GetElementTagName(const ElementId& element, std::string* tag_name, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->GetElementTagName(element, tag_name);
}

void GraphicsWebViewCmdExecutor::IsOptionElementSelected(const ElementId& element, bool* is_selected, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->IsOptionElementSelected(element, is_selected);
}

void GraphicsWebViewCmdExecutor::SetOptionElementSelected(const ElementId& element, bool selected, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->SetOptionElementSelected(element, selected);
}

void GraphicsWebViewCmdExecutor::GetElementSize(const ElementId& element, Size* size, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->GetElementSize(element, size);
}

void GraphicsWebViewCmdExecutor::ElementSubmit(const ElementId& element, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->ElementSubmit(element);
}

void GraphicsWebViewCmdExecutor::GetElementText(const ElementId& element, std::string* element_text, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->GetElementText(element, element_text);
}

void GraphicsWebViewCmdExecutor::GetElementCssProperty(const ElementId& element, const std::string& property, base::Value** value, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->GetElementCssProperty(element, property, value);
}

void GraphicsWebViewCmdExecutor::FindElement(const ElementId& root_element, const std::string& locator, const std::string& query, ElementId* element, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->FindElement(root_element, locator, query, element);
}

void GraphicsWebViewCmdExecutor::FindElements(const ElementId& root_element, const std::string& locator, const std::string& query, std::vector<ElementId>* elements, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->FindElements(root_element, locator, query, elements);
}

void GraphicsWebViewCmdExecutor::ActiveElement(ElementId* element, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->ActiveElement(element);
}

void GraphicsWebViewCmdExecutor::SwitchTo(Error** error) {
    CHECK_VIEW_EXISTANCE

    session_->set_current_view(view_id_);

    session_->logger().Log(kInfoLogLevel, "SwitchTo - set current view ("+view_id_.id()+")");

    *error = webkitProxy_->SwitchTo();
}

void GraphicsWebViewCmdExecutor::SwitchToFrameWithNameOrId(const std::string& name_or_id, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->SwitchToFrameWithNameOrId(name_or_id);
}

void GraphicsWebViewCmdExecutor::SwitchToFrameWithIndex(int index, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->SwitchToFrameWithIndex(index);
}

void GraphicsWebViewCmdExecutor::SwitchToFrameWithElement(const ElementId& element, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->SwitchToFrameWithElement(element);
}

void GraphicsWebViewCmdExecutor::SwitchToTopFrame(Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->SwitchToTopFrame();
}

void GraphicsWebViewCmdExecutor::SwitchToTopFrameIfCurrentFrameInvalid(Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->SwitchToTopFrameIfCurrentFrameInvalid();
}

void GraphicsWebViewCmdExecutor::NavigateToURL(const std::string& url, bool sync, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->NavigateToURL(url, sync);

    if (sync) {
        session_->logger().Log(kFineLogLevel, "Web sync load - " + url);
    } else {
        session_->logger().Log(kFineLogLevel, "Web async load - " + url);
    }
}

void GraphicsWebViewCmdExecutor::GetURL(std::string* url, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->GetURL(url);
}

void GraphicsWebViewCmdExecutor::ExecuteScript(const std::string& script, const base::ListValue* const args, base::Value** value, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->ExecuteScript(script, args, value);
}

void GraphicsWebViewCmdExecutor::ExecuteAsyncScript(const std::string& script, const base::ListValue* const args, base::Value** value, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->ExecuteAsyncScript(script, args, value);
}

void GraphicsWebViewCmdExecutor::GetAppCacheStatus(int* status, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->GetAppCacheStatus(status);
}

void GraphicsWebViewCmdExecutor::GetCookies(const std::string& url, base::ListValue** cookies, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->GetCookies(url, cookies);
}

void GraphicsWebViewCmdExecutor::SetCookie(const std::string& url, base::DictionaryValue* cookie_dict, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->SetCookie(url, cookie_dict);
}

void GraphicsWebViewCmdExecutor::DeleteCookie(const std::string& url, const std::string& cookie_name, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->DeleteCookie(url, cookie_name);
}

void GraphicsWebViewCmdExecutor::GetStorageKeys(StorageType type, base::ListValue** keys, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->GetStorageKeys(type, keys);
}

void GraphicsWebViewCmdExecutor::SetStorageItem(StorageType type, const std::string& key, const std::string& value, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->SetStorageItem(type, key, value);
}

void GraphicsWebViewCmdExecutor::ClearStorage(StorageType type, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->ClearStorage(type);
}

void GraphicsWebViewCmdExecutor::GetStorageItem(StorageType type, const std::string& key, std::string* value, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->GetStorageItem(type, key, value);
}

void GraphicsWebViewCmdExecutor::RemoveStorageItem(StorageType type, const std::string& key, std::string* value, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->RemoveStorageItem(type, key, value);
}

void GraphicsWebViewCmdExecutor::GetStorageSize(StorageType type, int* size, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->GetStorageSize(type, size);
}

void GraphicsWebViewCmdExecutor::GetBrowserLog(base::ListValue** browserLog, Error **error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->GetBrowserLog(browserLog);
}

void GraphicsWebViewCmdExecutor::GetPlayerState(const ElementId& element, PlayerState* state, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->GetPlayerState(element, state);
}

void GraphicsWebViewCmdExecutor::SetPlayerState(const ElementId& element, PlayerState state, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->SetPlayerState(element, state);
}

void GraphicsWebViewCmdExecutor::GetPlayerVolume(const ElementId& element, double* volume, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->GetPlayerVolume(element, volume);
}

void GraphicsWebViewCmdExecutor::SetPlayerVolume(const ElementId& element, double volume, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->SetPlayerVolume(element, volume);
}

void GraphicsWebViewCmdExecutor::GetPlayingPosition(const ElementId& element, double* position, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->GetPlayingPosition(element, position);
}

void GraphicsWebViewCmdExecutor::SetPlayingPosition(const ElementId& element, double position, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->SetPlayingPosition(element, position);
}

void GraphicsWebViewCmdExecutor::SetMute(const ElementId& element, bool mute, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->SetMute(element, mute);
}

void GraphicsWebViewCmdExecutor::GetMute(const ElementId& element, bool* mute, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->GetMute(element, mute);
}

void GraphicsWebViewCmdExecutor::SetPlaybackSpeed(const ElementId &element, double speed, Error **error)
{
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->SetPlaybackSpeed(element, speed);
}

void GraphicsWebViewCmdExecutor::GetPlaybackSpeed(const ElementId &element, double *speed, Error **error)
{
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->GetPlaybackSpeed(element, speed);
}

void GraphicsWebViewCmdExecutor::VisualizerSource(std::string* source, Error** error) {
    // TODO:
}

void GraphicsWebViewCmdExecutor::VisualizerShowPoint(Error** error) {
    // TODO:
}

void GraphicsWebViewCmdExecutor::GetOrientation(std::string *orientation, Error **error) {
    // TODO:
}

void GraphicsWebViewCmdExecutor::SetOnline(bool online, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->SetOnline(online);
}

void GraphicsWebViewCmdExecutor::IsOnline(bool* online, Error** error) {
    CHECK_VIEW_EXISTANCE

    *error = webkitProxy_->IsOnline(online);
}


} // namespace webdriver     
