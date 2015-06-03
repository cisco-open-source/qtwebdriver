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

#include "extension_qt/qml_view_executor.h"

#include "base/stringprintf.h"
#include "base/string_number_conversions.h"
#include "base/json/json_writer.h"

#include "value_conversion_util.h"
#include "webdriver_session.h"
#include "webdriver_view_factory.h"
#include "common_util.h"
#include "q_key_converter.h"
#include "extension_qt/widget_element_handle.h"
#include "extension_qt/widget_view_handle.h"
#include "qml_view_util.h"
#include "qml_objname_util.h"

#include <QtCore/QBuffer>
#include <QtCore/QDebug>
#include <QtGui/QApplication>
#include <QtGui/QStyleOptionGraphicsItem>

#include <QtDeclarative/QDeclarativeExpression>
#include <QtDeclarative/QDeclarativeEngine>
#include <QtDeclarative/QDeclarativeContext>

#include "third_party/pugixml/pugixml.hpp"

namespace webdriver {

const ViewType QQmlViewCmdExecutorCreator::QML_VIEW_TYPE = 0x13f6;    

QQmlViewCmdExecutorCreator::QQmlViewCmdExecutorCreator()
	: ViewCmdExecutorCreator() { }

ViewCmdExecutor* QQmlViewCmdExecutorCreator::CreateExecutor(Session* session, ViewId viewId) const {
    QDeclarativeView* pView = QQmlViewUtil::getQMLView(session, viewId);

    if (NULL != pView) {
        session->logger().Log(kFineLogLevel, "QML executor for view("+viewId.id()+")");
        return new QQmlViewCmdExecutor(session, viewId);
    }

	return NULL;
}

bool QQmlViewCmdExecutorCreator::CanHandleView(Session* session, ViewId viewId, ViewType* viewType) const {
    QDeclarativeView* pView = QQmlViewUtil::getQMLView(session, viewId);

    if (NULL != pView) {
        if (NULL != viewType) *viewType = QML_VIEW_TYPE;
        return true;
    }

    return false;
}

std::string QQmlViewCmdExecutorCreator::GetViewTypeName() const {
    return "qml";
}

QQmlViewCmdExecutor::QQmlViewCmdExecutor(Session* session, ViewId viewId)
	: QViewCmdExecutor(session, viewId) {
}

QQmlViewCmdExecutor::~QQmlViewCmdExecutor() {}

QDeclarativeView* QQmlViewCmdExecutor::getView(const ViewId& viewId, Error** error) {
    QDeclarativeView* pView = QQmlViewUtil::getQMLView(session_, viewId);

    if (NULL == pView) {
        session_->logger().Log(kWarningLogLevel, "checkView - no such qml view("+viewId.id()+")");
        *error = new Error(kNoSuchWindow);
        return NULL;
    }

    return pView;
}   

QDeclarativeItem* QQmlViewCmdExecutor::getElement(const ElementId &element, Error** error) {
    QElementHandle* element_handle = dynamic_cast<QElementHandle*>(session_->GetElementHandle(view_id_, element));

    if (NULL == element_handle) {
        *error = new Error(kNoSuchElement);
        return NULL;
    }

    if (!element_handle->is_valid()) {
        *error = new Error(kStaleElementReference);
        return NULL;
    }

    QDeclarativeItem* retObj = qobject_cast<QDeclarativeItem*>(element_handle->get());
    if (NULL == retObj) {
        *error = new Error(kUnknownError, "canot cast element to QDeclarativeItem.");
        return NULL;
    }

    return retObj;
}

void QQmlViewCmdExecutor::CanHandleUrl(const std::string& url, bool* can, Error **error) {
	*can = QQmlViewUtil::isUrlSupported(url);
}

void QQmlViewCmdExecutor::GetSource(std::string* source, Error** error) {
    QDeclarativeView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QDeclarativeItem* parentItem = qobject_cast<QDeclarativeItem*>(view->rootObject());
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

void QQmlViewCmdExecutor::SendKeys(const ElementId& element, const string16& keys, Error** error) {
    QDeclarativeView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QDeclarativeItem* pItem = getElement(element, error);
    if (NULL == pItem)
        return;

    QDeclarativeItem* pFocusItem = qobject_cast<QDeclarativeItem*>(view->scene()->focusItem());

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

    // set focus to element
    pItem->setFocus(true);
    if (!pItem->hasFocus()) {
        // restore old focus
        if (NULL != pFocusItem) pFocusItem->setFocus(true);        

        *error = new Error(kInvalidElementState);
        return;
    }

    std::vector<QKeyEvent>::iterator it = key_events.begin();
    while (it != key_events.end()) {
        qApp->sendEvent(view, &(*it));
        ++it;
    }

    // restore old focus
    if (NULL != pFocusItem)
        pFocusItem->setFocus(true);
}

void QQmlViewCmdExecutor::GetElementScreenShot(const ElementId& element, std::string* png, Error** error) {
    QDeclarativeView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QDeclarativeItem* pItem = getElement(element, error);
    if (NULL == pItem)
        return;

    QImage image(pItem->boundingRect().size().toSize(), QImage::Format_RGB32);
    image.fill(QColor(0, 0, 0).rgb());
    QPainter painter(&image);
    QStyleOptionGraphicsItem styleOption;
    qobject_cast<QGraphicsObject*>(pItem)->paint(&painter, &styleOption);
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

void QQmlViewCmdExecutor::MouseDoubleClick(Error** error) {
    QDeclarativeView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QPoint point = QCommonUtil::ConvertPointToQPoint(session_->get_mouse_position());
    QPointF scenePoint(point.x(), point.y());
    QPoint viewPos = view->mapFromScene(scenePoint);

    session_->logger().Log(kFineLogLevel, base::StringPrintf("MouseDblClick,  scene: (%4d, %4d)", (int)scenePoint.x(), (int)scenePoint.y()));
    session_->logger().Log(kFineLogLevel, base::StringPrintf("MouseDblClick,   view: (%4d, %4d)", viewPos.x(), viewPos.y()));

    QPoint screenPos = view->mapToGlobal(view->mapFromScene(scenePoint));

    session_->logger().Log(kFineLogLevel, base::StringPrintf("MouseDblClick, screen: (%4d, %4d)", screenPos.x(), screenPos.y()));

    QMouseEvent *dbEvent = new QMouseEvent(QEvent::MouseButtonDblClick, viewPos, screenPos, Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
    QMouseEvent *releaseEvent = new QMouseEvent(QEvent::MouseButtonRelease, viewPos, screenPos, Qt::LeftButton, Qt::NoButton, Qt::NoModifier);

    QApplication::postEvent(view->viewport(), dbEvent);
    QApplication::postEvent(view->viewport(), releaseEvent);
}

void QQmlViewCmdExecutor::MouseButtonUp(Error** error) {
    QDeclarativeView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QPoint point = QCommonUtil::ConvertPointToQPoint(session_->get_mouse_position());
    QPointF scenePoint(point.x(), point.y());
    QPoint viewPos = view->mapFromScene(scenePoint);

    session_->logger().Log(kFineLogLevel, base::StringPrintf("MouseUp,  scene: (%4d, %4d)", (int)scenePoint.x(), (int)scenePoint.y()));
    session_->logger().Log(kFineLogLevel, base::StringPrintf("MouseUp,   view: (%4d, %4d)", viewPos.x(), viewPos.y()));

    QPoint screenPos = view->mapToGlobal(view->mapFromScene(scenePoint));

    session_->logger().Log(kFineLogLevel, base::StringPrintf("MouseUp, screen: (%4d, %4d)", screenPos.x(), screenPos.y()));

    QMouseEvent *releaseEvent = new QMouseEvent(QEvent::MouseButtonRelease, viewPos, screenPos, Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
    QApplication::postEvent(view->viewport(), releaseEvent);
}

void QQmlViewCmdExecutor::MouseButtonDown(Error** error) {
    QDeclarativeView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QPoint point = QCommonUtil::ConvertPointToQPoint(session_->get_mouse_position());
    QPointF scenePoint(point.x(), point.y());
    QPoint viewPos = view->mapFromScene(scenePoint);

    session_->logger().Log(kFineLogLevel, base::StringPrintf("MouseDown,  scene: (%4d, %4d)", (int)scenePoint.x(), (int)scenePoint.y()));
    session_->logger().Log(kFineLogLevel, base::StringPrintf("MouseDown,   view: (%4d, %4d)", viewPos.x(), viewPos.y()));

    QPoint screenPos = view->mapToGlobal(view->mapFromScene(scenePoint));

    session_->logger().Log(kFineLogLevel, base::StringPrintf("MouseDown, screen: (%4d, %4d)", screenPos.x(), screenPos.y()));

    QMouseEvent *pressEvent = new QMouseEvent(QEvent::MouseButtonPress, viewPos, screenPos, Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
    QApplication::postEvent(view->viewport(), pressEvent);
}

void QQmlViewCmdExecutor::MouseClick(MouseButton button, Error** error) {
    QDeclarativeView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QPoint point = QCommonUtil::ConvertPointToQPoint(session_->get_mouse_position());
    QPointF scenePoint(point.x(), point.y());
    QPoint viewPos = view->mapFromScene(scenePoint);

    session_->logger().Log(kFineLogLevel, base::StringPrintf("MouseClick,  scene: (%4d, %4d)", (int)scenePoint.x(), (int)scenePoint.y()));
    session_->logger().Log(kFineLogLevel, base::StringPrintf("MouseClick,   view: (%4d, %4d)", viewPos.x(), viewPos.y()));

    QPoint screenPos = view->mapToGlobal(view->mapFromScene(scenePoint));

    session_->logger().Log(kFineLogLevel, base::StringPrintf("MouseClick, screen: (%4d, %4d)", screenPos.x(), screenPos.y()));

    Qt::MouseButton mouseButton = QCommonUtil::ConvertMouseButtonToQtMouseButton(button);
    QMouseEvent *pressEvent = new QMouseEvent(QEvent::MouseButtonPress, viewPos, screenPos, mouseButton, Qt::NoButton, Qt::NoModifier);
    QMouseEvent *releaseEvent = new QMouseEvent(QEvent::MouseButtonRelease, viewPos, screenPos, mouseButton, Qt::NoButton, Qt::NoModifier);

    QApplication::postEvent(view->viewport(), pressEvent);
    QApplication::postEvent(view->viewport(), releaseEvent);
}

void QQmlViewCmdExecutor::MouseWheel(const int delta, Error **error) {
    QDeclarativeView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QPoint point = QCommonUtil::ConvertPointToQPoint(session_->get_mouse_position());
    QPointF scenePoint(point.x(), point.y());
    QPoint viewPos = view->mapFromScene(scenePoint);

    session_->logger().Log(kFineLogLevel, base::StringPrintf("MouseWheel,  scene: (%4d, %4d)", (int)scenePoint.x(), (int)scenePoint.y()));
    session_->logger().Log(kFineLogLevel, base::StringPrintf("MouseWheel,   view: (%4d, %4d)", viewPos.x(), viewPos.y()));

    QPoint screenPos = view->mapToGlobal(viewPos);

    session_->logger().Log(kFineLogLevel, base::StringPrintf("MouseWheel, screen: (%4d, %4d)", screenPos.x(), screenPos.y()));

    QWheelEvent *wheelEvent = new QWheelEvent(viewPos, screenPos, delta, Qt::NoButton, Qt::NoModifier);

    QApplication::postEvent(view->viewport(), wheelEvent);
}

void QQmlViewCmdExecutor::MouseMove(const int x_offset, const int y_offset, Error** error) {
	QDeclarativeView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    Point prev_pos = session_->get_mouse_position();
    prev_pos.Offset(x_offset, y_offset);

	QPoint point = QCommonUtil::ConvertPointToQPoint(prev_pos);
    QPointF scenePoint(point.x(), point.y());

    if (!view->sceneRect().contains(scenePoint)) {
        *error = new Error(kMoveTargetOutOfBounds);
        return;
    }

    moveMouseInternal(view, scenePoint);

    session_->set_mouse_position(prev_pos);
}

void QQmlViewCmdExecutor::MouseMove(const ElementId& element, int x_offset, const int y_offset, Error** error) {
    QDeclarativeView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QDeclarativeItem* pItem = getElement(element, error);
    if (NULL == pItem)
        return;

    QPointF scenePoint = pItem->mapToScene(x_offset, y_offset);

    if (!view->sceneRect().contains(scenePoint)) {
        *error = new Error(kMoveTargetOutOfBounds);
        return;
    }

    moveMouseInternal(view, scenePoint);

    session_->set_mouse_position(Point(scenePoint.x(), scenePoint.y()));
}

void QQmlViewCmdExecutor::MouseMove(const ElementId& element, Error** error) {
    QDeclarativeView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QDeclarativeItem* pItem = getElement(element, error);
    if (NULL == pItem)
        return;

    QPointF scenePoint = pItem->mapToScene(0, 0);

    if (!view->sceneRect().contains(scenePoint)) {
        *error = new Error(kMoveTargetOutOfBounds);
        return;
    }

    moveMouseInternal(view, scenePoint);

    session_->set_mouse_position(Point(scenePoint.x(), scenePoint.y()));
}

void QQmlViewCmdExecutor::moveMouseInternal(QDeclarativeView* view, QPointF& point) {
    QPoint startScenePos = QCommonUtil::ConvertPointToQPoint(session_->get_mouse_position());
    QPoint startViewPos = view->mapFromScene(startScenePos);
    QPoint targetViewPos = view->mapFromScene(point);
    QPoint targetScreenPos = view->mapToGlobal(targetViewPos);

    Qt::MouseButton mouseButton = (view->scene()->mouseGrabberItem())?(Qt::LeftButton):(Qt::NoButton);

    QPointF minDragVector(QApplication::startDragDistance(), QApplication::startDragDistance());
    qreal dragThreshold = minDragVector.manhattanLength();
    QLineF line(startViewPos.x(), startViewPos.y(), targetViewPos.x(), targetViewPos.y());

    if ( line.length() > dragThreshold ) {
        // we need first mousemove event to initiate drag
        QPointF dragStartPointF = line.pointAt(dragThreshold / line.length());
        QPoint dragStartPoint((int)dragStartPointF.x(), (int)dragStartPointF.y());
        QPoint dragStartScreenPos = view->mapToGlobal(dragStartPoint);

        QMouseEvent *moveEvent = new QMouseEvent(QEvent::MouseMove, dragStartPoint, dragStartScreenPos, Qt::NoButton, mouseButton, Qt::NoModifier);
        QApplication::postEvent(view->viewport(), moveEvent);    
    }

    session_->logger().Log(kFineLogLevel, base::StringPrintf("MouseMove,  scene: (%4d, %4d)", (int)point.x(), (int)point.y()));
    session_->logger().Log(kFineLogLevel, base::StringPrintf("MouseMove,   view: (%4d, %4d)", targetViewPos.x(), targetViewPos.y()));
    session_->logger().Log(kFineLogLevel, base::StringPrintf("MouseMove, screen: (%4d, %4d)", targetScreenPos.x(), targetScreenPos.y()));

    QMouseEvent *moveEvent = new QMouseEvent(QEvent::MouseMove, targetViewPos, targetScreenPos, Qt::NoButton, mouseButton, Qt::NoModifier);
    QApplication::postEvent(view->viewport(), moveEvent);    
}

void QQmlViewCmdExecutor::ClickElement(const ElementId& element, Error** error) {
	QDeclarativeView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QDeclarativeItem* pItem = getElement(element, error);
    if (NULL == pItem)
        return;

    if (!pItem->isVisible()) {
        *error = new Error(kElementNotVisible);
        return;
    }

    session_->logger().Log(kFineLogLevel, "Click on ");
    session_->logger().Log(kFineLogLevel, pItem->objectName().toStdString());

    //QRectF sceneRect = pItem->mapRectToScene(pItem->boundingRect());
    QPointF scenePoint = pItem->mapToScene(0, 0);

    if (!view->sceneRect().contains(scenePoint)) {
        *error = new Error(kMoveTargetOutOfBounds);
        return;
    }

    QGraphicsSceneMouseEvent *pressEvent = new QGraphicsSceneMouseEvent(QEvent::GraphicsSceneMousePress);
    pressEvent->setScenePos(scenePoint);
    pressEvent->setButton(Qt::LeftButton);
    pressEvent->setButtons(Qt::LeftButton);

    QGraphicsSceneMouseEvent *releaseEvent = new QGraphicsSceneMouseEvent(QEvent::GraphicsSceneMouseRelease);
    releaseEvent->setScenePos(scenePoint);
    releaseEvent->setButton(Qt::LeftButton);
    releaseEvent->setButtons(Qt::NoButton);

    QApplication::postEvent(view->scene(), pressEvent);
    QApplication::postEvent(view->scene(), releaseEvent);
}

void QQmlViewCmdExecutor::GetAttribute(const ElementId& element, const std::string& key, base::Value** value, Error** error) {
	QDeclarativeView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QDeclarativeItem* pItem = getElement(element, error);
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

void QQmlViewCmdExecutor::ClearElement(const ElementId& element, Error** error) {
    QDeclarativeView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QDeclarativeItem* pItem = getElement(element, error);
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

void QQmlViewCmdExecutor::IsElementDisplayed(const ElementId& element, bool ignore_opacity, bool* is_displayed, Error** error) {
	QDeclarativeView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QDeclarativeItem* pItem = getElement(element, error);
    if (NULL == pItem)
        return;

    *is_displayed = pItem->isVisible();
}

void QQmlViewCmdExecutor::IsElementEnabled(const ElementId& element, bool* is_enabled, Error** error) {
    QDeclarativeView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QDeclarativeItem* pItem = getElement(element, error);
    if (NULL == pItem)
        return;

    *is_enabled = pItem->isEnabled();
}

void QQmlViewCmdExecutor::ElementEquals(const ElementId& element1, const ElementId& element2, bool* is_equal, Error** error) {
	QDeclarativeView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QDeclarativeItem* pItem1 = getElement(element1, error);
    if (NULL == pItem1)
        return;

    QDeclarativeItem* pItem2 = getElement(element2, error);
    if (NULL == pItem2)
        return;

    *is_equal = (pItem1 == pItem2);
}

void QQmlViewCmdExecutor::GetElementLocation(const ElementId& element, Point* location, Error** error) {
	QDeclarativeView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QDeclarativeItem* pItem = getElement(element, error);
    if (NULL == pItem)
        return;

    QPointF scenePos = pItem->scenePos();

    *location = Point(scenePos.x(), scenePos.y());
}

void QQmlViewCmdExecutor::GetElementLocationInView(const ElementId& element, Point* location, Error** error) {
	QDeclarativeView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QDeclarativeItem* pItem = getElement(element, error);
    if (NULL == pItem)
        return;

    QPointF scenePos = pItem->scenePos();
    QPoint pos = view->mapFromScene(scenePos);

    // TODO: need check if pos fits into viewport?    
    *location = Point(pos.x(), pos.y());
}

void QQmlViewCmdExecutor::GetElementTagName(const ElementId& element, std::string* tag_name, Error** error) {
    QDeclarativeView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QDeclarativeItem* pItem = getElement(element, error);
    if (NULL == pItem)
        return;

    QString className(pItem->metaObject()->className());
    QQmlViewUtil::removeInternalSuffixes(className);

    *tag_name = className.toStdString();
}

void QQmlViewCmdExecutor::GetElementSize(const ElementId& element, Size* size, Error** error) {
	QDeclarativeView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QDeclarativeItem* pItem = getElement(element, error);
    if (NULL == pItem)
        return;

    *size = Size(pItem->width(), pItem->height());
}

void QQmlViewCmdExecutor::GetElementText(const ElementId& element, std::string* element_text, Error** error) {
    QDeclarativeView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QDeclarativeItem* pItem = getElement(element, error);
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

void QQmlViewCmdExecutor::FindElements(const ElementId& root_element, const std::string& locator, const std::string& query, std::vector<ElementId>* elements, Error** error) {
	QDeclarativeView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    session_->logger().Log(kFineLogLevel, "FindElements, loc:"+locator+" query:"+query);

    Error* tmp_error = NULL;
    QDeclarativeItem *parentItem = getElement(root_element, &tmp_error);
    scoped_ptr<Error> scoped_err(tmp_error);

    if (NULL == parentItem) {
        session_->logger().Log(kInfoLogLevel, "no root element specified, search from root.");
        parentItem = qobject_cast<QDeclarativeItem*>(view->rootObject());;
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

void QQmlViewCmdExecutor::FindElements(QDeclarativeItem* parent, const std::string& locator, const std::string& query, std::vector<ElementId>* elements, Error** error) {
    if (FilterElement(parent, locator, query)) {
        ElementId elm;
        session_->AddElement(view_id_, new QElementHandle(parent), &elm);
        (*elements).push_back(elm);

        session_->logger().Log(kFineLogLevel, "element found: "+elm.id());
    }

    QList<QGraphicsItem*> childs = parent->childItems();
    foreach(QGraphicsItem *child, childs) {
        QDeclarativeItem* childItem = qobject_cast<QDeclarativeItem*>(child);
        if (childItem)
            FindElements(childItem, locator, query, elements, error);
    }
}

void QQmlViewCmdExecutor::ActiveElement(ElementId* element, Error** error) {
	QDeclarativeView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QDeclarativeItem* pFocusItem = qobject_cast<QDeclarativeItem*>(view->scene()->focusItem());
    if (NULL == pFocusItem) {
        *error = new Error(kNoSuchElement);
        return;
    }

    ElementId active_element;

    session_->AddElement(view_id_, new QElementHandle(pFocusItem), &active_element);

    *element = active_element;
}

void QQmlViewCmdExecutor::NavigateToURL(const std::string& url, bool sync, Error** error) {
	QDeclarativeView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    session_->logger().Log(kFineLogLevel, "QML - NavigateToURL" + url);

    QUrl address(QString(url.c_str()));

    if (sync) {
        QEventLoop loop;
        QObject::connect(view, SIGNAL(statusChanged(QDeclarativeView::Status)),&loop,SLOT(quit()));
        view->setSource(address);

        if (QDeclarativeView::Loading == view->status()) {
            loop.exec();
        }

        if (QDeclarativeView::Ready != view->status()) {
            session_->logger().Log(kWarningLogLevel, "QML sync load, smth wrong. View is not in READY state.");
        }

        session_->logger().Log(kFineLogLevel, "QML sync load - " + url);
    } else {
        view->setSource(address);

        session_->logger().Log(kFineLogLevel, "QML async load - " + url);
    }
}

void QQmlViewCmdExecutor::GetURL(std::string* url, Error** error) {
	QDeclarativeView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    *url = view->source().toString().toStdString();
}

void QQmlViewCmdExecutor::ExecuteScript(const std::string& script, const base::ListValue* const args, base::Value** value, Error** error) {
    QDeclarativeView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    std::string args_as_json;
    base::JSONWriter::Write(static_cast<const Value* const>(args), &args_as_json);

    std::string jscript = base::StringPrintf(
        "(function(x) { %s }.apply(this, %s));",
        script.c_str(),
        args_as_json.c_str());

    QDeclarativeContext *rootContext = view->rootContext();
    QVariant p = rootContext->contextProperty("ObjectNameUtils");

    if (p.isNull()) {
        QDeclarativeItem *rootItem = qobject_cast<QDeclarativeItem*>(view->rootObject());
        ObjectNameUtils* objn = new ObjectNameUtils(rootItem);
        rootContext->setContextProperty("ObjectNameUtils", objn);
    }

    QDeclarativeExpression expr(rootContext, view->rootObject(), jscript.c_str());
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

void QQmlViewCmdExecutor::VisualizerSource(std::string* source, Error** error)
{
    QDeclarativeView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QDeclarativeItem* parentItem = qobject_cast<QDeclarativeItem*>(view->rootObject());
    if (NULL == parentItem) {
        session_->logger().Log(kInfoLogLevel, "no root element found.");
        *error = new Error(kUnknownError, "no root element found.");
        return;
    }

    QByteArray byteArray;
    QBuffer buff(&byteArray);
    buff.open(QIODevice::ReadWrite);

    QQmlXmlSerializer serializer(&buff);
    serializer.setDumpAll(true);
    serializer.createXml(parentItem);
    *source = byteArray.data();

    session_->logger().Log(kInfoLogLevel, "VisualizerSource:");
    session_->logger().Log(kInfoLogLevel, *source);
}

bool QQmlViewCmdExecutor::FilterElement(const QDeclarativeItem* item, const std::string& locator, const std::string& query) {
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

void QQmlViewCmdExecutor::FindElementsByXpath(QDeclarativeItem* parent, const std::string &query, std::vector<ElementId>* elements, Error **error) {
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
