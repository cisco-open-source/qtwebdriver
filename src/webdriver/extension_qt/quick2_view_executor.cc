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

#include "extension_qt/quick2_view_executor.h"

#include "base/stringprintf.h"
#include "base/string_number_conversions.h"
#include "base/json/json_writer.h"

#include "value_conversion_util.h"
#include "webdriver_session.h"
#include "webdriver_view_factory.h"
#include "webdriver_util.h"
#include "q_key_converter.h"
#include "qml_view_util.h"
#include "qml_objname_util.h"

#include "extension_qt/event_dispatcher.h"
#include "extension_qt/wd_event_dispatcher.h"

// TODO: rename?
#include "extension_qt/widget_element_handle.h"

#include <QtCore/QBuffer>
#include <QtCore/QDebug>
#include <QtCore/QTimer>
#include <QtGui/QGuiApplication>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlExpression>
#include <QtQml/QQmlEngine>
#include <QtGui/QStyleHints>
#include <QtCore/QtMath>

#include "third_party/pugixml/pugixml.hpp"

namespace webdriver {

const ViewType Quick2ViewCmdExecutorCreator::QML_VIEW_TYPE = 0x13f6;    

Quick2ViewCmdExecutorCreator::Quick2ViewCmdExecutorCreator()
	: ViewCmdExecutorCreator() { }

ViewCmdExecutor* Quick2ViewCmdExecutorCreator::CreateExecutor(Session* session, ViewId viewId) const {
    QQuickView* pView = QQmlViewUtil::getQMLView(session, viewId);

    if (NULL != pView) {
        session->logger().Log(kFineLogLevel, "Quick2 executor for view("+viewId.id()+")");
        return new Quick2ViewCmdExecutor(session, viewId);
    }

	return NULL;
}

bool Quick2ViewCmdExecutorCreator::CanHandleView(Session* session, ViewId viewId, ViewType* viewType) const {
    QQuickView* pView = QQmlViewUtil::getQMLView(session, viewId);

    if (NULL != pView) {
        if (NULL != viewType) *viewType = QML_VIEW_TYPE;
        return true;
    }

    return false;
}

std::string Quick2ViewCmdExecutorCreator::GetViewTypeName() const {
    return "qml2";
}

Quick2ViewCmdExecutor::Quick2ViewCmdExecutor(Session* session, ViewId viewId)
	: QWindowViewCmdExecutor(session, viewId) {
}

Quick2ViewCmdExecutor::~Quick2ViewCmdExecutor() {}

QQuickView* Quick2ViewCmdExecutor::getView(const ViewId& viewId, Error** error) {
    QQuickView* pView = QQmlViewUtil::getQMLView(session_, viewId);

    if (NULL == pView) {
        session_->logger().Log(kWarningLogLevel, "checkView - no such qml view("+viewId.id()+")");
        *error = new Error(kNoSuchWindow);
        return NULL;
    }

    return pView;
}   

QQuickItem* Quick2ViewCmdExecutor::getElement(const ElementId &element, Error** error) {
    QElementHandle* element_handle = dynamic_cast<QElementHandle*>(session_->GetElementHandle(view_id_, element));

    if (NULL == element_handle) {
        *error = new Error(kNoSuchElement);
        return NULL;
    }

    if (!element_handle->is_valid()) {
        *error = new Error(kStaleElementReference);
        return NULL;
    }

    QQuickItem* retObj = qobject_cast<QQuickItem*>(element_handle->get());
    if (NULL == retObj) {
        *error = new Error(kUnknownError, "canot cast element to QQuickItem.");
        return NULL;
    }

    return retObj;
}

void Quick2ViewCmdExecutor::CanHandleUrl(const std::string& url, bool* can, Error **error) {
	*can = QQmlViewUtil::isUrlSupported(url);
}

void Quick2ViewCmdExecutor::GetSource(std::string* source, Error** error) {
    QQuickView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QQuickItem* parentItem = qobject_cast<QQuickItem*>(view->contentItem());
    if (NULL == parentItem) {
        session_->logger().Log(kInfoLogLevel, "no root element found.");    
        *error = new Error(kUnknownError, "no root element found.");
        return;
    }

    QByteArray byteArray;
    QBuffer buff(&byteArray);
    buff.open(QIODevice::ReadWrite);

    QQmlXmlSerializer serializer(&buff);
    serializer.createXml(parentItem);
    *source = byteArray.data();
}

void Quick2ViewCmdExecutor::SendKeys(const ElementId& element, const string16& keys, Error** error) {
    QQuickView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QQuickItem* pItem = getElement(element, error);
    if (NULL == pItem)
        return;

    if (!pItem->isVisible()) {
        *error = new Error(kElementNotVisible);
        return;
    }

    if (!pItem->isEnabled()) {
        *error = new Error(kInvalidElementState);
        return;
    }

    std::string err_msg;
    std::vector<QKeyEvent> key_events;
    int modifiers = Qt::NoModifier;

    if (!QKeyConverter::ConvertKeysToWebKeyEvents(keys,
                               session_->logger(),
                               true,
                               &modifiers,
                               &key_events,
                               &err_msg)) {
        session_->logger().Log(kSevereLogLevel, "SendKeys - cant convert keys:"+err_msg);
        *error = new Error(kUnknownError, "SendKeys - cant convert keys:"+err_msg);
        return;
    }

    std::vector<QKeyEvent>::iterator it = key_events.begin();
    while (it != key_events.end()) {
        view->sendEvent(pItem, &(*it));
        ++it;
    }
}

void Quick2ViewCmdExecutor::SendKeys(const string16& keys, Error** error) {
    QQuickView* view = getView(view_id_, error);
    if (NULL == view)
        return;

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

    QQuickItem* pFocusItem = getFocusItem(view);

    session_->set_sticky_modifiers(modifiers);

    std::vector<QKeyEvent>::iterator it = key_events.begin();
    while (it != key_events.end()) {

        bool consumed = WDEventDispatcher::getInstance()->dispatch(&(*it));

        if (!consumed) {
            if (NULL != pFocusItem) {
                view->sendEvent(pFocusItem, &(*it));
            } else {
                QGuiApplication::sendEvent(view, &(*it));
            }
        }
        ++it;
    }
}

void Quick2ViewCmdExecutor::MouseDoubleClick(Error** error) {
    QQuickView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QPoint point = ConvertPointToQPoint(session_->get_mouse_position());
    QPointF scenePoint(point.x(), point.y());
    QPointF screenPos(view->x() + scenePoint.x(), view->y() + scenePoint.y());

    session_->logger().Log(kFineLogLevel, base::StringPrintf("MouseDoubleClick,  scene: (%4d, %4d)", (int)scenePoint.x(), (int)scenePoint.y()));
    session_->logger().Log(kFineLogLevel, base::StringPrintf("MouseDoubleClick, screen: (%4d, %4d)", (int)screenPos.x(), (int)screenPos.y()));

    Qt::KeyboardModifiers sticky_modifiers(session_->get_sticky_modifiers());

    QMouseEvent *dbClckEvent = new QMouseEvent(QEvent::MouseButtonDblClick,
                            scenePoint,
                            screenPos,
                            Qt::LeftButton,
                            Qt::LeftButton,
                            sticky_modifiers);

    QMouseEvent *releaseEvent = new QMouseEvent(QEvent::MouseButtonRelease,
                            scenePoint,
                            screenPos,
                            Qt::LeftButton,
                            Qt::NoButton,
                            sticky_modifiers);

    QGuiApplication::postEvent(view, dbClckEvent);
    QGuiApplication::postEvent(view, releaseEvent);
}

void Quick2ViewCmdExecutor::MouseButtonUp(Error** error) {
    QQuickView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QPoint point = ConvertPointToQPoint(session_->get_mouse_position());
    QPointF scenePoint(point.x(), point.y());
    QPointF screenPos(view->x() + scenePoint.x(), view->y() + scenePoint.y());

    session_->logger().Log(kFineLogLevel, base::StringPrintf("MouseButtonUp,  scene: (%4d, %4d)", (int)scenePoint.x(), (int)scenePoint.y()));
    session_->logger().Log(kFineLogLevel, base::StringPrintf("MouseButtonUp, screen: (%4d, %4d)", (int)screenPos.x(), (int)screenPos.y()));

    Qt::KeyboardModifiers sticky_modifiers(session_->get_sticky_modifiers());

    QMouseEvent *releaseEvent = new QMouseEvent(QEvent::MouseButtonRelease,
                            scenePoint,
                            screenPos,
                            Qt::LeftButton,
                            Qt::NoButton,
                            sticky_modifiers);

    QGuiApplication::postEvent(view, releaseEvent);
}

void Quick2ViewCmdExecutor::MouseButtonDown(Error** error) {
    QQuickView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QPoint point = ConvertPointToQPoint(session_->get_mouse_position());
    QPointF scenePoint(point.x(), point.y());
    QPointF screenPos(view->x() + scenePoint.x(), view->y() + scenePoint.y());

    session_->logger().Log(kFineLogLevel, base::StringPrintf("MouseButtonDown,  scene: (%4d, %4d)", (int)scenePoint.x(), (int)scenePoint.y()));
    session_->logger().Log(kFineLogLevel, base::StringPrintf("MouseButtonDown, screen: (%4d, %4d)", (int)screenPos.x(), (int)screenPos.y()));

    Qt::KeyboardModifiers sticky_modifiers(session_->get_sticky_modifiers());

    QMouseEvent *pressEvent = new QMouseEvent(QEvent::MouseButtonPress,
                            scenePoint,
                            screenPos,
                            Qt::LeftButton,
                            Qt::LeftButton,
                            sticky_modifiers);

    QGuiApplication::postEvent(view, pressEvent);
}

void Quick2ViewCmdExecutor::MouseClick(MouseButton button, Error** error) {
    QQuickView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QPoint point = ConvertPointToQPoint(session_->get_mouse_position());
    QPointF scenePoint(point.x(), point.y());
    QPointF screenPos(view->x() + scenePoint.x(), view->y() + scenePoint.y());

    session_->logger().Log(kFineLogLevel, base::StringPrintf("MouseClick,  scene: (%4d, %4d)", (int)scenePoint.x(), (int)scenePoint.y()));
    session_->logger().Log(kFineLogLevel, base::StringPrintf("MouseClick, screen: (%4d, %4d)", (int)screenPos.x(), (int)screenPos.y()));

    Qt::MouseButton mouseButton = ConvertMouseButtonToQtMouseButton(button);

    Qt::KeyboardModifiers sticky_modifiers(session_->get_sticky_modifiers());

    QMouseEvent *pressEvent = new QMouseEvent(QEvent::MouseButtonPress,
                            scenePoint,
                            screenPos,
                            mouseButton,
                            mouseButton,
                            sticky_modifiers);

    QMouseEvent *releaseEvent = new QMouseEvent(QEvent::MouseButtonRelease,
                            scenePoint,
                            screenPos,
                            mouseButton,
                            Qt::NoButton,
                            sticky_modifiers);

    QGuiApplication::postEvent(view, pressEvent);
    QGuiApplication::postEvent(view, releaseEvent);
}

void Quick2ViewCmdExecutor::MouseWheel(const int delta, Error **error) {
    QQuickView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QPoint point = ConvertPointToQPoint(session_->get_mouse_position());
    QPointF scenePoint(point.x(), point.y());
    QPointF screenPos(view->x() + scenePoint.x(), view->y() + scenePoint.y());

    session_->logger().Log(kFineLogLevel, base::StringPrintf("MouseWheel,  scene: (%4d, %4d)", (int)scenePoint.x(), (int)scenePoint.y()));
    session_->logger().Log(kFineLogLevel, base::StringPrintf("MouseWheel, screen: (%4d, %4d)", (int)screenPos.x(), (int)screenPos.y()));

    // ignore scrolling distance in pixels on screen,
    QPoint pixelDelta(0, 0);

    QPoint angleDelta(0, delta);

    QWheelEvent *wheelEvent = new QWheelEvent(scenePoint,
                                              screenPos,
                                              pixelDelta,
                                              angleDelta,
                                              delta,
                                              Qt::Vertical,
                                              Qt::NoButton,
                                              Qt::NoModifier);

    QGuiApplication::postEvent(view, wheelEvent);
}

void Quick2ViewCmdExecutor::MouseMove(const int x_offset, const int y_offset, Error** error) {
	QQuickView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    Point prev_pos = session_->get_mouse_position();
    prev_pos.Offset(x_offset, y_offset);

	QPoint point = ConvertPointToQPoint(prev_pos);
    QPointF scenePoint(point.x(), point.y());

    QRectF sceneRect(QPointF(0,0), view->size());
    if (!sceneRect.contains(scenePoint)) {
        *error = new Error(kMoveTargetOutOfBounds);
        return;
    }

    moveMouseInternal(view, scenePoint);

    session_->set_mouse_position(prev_pos);
}

void Quick2ViewCmdExecutor::MouseMove(const ElementId& element, int x_offset, const int y_offset, Error** error) {
    QQuickView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QQuickItem* pItem = getElement(element, error);
    if (NULL == pItem)
        return;

    QPointF scenePoint = pItem->mapToScene(QPointF(x_offset, y_offset));

    QRectF sceneRect(QPointF(0,0), view->size());
    if (!sceneRect.contains(scenePoint)) {
        *error = new Error(kMoveTargetOutOfBounds);
        return;
    }

    moveMouseInternal(view, scenePoint);
    
    session_->set_mouse_position(Point(scenePoint.x(), scenePoint.y()));
}

void Quick2ViewCmdExecutor::MouseMove(const ElementId& element, Error** error) {
    QQuickView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QQuickItem* pItem = getElement(element, error);
    if (NULL == pItem)
        return;

    QPointF scenePoint = pItem->mapToScene(QPointF(pItem->width()/2.0, pItem->height()/2.0));

    QRectF sceneRect(QPointF(0,0), view->size());
    if (!sceneRect.contains(scenePoint)) {
        *error = new Error(kMoveTargetOutOfBounds);
        return;
    }

    moveMouseInternal(view, scenePoint);
    
    session_->set_mouse_position(Point(scenePoint.x(), scenePoint.y()));
}

void Quick2ViewCmdExecutor::moveMouseInternal(QQuickView* view, QPointF& point) {
    QPoint startViewPos = QCommonUtil::ConvertPointToQPoint(session_->get_mouse_position());
    QPointF targetScreenPos(view->x() + point.x(), view->y() + point.y());

    Qt::MouseButton mouseButton = (view->mouseGrabberItem())?(Qt::LeftButton):(Qt::NoButton);
    Qt::KeyboardModifiers sticky_modifiers(session_->get_sticky_modifiers());


    QStyleHints *styleHints = QGuiApplication::styleHints();
    QPointF minDragVector(styleHints->startDragDistance(), styleHints->startDragDistance());
    qreal dragThreshold = minDragVector.manhattanLength();
    QLineF line(startViewPos.x(), startViewPos.y(), point.x(), point.y());

    if ( line.length() > dragThreshold ) {
        // we need first mousemove event to initiate drag
        QPointF dragStartPoint = line.pointAt(dragThreshold / line.length());
        QPointF dragStartScreenPos(view->x() + dragStartPoint.x(), view->y() + dragStartPoint.y());

        QMouseEvent *moveEvent = new QMouseEvent(QEvent::MouseMove, dragStartPoint, dragStartScreenPos, Qt::NoButton, mouseButton, sticky_modifiers);
        QGuiApplication::postEvent(view, moveEvent);    
    }

    session_->logger().Log(kFineLogLevel, base::StringPrintf("MouseMove,  scene: (%4d, %4d)", (int)point.x(), (int)point.y()));
    session_->logger().Log(kFineLogLevel, base::StringPrintf("MouseMove, screen: (%4d, %4d)", (int)targetScreenPos.x(), (int)targetScreenPos.y()));

    QMouseEvent *moveEvent = new QMouseEvent(QEvent::MouseMove, point, targetScreenPos, Qt::NoButton, mouseButton, sticky_modifiers);
    QGuiApplication::postEvent(view, moveEvent);    
}

void Quick2ViewCmdExecutor::ClickElement(const ElementId& element, Error** error) {
	QQuickView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QQuickItem* pItem = getElement(element, error);
    if (NULL == pItem)
        return;

    if (!pItem->isVisible()) {
        *error = new Error(kElementNotVisible);
        return;
    }

    session_->logger().Log(kFineLogLevel, "Click on:");
    session_->logger().Log(kFineLogLevel, pItem->objectName().toStdString());

    QPointF scenePoint = pItem->mapToScene(QPointF(pItem->width()/2.0,pItem->height()/2.0));

    QRectF sceneRect(QPointF(0,0), view->size());
    if (!sceneRect.contains(scenePoint)) {
        *error = new Error(kMoveTargetOutOfBounds);
        return;
    }

    QMouseEvent *pressEvent = new QMouseEvent(QEvent::MouseButtonPress,
                             scenePoint,
                             Qt::LeftButton,
                             Qt::LeftButton,
                             Qt::NoModifier);

    QMouseEvent *releaseEvent = new QMouseEvent(QEvent::MouseButtonRelease,
                             scenePoint,
                             Qt::LeftButton,
                             Qt::NoButton,
                             Qt::NoModifier);

    QGuiApplication::postEvent(view, pressEvent);
    QGuiApplication::postEvent(view, releaseEvent);
    QGuiApplication::processEvents();
}

void Quick2ViewCmdExecutor::GetAttribute(const ElementId& element, const std::string& key, base::Value** value, Error** error) {
	QQuickView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QQuickItem* pItem = getElement(element, error);
    if (NULL == pItem)
        return;

    QVariant propertyValue = pItem->property(key.c_str());

    // substituate "id" with "objectName"
    QString idName("id");
    if (0 == idName.compare(key.c_str(), Qt::CaseInsensitive)) {
        propertyValue = pItem->property("objectName");
    }

    Value* val = NULL;

    if (propertyValue.isValid()) {
        // convert QVariant to base::Value
        if (propertyValue.canConvert<QString>()) {
            val = Value::CreateStringValue(propertyValue.toString().toStdString());
        } else {
            session_->logger().Log(kWarningLogLevel, "cant handle proprty type.");
        }
    } else {
        session_->logger().Log(kWarningLogLevel, "property not found.");
    }

    if (NULL == val) {
        val = Value::CreateNullValue();
    }

    scoped_ptr<Value> ret_value(val);
    *value = static_cast<Value*>(ret_value.release());
}

void Quick2ViewCmdExecutor::ClearElement(const ElementId& element, Error** error) {
    QQuickView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QQuickItem* pItem = getElement(element, error);
    if (NULL == pItem)
        return;

    if (!pItem->isVisible()) {
        *error = new Error(kElementNotVisible);
        return;
    }

    if (!pItem->isEnabled()) {
        *error = new Error(kInvalidElementState);
        return;
    }

    QVariant propertyValue = pItem->property("text");

    if (propertyValue.isValid()) {
        pItem->setProperty("text", QVariant(""));
        return;
    } else {
        session_->logger().Log(kWarningLogLevel, "there is no \"text\" property.");
    }    

    *error = new Error(kInvalidElementState);
}

void Quick2ViewCmdExecutor::IsElementDisplayed(const ElementId& element, bool ignore_opacity, bool* is_displayed, Error** error) {
	QQuickView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QQuickItem* pItem = getElement(element, error);
    if (NULL == pItem)
        return;

    *is_displayed = pItem->isVisible();
}

void Quick2ViewCmdExecutor::IsElementEnabled(const ElementId& element, bool* is_enabled, Error** error) {
    QQuickView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QQuickItem* pItem = getElement(element, error);
    if (NULL == pItem)
        return;

    *is_enabled = pItem->isEnabled();
}

void Quick2ViewCmdExecutor::ElementEquals(const ElementId& element1, const ElementId& element2, bool* is_equal, Error** error) {
	QQuickView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QQuickItem* pItem1 = getElement(element1, error);
    if (NULL == pItem1)
        return;

    QQuickItem* pItem2 = getElement(element2, error);
    if (NULL == pItem2)
        return;

    *is_equal = (pItem1 == pItem2);
}

void Quick2ViewCmdExecutor::GetElementLocation(const ElementId& element, Point* location, Error** error) {
	QQuickView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QQuickItem* pItem = getElement(element, error);
    if (NULL == pItem)
        return;

    QPointF scenePos = pItem->mapToScene(QPointF(0,0));

    *location = Point(scenePos.x(), scenePos.y());
}

void Quick2ViewCmdExecutor::GetElementLocationInView(const ElementId& element, Point* location, Error** error) {
	QQuickView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QQuickItem* pItem = getElement(element, error);
    if (NULL == pItem)
        return;

    QPointF scenePos = pItem->mapToScene(QPointF(0,0));

    // TODO: need check if pos fits into viewport?    
    *location = Point(scenePos.x(), scenePos.y());
}

void Quick2ViewCmdExecutor::GetElementTagName(const ElementId& element, std::string* tag_name, Error** error) {
    QQuickView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QQuickItem* pItem = getElement(element, error);
    if (NULL == pItem)
        return;

    QString className(pItem->metaObject()->className());
    QQmlViewUtil::removeInternalSuffixes(className);

    *tag_name = className.toStdString();
}

void Quick2ViewCmdExecutor::GetElementSize(const ElementId& element, Size* size, Error** error) {
	QQuickView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QQuickItem* pItem = getElement(element, error);
    if (NULL == pItem)
        return;

    *size = Size(pItem->width(), pItem->height());
}

void Quick2ViewCmdExecutor::GetElementText(const ElementId& element, std::string* element_text, Error** error) {
    QQuickView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QQuickItem* pItem = getElement(element, error);
    if (NULL == pItem)
        return;

    QVariant propertyValue = pItem->property("text");

    if (propertyValue.isValid()) {
        if (QVariant::String == propertyValue.type()) {
            *element_text = propertyValue.toString().toStdString();
        } else {
            session_->logger().Log(kWarningLogLevel, "cant handle non-string \"text\" proprty.");
        }
    } else {
        session_->logger().Log(kWarningLogLevel, "property not found.");
    }
}

void Quick2ViewCmdExecutor::FindElements(const ElementId& root_element, const std::string& locator, const std::string& query, std::vector<ElementId>* elements, Error** error) {
    QQuickView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    session_->logger().Log(kFineLogLevel, "FindElements, loc:"+locator+" query:"+query);

    Error* tmp_error = NULL;
    QQuickItem *parentItem = getElement(root_element, &tmp_error);
    scoped_ptr<Error> scoped_err(tmp_error);

    if (NULL == parentItem) {
        session_->logger().Log(kInfoLogLevel, "no root element specified, search from root.");
        parentItem = qobject_cast<QQuickItem*>(view->contentItem());;
    }

    if (NULL == parentItem) {
        session_->logger().Log(kWarningLogLevel, "no root element.");
        *error = new Error(kUnknownError, "no root element.");
        return;
    }

    if (locator == LocatorType::kXpath) {
        FindElementsByXpath(parentItem, query, elements, error);
    } else {
        FindElements(parentItem, locator, query, elements, error);
    }
}

void Quick2ViewCmdExecutor::FindElements(QQuickItem* parent, const std::string& locator, const std::string& query, std::vector<ElementId>* elements, Error** error) {
    if (FilterElement(parent, locator, query)) {
        ElementId elm;
        session_->AddElement(view_id_, new QElementHandle(parent), &elm);
        (*elements).push_back(elm);

        session_->logger().Log(kFineLogLevel, "element found: "+elm.id());
    }

    QList<QQuickItem*> childs = parent->childItems();
    foreach(QQuickItem *child, childs) {
        FindElements(child, locator, query, elements, error);
    }
}

void Quick2ViewCmdExecutor::ActiveElement(ElementId* element, Error** error) {
	QQuickView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QQuickItem* pFocusItem = getFocusItem(view);
    if (NULL == pFocusItem) {
        *error = new Error(kNoSuchElement);
        return;
    }

    ElementId active_element;

    session_->AddElement(view_id_, new QElementHandle(pFocusItem), &active_element);

    *element = active_element;
}

void Quick2ViewCmdExecutor::NavigateToURL(const std::string& url, bool sync, Error** error) {
	QQuickView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    session_->logger().Log(kFineLogLevel, "QML - NavigateToURL" + url);

    QUrl address(QString(url.c_str()));

    view->engine()->clearComponentCache();

    QQmlContext *rootContext = view->rootContext();
    QVariant pWin = rootContext->contextProperty("QmlWindow");

    if (pWin.isNull()) {
        rootContext->setContextProperty("QmlWindow", view);
    }

    if (sync) {
        QEventLoop loop;
        QObject::connect(view, SIGNAL(statusChanged(QQuickView::Status)),&loop,SLOT(quit()));
        view->setSource(address);

        if (QQuickView::Loading == view->status()) {
            loop.exec();
        }

        if (QQuickView::Ready != view->status()) {
            session_->logger().Log(kWarningLogLevel, "QML sync load, smth wrong. View is not in READY state.");
        }

        session_->logger().Log(kFineLogLevel, "QML sync load - " + url);
    } else {
        view->setSource(address);

        session_->logger().Log(kFineLogLevel, "QML async load - " + url);
    }
}

void Quick2ViewCmdExecutor::GetURL(std::string* url, Error** error) {
	QQuickView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    *url = view->source().toString().toStdString();
}

void Quick2ViewCmdExecutor::GetScreenShot(std::string* png, Error** error) {
    QQuickView* view = getView(view_id_, error);
    if (NULL == view)
        return;
    
    const FilePath::CharType kPngFileName[] = FILE_PATH_LITERAL("./screen.png");
    FilePath path = session_->temp_dir().Append(kPngFileName);;

    QImage image = view->grabWindow();

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

void Quick2ViewCmdExecutor::GetElementScreenShot(const ElementId& element, std::string* png, Error** error) {
    QQuickView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QQuickItem* pItem = getElement(element, error);
    if (NULL == pItem)
        return;

    QImage grabbed = view->grabWindow();
    QPointF scenePos = pItem->mapToScene(QPointF(0,0));
    QRectF rf(scenePos.x(), scenePos.y(), pItem->width(), pItem->height());        
    rf = rf.intersected(QRectF(0, 0, grabbed.width(), grabbed.height()));

    QImage image = grabbed.copy(rf.toAlignedRect());

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

void Quick2ViewCmdExecutor::ExecuteScript(const std::string& script, const base::ListValue* const args, base::Value** value, Error** error) {
    QQuickView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    std::string args_as_json;
    base::JSONWriter::Write(static_cast<const Value* const>(args), &args_as_json);

    std::string jscript = base::StringPrintf(
        "(function(x) { %s }.apply(this, %s));",
        script.c_str(),
        args_as_json.c_str());

    QQmlContext *rootContext = view->rootContext();
    QVariant p = rootContext->contextProperty("ObjectNameUtils");

    if (p.isNull()) {
        QQuickItem *rootItem = qobject_cast<QQuickItem*>(view->contentItem());
        ObjectNameUtils* objn = new ObjectNameUtils(rootItem);
        rootContext->setContextProperty("ObjectNameUtils", objn);
    }


    QQmlExpression expr(rootContext, view, jscript.c_str());
    QVariant result = expr.evaluate();
    if (expr.hasError()) {
        *error = new Error(kJavaScriptError, expr.error().toString().toStdString());
        session_->logger().Log(kWarningLogLevel, expr.error().toString().toStdString());
        return;
    }

    Value* val = NULL;

    if (result.isValid()) {
        // convert QVariant to base::Value
        switch (result.type()) {
        case QVariant::Bool:
            val = Value::CreateBooleanValue(result.toBool());
            break;
        case QVariant::Int:
            val = Value::CreateIntegerValue(result.toInt());
            break;
        case QVariant::Double:
            val = Value::CreateDoubleValue(result.toDouble());
            break;
        case QVariant::String:
            val = Value::CreateStringValue(result.toString().toStdString());
            break;
        default:
            session_->logger().Log(kWarningLogLevel, "cant handle result type.");
            *error = new Error(kUnknownError, "cant handle result type.");
        }
    } else {
        session_->logger().Log(kWarningLogLevel, "result is not valid.");
        *error = new Error(kJavaScriptError, "result is not valid.");
    }

    if (NULL == val) {
        val = Value::CreateNullValue();
    }

    scoped_ptr<Value> ret_value(val);
    *value = static_cast<Value*>(ret_value.release());
}

void Quick2ViewCmdExecutor::GetPlayerState(const ElementId &element, PlayerState *state, Error **error)
{
    QQuickView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    std::string tagName;
    GetElementTagName(element, &tagName, error);
    if(*error)
        return;

    if(tagName != "video" && tagName != "Video"){
        *error = new Error(kInvalidElementState);
        return;
    }

    QQuickItem* pItem = getElement(element, error);
    if (NULL == pItem)
        return;

    base::Value* playbackState = NULL;
    QVariant propertyValue = pItem->property("playbackState");

    if (propertyValue.isValid()) {
        // convert QVariant to base::Value
        switch (propertyValue.type()) {
        case QVariant::Int:
            playbackState = Value::CreateIntegerValue(propertyValue.toInt());
            break;
        default:
            session_->logger().Log(kWarningLogLevel, "wrong property type");
            *error = new Error(kInvalidElementState);
            return;
        }
    } else {
        session_->logger().Log(kWarningLogLevel, "property not found.");
        *error = new Error(kInvalidElementState);
        return;
    }

    int playbackStateInt;
    playbackState->GetAsInteger(&playbackStateInt);

    *state = (PlayerState)playbackStateInt;
}

void Quick2ViewCmdExecutor::SetPlayerState(const ElementId &element, PlayerState state, Error **error)
{
    QQuickView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QQuickItem* pItem = getElement(element, error);
    if (NULL == pItem)
        return;

    std::string tagName;
    GetElementTagName(element, &tagName, error);
    if(*error)
        return;

    if(tagName != "video" && tagName != "Video"){
        *error = new Error(kInvalidElementState);
        return;
    }

    bool isMethodCalled = false;
    switch(state){
        case Stopped: isMethodCalled = QMetaObject::invokeMethod(pItem, "stop"); break;
        case Playing: isMethodCalled = QMetaObject::invokeMethod(pItem, "play"); break;
        case Paused: isMethodCalled = QMetaObject::invokeMethod(pItem, "pause"); break;
    }

    if(!isMethodCalled){
        (*error) = new Error(kUnknownError,
                         std::string("Error while executing comand: There is no such member or the parameters did not match"));
    }
}

void Quick2ViewCmdExecutor::GetPlayerVolume(const ElementId &element, double *volume, Error **error)
{
    QQuickView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QQuickItem* pItem = getElement(element, error);
    if (NULL == pItem)
        return;

    std::string tagName;
    GetElementTagName(element, &tagName, error);
    if(*error)
        return;

    if(tagName != "video" && tagName != "Video"){
        *error = new Error(kInvalidElementState);
        return;
    }

    base::Value* volumeValue = NULL;
    GetAttribute(element, "volume", &volumeValue, error);

    if( *error){
        return;
    }

    volumeValue->GetAsDouble(volume);
}

void Quick2ViewCmdExecutor::SetPlayerVolume(const ElementId &element, double volume, Error **error)
{
    QQuickView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QQuickItem* pItem = getElement(element, error);
    if (NULL == pItem)
        return;

    std::string tagName;
    GetElementTagName(element, &tagName, error);
    if(*error)
        return;

    if(tagName != "video" && tagName != "Video"){
        *error = new Error(kInvalidElementState);
        return;
    }

    QVariant volumeVariant(volume);
    bool isPropertyAssigned = pItem->setProperty("volume", volumeVariant);

    if(!isPropertyAssigned){
        (*error) = new Error(kUnknownError,
                         std::string("Error while executing comand: There is no such member or the parameters did not match"));
    }
}

void Quick2ViewCmdExecutor::GetPlayingPosition(const ElementId &element, double *position, Error **error)
{
    QQuickView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QQuickItem* pItem = getElement(element, error);
    if (NULL == pItem)
        return;

    std::string tagName;
    GetElementTagName(element, &tagName, error);
    if(*error)
        return;

    if(tagName != "video" && tagName != "Video"){
        *error = new Error(kInvalidElementState);
        return;
    }

    base::Value* positionValue = NULL;
    GetAttribute(element, "position", &positionValue, error);

    if( *error){
        return;
    }

    int positionInt;
    positionValue->GetAsInteger(&positionInt);

    *position = positionInt/1000.0;
}

void Quick2ViewCmdExecutor::SetPlayingPosition(const ElementId &element, double position, Error **error)
{
    QQuickView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QQuickItem* pItem = getElement(element, error);
    if (NULL == pItem)
        return;

    std::string tagName;
    GetElementTagName(element, &tagName, error);
    if(*error)
        return;

    if(tagName != "video" && tagName != "Video"){
        *error = new Error(kInvalidElementState);
        return;
    }

    QVariant var((int)((position) * 1000));
    bool isMethodCalled = QMetaObject::invokeMethod(pItem, "seek",
                                                    Q_ARG(QVariant, var));

    if(!isMethodCalled){
        (*error) = new Error(kUnknownError,
                         std::string("Error while executing comand: There is no such member or the parameters did not match"));
    }
}

void Quick2ViewCmdExecutor::SetMute(const ElementId &element, bool mute, Error **error)
{
    QQuickView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QQuickItem* pItem = getElement(element, error);
    if (NULL == pItem)
        return;

    std::string tagName;
    GetElementTagName(element, &tagName, error);
    if(*error)
        return;

    if(tagName != "video" && tagName != "Video"){
        *error = new Error(kInvalidElementState);
        return;
    }

    QVariant muteVariant(mute);
    bool isPropertyAssigned = pItem->setProperty("muted", muteVariant);

    if(!isPropertyAssigned){
        (*error) = new Error(kUnknownError,
                         std::string("Error while executing comand: There is no such member or the parameters did not match"));
    }
}

void Quick2ViewCmdExecutor::GetMute(const ElementId &element, bool *mute, Error **error)
{
    QQuickView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QQuickItem* pItem = getElement(element, error);
    if (NULL == pItem)
        return;

    std::string tagName;
    GetElementTagName(element, &tagName, error);
    if(*error)
        return;
    if(tagName != "video" && tagName != "Video"){
        *error = new Error(kInvalidElementState);
        return;
    }

    base::Value* muteValue = NULL;
    GetAttribute(element, "muted", &muteValue, error);
    if( *error){
        return;
    }

    muteValue->GetAsBoolean(mute);
}

void Quick2ViewCmdExecutor::SetPlaybackSpeed(const ElementId &element, double speed, Error **error)
{
    QQuickView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QQuickItem* pItem = getElement(element, error);
    if (NULL == pItem)
        return;

    std::string tagName;
    GetElementTagName(element, &tagName, error);
    if(*error)
        return;

    if(tagName != "video" && tagName != "Video"){
        *error = new Error(kInvalidElementState);
        return;
    }

    QVariant speedVariant(speed);
    bool isPropertyAssigned = pItem->setProperty("playbackRate", speedVariant);

    if(!isPropertyAssigned){
        (*error) = new Error(kUnknownError,
                         std::string("Error while executing comand: There is no such member or the parameters did not match"));
    }
}

void Quick2ViewCmdExecutor::GetPlaybackSpeed(const ElementId &element, double *speed, Error **error)
{
    QQuickView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QQuickItem* pItem = getElement(element, error);
    if (NULL == pItem)
        return;

    std::string tagName;
    GetElementTagName(element, &tagName, error);
    if(*error)
        return;

    if(tagName != "video" && tagName != "Video"){
        *error = new Error(kInvalidElementState);
        return;
    }

    base::Value* positionValue = NULL;
    GetAttribute(element, "playbackRate", &positionValue, error);

    if( *error){
        return;
    }

    positionValue->GetAsDouble(speed);
}

void Quick2ViewCmdExecutor::TouchClick(const ElementId& element, Error **error)
{
    QQuickView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    Point location(0,0);

    // calculate the half of the element size and translate by it.
    Size size;
    GetElementSize(element, &size, error);
    if (*error)
        return;

    location.Offset(size.width() / 2, size.height() / 2);

    QPointF point = ConvertPointToQPoint(location);

    QQuickItem* pItem = getElement(element, error);
    if (*error)
        return;

    point = pItem->mapToScene(point);

    QTouchEvent *touchBeginEvent = createSimpleTouchEvent(QEvent::TouchBegin, Qt::TouchPointPressed, point);
    QTouchEvent *touchEndEvent = createSimpleTouchEvent(QEvent::TouchEnd, Qt::TouchPointReleased, point);

    QGuiApplication::postEvent(view, touchBeginEvent);
    QGuiApplication::postEvent(view, touchEndEvent);

    QGuiApplication::processEvents();
}


void Quick2ViewCmdExecutor::TouchDoubleClick(const ElementId& element, Error **error)
{
    QQuickView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    Point location(0,0);

    // calculate the half of the element size and translate by it.
    Size size;
    GetElementSize(element, &size, error);
    if (*error)
        return;

    location.Offset(size.width() / 2, size.height() / 2);

    QPointF point = ConvertPointToQPoint(location);

    QQuickItem* pItem = getElement(element, error);
    if (*error)
        return;

    point = pItem->mapToScene(point);

    QTouchEvent *touchBeginEvent = createSimpleTouchEvent(QEvent::TouchBegin, Qt::TouchPointPressed, point);
    QTouchEvent *touchEndEvent = createSimpleTouchEvent(QEvent::TouchEnd, Qt::TouchPointReleased, point);
    QTouchEvent *touchBeginEvent2 = createSimpleTouchEvent(QEvent::TouchBegin, Qt::TouchPointPressed, point);
    QTouchEvent *touchEndEvent2 = createSimpleTouchEvent(QEvent::TouchEnd, Qt::TouchPointReleased, point);

    QGuiApplication::postEvent(view, touchBeginEvent);
    QGuiApplication::postEvent(view, touchEndEvent);
    QGuiApplication::postEvent(view, touchBeginEvent2);
    QGuiApplication::postEvent(view, touchEndEvent2);


    QGuiApplication::processEvents();
}

void Quick2ViewCmdExecutor::TouchDown(const int &x, const int &y, Error **error)
{
    QQuickView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QPointF point = ConvertPointToQPoint(Point(x, y));

    QTouchEvent *touchBeginEvent = createSimpleTouchEvent(QEvent::TouchBegin, Qt::TouchPointPressed, point);

    QGuiApplication::postEvent(view, touchBeginEvent);

    QGuiApplication::processEvents();
}

void Quick2ViewCmdExecutor::TouchUp(const int &x, const int &y, Error **error)
{
    QQuickView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QPointF point = ConvertPointToQPoint(Point(x, y));

    QTouchEvent *touchEndEvent = createSimpleTouchEvent(QEvent::TouchEnd, Qt::TouchPointReleased, point);

    QGuiApplication::postEvent(view, touchEndEvent);

    QGuiApplication::processEvents();
}

void Quick2ViewCmdExecutor::TouchMove(const int &x, const int &y, Error **error)
{
    QQuickView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QPointF point = ConvertPointToQPoint(Point(x, y));

    QTouchEvent *touchMoveEvent = createSimpleTouchEvent(QEvent::TouchUpdate, Qt::TouchPointMoved, point);

    QGuiApplication::postEvent(view, touchMoveEvent);

    QGuiApplication::processEvents();
}

void Quick2ViewCmdExecutor::TouchLongClick(const ElementId& element, Error **error)
{
    QQuickView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    Point location(0,0);

    // calculate the half of the element size and translate by it.
    Size size;
    GetElementSize(element, &size, error);
    if (*error)
        return;

    location.Offset(size.width() / 2, size.height() / 2);

    QPointF point = ConvertPointToQPoint(location);

    QEventLoop loop;
    QTimer::singleShot(1000, &loop, SLOT(quit()));

    QQuickItem* pItem = getElement(element, error);
    if (*error)
        return;

    point = pItem->mapToScene(point);

    QTouchEvent *touchBeginEvent = createSimpleTouchEvent(QEvent::TouchBegin, Qt::TouchPointPressed, point);
    QGuiApplication::postEvent(view, touchBeginEvent);

    loop.exec();

    QTouchEvent *touchEndEvent = createSimpleTouchEvent(QEvent::TouchEnd, Qt::TouchPointReleased, point);
    QGuiApplication::postEvent(view, touchEndEvent);

    QGuiApplication::processEvents();

}

void Quick2ViewCmdExecutor::TouchScroll(const ElementId &element, const int &xoffset, const int &yoffset, Error **error)
{
    QQuickView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    Point location(0,0);

    // calculate the half of the element size and translate by it.
    Size size;
    GetElementSize(element, &size, error);
    if (*error)
        return;

    location.Offset(size.width() / 2, size.height() / 2);

    QPointF startPoint = ConvertPointToQPoint(location);

    QQuickItem* pItem = getElement(element, error);
    if (*error)
        return;

    startPoint = pItem->mapToScene(startPoint);

    Point offset(xoffset, yoffset);
    QPointF offsetPoint = ConvertPointToQPoint(offset);
    int stepCount = 20;
    int timeBetweenEvent = 30;
    QEventLoop loop;

    for (int i = 0; i <= stepCount; ++i)
    {
        QPointF touchPoint(startPoint.x()+offsetPoint.x()*i/stepCount, startPoint.y()+offsetPoint.y()*i/stepCount);

        QTouchEvent *touchEvent;
        if (i == 0)
            touchEvent = createSimpleTouchEvent(QEvent::TouchBegin, Qt::TouchPointPressed, touchPoint);
        else if (i == stepCount)
            touchEvent = createSimpleTouchEvent(QEvent::TouchEnd, Qt::TouchPointReleased, touchPoint);
        else
            touchEvent = createSimpleTouchEvent(QEvent::TouchUpdate, Qt::TouchPointMoved, touchPoint);


        QGuiApplication::postEvent(view, touchEvent);
        QTimer::singleShot(timeBetweenEvent, &loop, SLOT(quit()));
        loop.exec();
    }
    QGuiApplication::processEvents();
}

void Quick2ViewCmdExecutor::TouchFlick(const ElementId &element, const int &xoffset, const int &yoffset, const int &speed, Error **error)
{
    QQuickView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    Point location(0,0);

    // calculate the half of the element size and translate by it.
    Size size;
    GetElementSize(element, &size, error);
    if (*error)
        return;

    location.Offset(size.width() / 2, size.height() / 2);
    QPointF startPoint = ConvertPointToQPoint(location);

    QQuickItem* pItem = getElement(element, error);
    if (*error)
        return;

    startPoint = pItem->mapToScene(startPoint);

    QPointF offsetPoint = ConvertPointToQPoint(Point(xoffset, yoffset));

    QEventLoop loop;

    //some magic numbers
    int stepCount = 20;
    int timeBetweenEvent = 30/(speed+1);

    QVector2D velocity(xoffset*1000/(stepCount*timeBetweenEvent), yoffset*1000/(stepCount*timeBetweenEvent));

    for (int i = 0; i <= stepCount; ++i)
    {
        QPointF touchPoint(startPoint.x()+offsetPoint.x()*i/stepCount, startPoint.y()+offsetPoint.y()*i/stepCount);

        QTouchEvent *touchEvent;
        if (i == 0)
            touchEvent = createSimpleTouchEvent(QEvent::TouchBegin, Qt::TouchPointPressed, touchPoint, velocity);
        else if (i == stepCount)
            touchEvent = createSimpleTouchEvent(QEvent::TouchEnd, Qt::TouchPointReleased, touchPoint, velocity);
        else
            touchEvent = createSimpleTouchEvent(QEvent::TouchUpdate, Qt::TouchPointMoved, touchPoint, velocity);

        QGuiApplication::postEvent(view, touchEvent);
        QTimer::singleShot(timeBetweenEvent, &loop, SLOT(quit()));
        loop.exec();
        if (i == stepCount)
        {
            QPointF touchPoint(startPoint);
            touchEvent = createSimpleTouchEvent(QEvent::TouchCancel, Qt::TouchPointPressed, touchPoint);
            QGuiApplication::postEvent(view, touchEvent);
        }
    }
    QGuiApplication::processEvents();
}

void Quick2ViewCmdExecutor::TouchPinchRotate(const ElementId &element, const int &angle, Error **error)
{
    QQuickView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    Point location(0,0);

    // calculate the half of the element size and translate by it.
    Size size;
    GetElementSize(element, &size, error);
    if (*error)
        return;

    location.Offset(size.width() / 2, size.height() / 2);

    QPointF point = ConvertPointToQPoint(location);

    QQuickItem* pItem = getElement(element, error);
    if (*error)
        return;

    point = pItem->mapToScene(point);

    QPointF startPoint(point.x(), point.y());

    float dx = qApp->styleHints()->startDragDistance();
    float dy = qApp->styleHints()->startDragDistance();

    float degree = angle;
    float rad = qDegreesToRadians(degree);


    QTouchEvent *touchBeginEvent = create2PointTouchEvent(QEvent::TouchBegin, Qt::TouchPointPressed, startPoint, startPoint);
    QGuiApplication::postEvent(view, touchBeginEvent);

    int stepCount = 20;

    for (int i = 0; i <= stepCount; ++i)
    {
        QPointF point1(point.x() - dx*cos(rad*i/stepCount), point.y() - dy*sin(rad*i/stepCount));
        QPointF point2(point.x() + dx*cos(rad*i/stepCount), point.y() + dy*sin(rad*i/stepCount));

        QTouchEvent *touchMoveEvent = create2PointTouchEvent(QEvent::TouchUpdate, Qt::TouchPointMoved, point1, point2);
        QGuiApplication::postEvent(view, touchMoveEvent);

        if (stepCount == i)
        {
            QTouchEvent *touchEndEvent = create2PointTouchEvent(QEvent::TouchEnd, Qt::TouchPointReleased, point1, point2);
            QGuiApplication::postEvent(view, touchEndEvent);
        }
    }

    QGuiApplication::processEvents();
}

void Quick2ViewCmdExecutor::TouchPinchZoom(const ElementId &element, const double &scale, Error **error)
{
    QQuickView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    Point location(0,0);

    // calculate the half of the element size and translate by it.
    Size size;
    GetElementSize(element, &size, error);
    if (*error)
        return;

    location.Offset(size.width() / 2, size.height() / 2);

    QPointF point = ConvertPointToQPoint(location);

    QQuickItem* pItem = getElement(element, error);
    if (*error)
        return;

    point = pItem->mapToScene(point);

    QPointF startPoint(point.x(), point.y());

    float dx;

    if (scale >= 1)
        dx = scale*qApp->styleHints()->startDragDistance();
    else
        dx = (1-scale)*qApp->styleHints()->startDragDistance();

    QTouchEvent *touchBeginEvent = create2PointTouchEvent(QEvent::TouchBegin, Qt::TouchPointPressed, startPoint, startPoint);
    QGuiApplication::postEvent(view, touchBeginEvent);

    int stepCount = 20;

    for (int i = 1; i <= stepCount; ++i)
    {
        QPointF point1(startPoint);
        QPointF point2(startPoint);
        if (i == 1)
        {
            //need to trigger PinchStart event
            point1.setX(startPoint.x() + qApp->styleHints()->startDragDistance());
        }
        else
        {
            if (scale > 1)
                point1.setX(startPoint.x() + dx*i/stepCount);
            else
                point1.setX(startPoint.x() + qApp->styleHints()->startDragDistance() - dx*i/stepCount);
        }

        QTouchEvent *touchMoveEvent = create2PointTouchEvent(QEvent::TouchUpdate, Qt::TouchPointMoved, point2, point1);
        QGuiApplication::postEvent(view, touchMoveEvent);

        if (i == stepCount)
        {
            QTouchEvent *touchEndEvent = create2PointTouchEvent(QEvent::TouchEnd, Qt::TouchPointReleased, point2, point1);
            QGuiApplication::postEvent(view, touchEndEvent);
        }
    }

    QGuiApplication::processEvents();
}

QQuickItem* Quick2ViewCmdExecutor::getFocusItem(QQuickView* view) {
    QQuickItem* pFocusItem = view->activeFocusItem();
    if (NULL != pFocusItem) return pFocusItem;

    // no active focus - try to find manually
    QQuickItem* pContentItem = view->contentItem();
    if (NULL != pContentItem) {
        pFocusItem = pContentItem;
        while (pFocusItem->isFocusScope()
               && pFocusItem->scopedFocusItem()
               && pFocusItem->scopedFocusItem()->isEnabled()) {
            pFocusItem = pFocusItem->scopedFocusItem();
        }
    }

    return pFocusItem;
}

bool Quick2ViewCmdExecutor::FilterElement(const QQuickItem* item, const std::string& locator, const std::string& query) {
    QString className(item->metaObject()->className());
    QQmlViewUtil::removeInternalSuffixes(className);

    if (locator == LocatorType::kClassName) {
        if (query == className.toStdString())
            return true;
    } else if (locator == LocatorType::kTagName) {
        if (query == className.toStdString())
            return true;
    } else if (locator == LocatorType::kId) {
        if (query == item->objectName().toStdString())
            return true;
    } else {
        session_->logger().Log(kWarningLogLevel, "unsupported locator - "+locator);
        // LocatorType::kName
        // LocatorType::kLinkText
        // LocatorType::kPartialLinkText
        // LocatorType::kCss
    }

    return false;
}

void Quick2ViewCmdExecutor::FindElementsByXpath(QQuickItem* parent, const std::string &query, std::vector<ElementId>* elements, Error **error) {
    QByteArray byteArray;
    QBuffer buff(&byteArray);
    buff.open(QIODevice::ReadWrite);

    QQmlXmlSerializer serializer(&buff);
    serializer.setDumpAll(true);
    serializer.createXml(parent);

    buff.seek(0);

    // prepare pointer to data to parse
    const void* content = byteArray.data();
    int content_size = byteArray.size();

    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_buffer(content, content_size);

    if (result) {
        session_->logger().Log(kFineLogLevel, "UI XML parsed without errors");

        // Select nodes via compiled query
        pugi::xpath_query query_nodes(query.c_str());
        pugi::xpath_node_set found_nodes = query_nodes.evaluate_node_set(doc);

        if ( (NULL == query_nodes.result().error) && 
             (pugi::xpath_type_node_set == query_nodes.return_type()) ) {

            for (pugi::xpath_node_set::const_iterator it = found_nodes.begin(); it != found_nodes.end(); ++it) {
                pugi::xpath_node node = *it;

                QString elemId(node.node().attribute("elementId").value());

                if (!elemId.isEmpty()) {
                    const QQmlXmlSerializer::XMLElementMap& elementsMap = serializer.getElementsMap();
                    if (elementsMap.contains(elemId)) {
                        ElementId elm;
                        session_->AddElement(view_id_, new QElementHandle(elementsMap[elemId]), &elm);
                        (*elements).push_back(elm);
                        session_->logger().Log(kFineLogLevel, "element found: "+elm.id());
                    } else {
                        session_->logger().Log(kSevereLogLevel, "cant get element from map, skipped");
                    }
                }
            }
        } else {
            std::string error_descr = "Cant evaluate XPath to node set: ";       
            error_descr += query_nodes.result().description();
            session_->logger().Log(kWarningLogLevel, error_descr);
            *error = new Error(kXPathLookupError);
        }
    }
    else
    {
        std::string error_descr = "   Error description: ";
        error_descr += result.description();
        session_->logger().Log(kWarningLogLevel, "UI XML parsed with errors:");
        session_->logger().Log(kWarningLogLevel, error_descr);
    }

    // destroy tree
    doc.reset();

    buff.close();
}

} //namespace webdriver 
