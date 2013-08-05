#include "extension_qt/qml_view_executor.h"

#include "base/stringprintf.h"
#include "base/string_number_conversions.h"

#include "value_conversion_util.h"
#include "webdriver_session.h"
#include "webdriver_view_factory.h"
#include "webdriver_util.h"
#include "q_key_converter.h"
#include "extension_qt/widget_element_handle.h"
#include "extension_qt/widget_view_handle.h"
#include "qml_view_util.h"

#include <QtCore/QBuffer>
#include <QtCore/QDebug>
#include <QtGui/QApplication>

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

    XMLElementMap elementsMap;
    QByteArray byteArray;
    QBuffer buff(&byteArray);
    buff.open(QIODevice::ReadWrite);

    createUIXML(parentItem, &buff, elementsMap, error);

    if (*error)
        return;

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

void QQmlViewCmdExecutor::MouseDoubleClick(Error** error) {
    QDeclarativeView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QPoint point = ConvertPointToQPoint(session_->get_mouse_position());
    QPointF scenePoint(point.x(), point.y());

    QGraphicsSceneMouseEvent *dbClckEvent = new QGraphicsSceneMouseEvent(QEvent::GraphicsSceneMouseDoubleClick);
    dbClckEvent->setScenePos(scenePoint);
    dbClckEvent->setButton(Qt::LeftButton);
    dbClckEvent->setButtons(Qt::LeftButton);

    QGraphicsSceneMouseEvent *releaseEvent = new QGraphicsSceneMouseEvent(QEvent::GraphicsSceneMouseRelease);
    releaseEvent->setScenePos(scenePoint);
    releaseEvent->setButton(Qt::LeftButton);
    releaseEvent->setButtons(Qt::LeftButton);

    QApplication::postEvent(view->scene(), dbClckEvent);
    QApplication::postEvent(view->scene(), releaseEvent);
}

void QQmlViewCmdExecutor::MouseButtonUp(Error** error) {
    QDeclarativeView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QPoint point = ConvertPointToQPoint(session_->get_mouse_position());
    QPointF scenePoint(point.x(), point.y());

    QGraphicsSceneMouseEvent *releaseEvent = new QGraphicsSceneMouseEvent(QEvent::GraphicsSceneMouseRelease);
    releaseEvent->setScenePos(scenePoint);
    releaseEvent->setButton(Qt::LeftButton);
    releaseEvent->setButtons(Qt::LeftButton);

    QApplication::postEvent(view->scene(), releaseEvent);
}

void QQmlViewCmdExecutor::MouseButtonDown(Error** error) {
    QDeclarativeView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QPoint point = ConvertPointToQPoint(session_->get_mouse_position());
    QPointF scenePoint(point.x(), point.y());

    QGraphicsSceneMouseEvent *pressEvent = new QGraphicsSceneMouseEvent(QEvent::GraphicsSceneMousePress);
    pressEvent->setScenePos(scenePoint);
    pressEvent->setButton(Qt::LeftButton);
    pressEvent->setButtons(Qt::LeftButton);

    QApplication::postEvent(view->scene(), pressEvent);
}

void QQmlViewCmdExecutor::MouseClick(MouseButton button, Error** error) {
    QDeclarativeView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QPoint point = ConvertPointToQPoint(session_->get_mouse_position());
    QPointF scenePoint(point.x(), point.y());

    Qt::MouseButton mouseButton = ConvertMouseButtonToQtMouseButton(button);

    QGraphicsSceneMouseEvent *pressEvent = new QGraphicsSceneMouseEvent(QEvent::GraphicsSceneMousePress);
    pressEvent->setScenePos(scenePoint);
    pressEvent->setButton(mouseButton);
    pressEvent->setButtons(mouseButton);

    QGraphicsSceneMouseEvent *releaseEvent = new QGraphicsSceneMouseEvent(QEvent::GraphicsSceneMouseRelease);
    releaseEvent->setScenePos(scenePoint);
    releaseEvent->setButton(mouseButton);
    releaseEvent->setButtons(mouseButton);

    QApplication::postEvent(view->scene(), pressEvent);
    QApplication::postEvent(view->scene(), releaseEvent);
//    if (Qt::RightButton == mouseButton) {
//        QContextMenuEvent *contextEvent = new QContextMenuEvent(QContextMenuEvent::Mouse, point);
//        QApplication::postEvent(view->scene(), contextEvent);
//    }
}

void QQmlViewCmdExecutor::MouseMove(const int x_offset, const int y_offset, Error** error) {
	QDeclarativeView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    Point prev_pos = session_->get_mouse_position();
    prev_pos.Offset(x_offset, y_offset);

	QPoint point = ConvertPointToQPoint(prev_pos);
    QPointF scenePoint(point.x(), point.y());

    if (!view->sceneRect().contains(scenePoint)) {
        *error = new Error(kMoveTargetOutOfBounds);
        return;
    }

    QGraphicsSceneMouseEvent *moveEvent = new QGraphicsSceneMouseEvent(QEvent::GraphicsSceneMouseMove);
    moveEvent->setScenePos(scenePoint);
    QApplication::postEvent(view->scene(), moveEvent);

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

    QGraphicsSceneMouseEvent *moveEvent = new QGraphicsSceneMouseEvent(QEvent::GraphicsSceneMouseMove);
    moveEvent->setScenePos(scenePoint);
    QApplication::postEvent(view->scene(), moveEvent);
    
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

    QGraphicsSceneMouseEvent *moveEvent = new QGraphicsSceneMouseEvent(QEvent::GraphicsSceneMouseMove);
    moveEvent->setScenePos(scenePoint);
    QApplication::postEvent(view->scene(), moveEvent);
    
    session_->set_mouse_position(Point(scenePoint.x(), scenePoint.y()));
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
    releaseEvent->setButtons(Qt::LeftButton);

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
    Value* val = NULL;

    if (propertyValue.isValid()) {
        // convert QVariant to base::Value
        switch (propertyValue.type()) {
        case QVariant::Bool:
            val = Value::CreateBooleanValue(propertyValue.toBool());
            break;
        case QVariant::Int:
            val = Value::CreateIntegerValue(propertyValue.toInt());
            break;
        case QVariant::Double:
            val = Value::CreateDoubleValue(propertyValue.toDouble());
            break;
        case QVariant::String:
            val = Value::CreateStringValue(propertyValue.toString().toStdString());
            break;
        default:
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

    *tag_name = pItem->metaObject()->className();
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

    if (locator == LocatorType::kXpath) {
        // TODO:
        // FindNativeElementsByXpath(parentWidget, query, elements, error);
    } else {
        // process root        
        if (FilterElement(parentItem, locator, query)) {
            ElementId elm;
            session_->AddElement(view_id_, new QElementHandle(parentItem), &elm);
            (*elements).push_back(elm);

            session_->logger().Log(kFineLogLevel, "element found: "+elm.id());
        }

        // list all child items and find matched locator
        QList<QDeclarativeItem*> childs = parentItem->findChildren<QDeclarativeItem*>();
        foreach(QDeclarativeItem *child, childs) {
            //qDebug() << "-----------------";
            //qDebug() << "className: " << child->metaObject()->className();
            //qDebug() << "objectName: " << child->objectName();
            //qDebug() << "prop id: " << child->property("id");
            //qDebug() << "prop name: " << child->property("name");
    
            if (FilterElement(child, locator, query)) {
                ElementId elm;
                session_->AddElement(view_id_, new QElementHandle(child), &elm);
                (*elements).push_back(elm);

                session_->logger().Log(kFineLogLevel, "element found: "+elm.id());
            }
        }
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
        QObject::connect(view, SIGNAL(statusChanged()),&loop,SLOT(quit()));
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

bool QQmlViewCmdExecutor::FilterElement(const QDeclarativeItem* item, const std::string& locator, const std::string& query) {
    if (locator == LocatorType::kClassName) {
        if (query == item->metaObject()->className())
            return true;
    } else if (locator == LocatorType::kTagName) {
        if (query == item->metaObject()->className())
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

/*
void QQmlViewCmdExecutor::FindNativeElementsByXpath(QWidget* parent, const std::string &query, std::vector<ElementId>* elements, Error **error) {
    
#ifndef WD_CONFIG_XPATH
    *error = new Error(kXPathLookupError, "Finding elements by xpath is not supported");
    return;
#else
    QXmlResultItems result;
    QByteArray byteArray;
    QBuffer buff(&byteArray);

    buff.open(QIODevice::ReadWrite);
    XMLElementMap elementsMap;
    createUIXML(parent, &buff, elementsMap, error, false);
    if (*error)
        return;

    buff.seek(0);
    QXmlQuery xmlquery;
    xmlquery.bindVariable("buff", &buff);
    xmlquery.setQuery("doc($buff) "+QString(query.c_str()));

    if (!xmlquery.isValid()) {
        *error = new Error(kXPathLookupError);
        return;
    }
    xmlquery.evaluateTo(&result);

    buff.close();
    QXmlItem item(result.next());
    while (!item.isNull()) {
        if (item.isNode()) {
            xmlquery.setFocus(item);
            xmlquery.setQuery("./@elementId/string()");
            if (!xmlquery.isValid()) {
                *error = new Error(kXPathLookupError);
                return;
            }

            QString elementId;
            xmlquery.evaluateTo(&elementId);
            if (!elementId.isEmpty()) {
                // TODO: what this? why do we need to remove EOL?
                elementId.remove('\n');

                if (elementsMap.contains(elementId)) {
                    ElementId elm;
                    session_->AddElement(view_id_, new QElementHandle(elementsMap[elementId]), &elm);
                    (*elements).push_back(elm);
                    session_->logger().Log(kFineLogLevel, "element found: "+elm.id());
                } else {
                    session_->logger().Log(kSevereLogLevel, "cant get element from map, skipped");
                }
            }
        }
        item = result.next();
    }
    if (elements->empty())
    {
        return;
    }
#endif

}
*/

void QQmlViewCmdExecutor::createUIXML(QDeclarativeItem *parent, QIODevice* buff, XMLElementMap& elementsMap, Error** error) {
    
    QXmlStreamWriter* writer = new QXmlStreamWriter();

    writer->setDevice(buff);
    writer->setAutoFormatting(true);
    writer->writeStartDocument();

    addItemToXML(parent, elementsMap, writer);

    writer->writeEndDocument();

    delete writer;
}

void QQmlViewCmdExecutor::addItemToXML(QDeclarativeItem* parent, XMLElementMap& elementsMap, QXmlStreamWriter* writer) {
    
    writer->writeStartElement(parent->metaObject()->className());

    if (!parent->objectName().isEmpty())
        writer->writeAttribute("id", parent->objectName());

    QString elementKey = GenerateRandomID().c_str();
    elementsMap.insert(elementKey, QPointer<QDeclarativeItem>(parent));
    writer->writeAttribute("elementId", elementKey);

    QList<QObject*> childs = parent->children();
    foreach(QObject *child, childs) {
        QDeclarativeItem* childItem = qobject_cast<QDeclarativeItem*>(child);
        if (childItem)
            addItemToXML(childItem, elementsMap, writer);
    }

    writer->writeEndElement();
}


} //namespace webdriver 
