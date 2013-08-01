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

#ifdef WD_CONFIG_XPATH
#include <QtXmlPatterns/QXmlQuery>
#include <QtXmlPatterns/QXmlResultItems>
#endif

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
/*	
    QWidget* view = getView(view_id_, error);
    if (NULL == view)
        return;

    XMLElementMap elementsMap;
    QByteArray byteArray;
    QBuffer buff(&byteArray);
    buff.open(QIODevice::ReadWrite);
    createUIXML(view, &buff, elementsMap, error, true);

    if (*error)
        return;

    *source = byteArray.data();
*/
}

void QQmlViewCmdExecutor::SendKeys(const ElementId& element, const string16& keys, Error** error) {
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
        qApp->sendEvent(view, &(*it));
        ++it;
    }

    {
        view->rotate(45);
    }
}

void QQmlViewCmdExecutor::MouseDoubleClick(Error** error) {
/*    
	QWidget* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QPoint point = ConvertPointToQPoint(session_->get_mouse_position());

    // Find child widget that will receive event
    QWidget *receiverWidget = view->childAt(point);
    if (NULL != receiverWidget) {
        point = receiverWidget->mapFrom(view, point);
    } else {
        receiverWidget = view;
    }

    QMouseEvent *dbEvent = new QMouseEvent(QEvent::MouseButtonDblClick, point, Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
    QMouseEvent *releaseEvent = new QMouseEvent(QEvent::MouseButtonRelease, point, Qt::LeftButton, Qt::NoButton, Qt::NoModifier);

    QApplication::postEvent(receiverWidget, dbEvent);
    QApplication::postEvent(receiverWidget, releaseEvent);
*/
}

void QQmlViewCmdExecutor::MouseButtonUp(Error** error) {
/*    
	QWidget* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QPoint point = ConvertPointToQPoint(session_->get_mouse_position());

    // Find child widget that will receive event
    QWidget *receiverWidget = view->childAt(point);
    if (NULL != receiverWidget) {
        point = receiverWidget->mapFrom(view, point);
    } else {
        receiverWidget = view;
    }

    QMouseEvent *releaseEvent = new QMouseEvent(QEvent::MouseButtonRelease, point, Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
    QApplication::postEvent(receiverWidget, releaseEvent);
*/    
}

void QQmlViewCmdExecutor::MouseButtonDown(Error** error) {
/*    
	QWidget* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QPoint point = ConvertPointToQPoint(session_->get_mouse_position());

    // Find child widget that will receive event
    QWidget *receiverWidget = view->childAt(point);
    if (NULL != receiverWidget) {
        point = receiverWidget->mapFrom(view, point);
    } else {
        receiverWidget = view;
    }

    QMouseEvent *pressEvent = new QMouseEvent(QEvent::MouseButtonPress, point, Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
    QApplication::sendEvent(receiverWidget, pressEvent);
*/    
}

void QQmlViewCmdExecutor::MouseClick(MouseButton button, Error** error) {
/*    
	QWidget* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QPoint point = ConvertPointToQPoint(session_->get_mouse_position());

    // Find child widget that will receive event
    QWidget *receiverWidget = view->childAt(point);
    if (NULL != receiverWidget) {
        point = receiverWidget->mapFrom(view, point);
    } else {
        receiverWidget = view;
    }

    Qt::MouseButton mouseButton = ConvertMouseButtonToQtMouseButton(button);
    QMouseEvent *pressEvent = new QMouseEvent(QEvent::MouseButtonPress, point, mouseButton, Qt::NoButton, Qt::NoModifier);
    QMouseEvent *releaseEvent = new QMouseEvent(QEvent::MouseButtonRelease, point, mouseButton, Qt::NoButton, Qt::NoModifier);

    QApplication::postEvent(receiverWidget, pressEvent);
    QApplication::postEvent(receiverWidget, releaseEvent);
    if (Qt::RightButton == mouseButton) {
        QContextMenuEvent *contextEvent = new QContextMenuEvent(QContextMenuEvent::Mouse, point);
        QApplication::postEvent(receiverWidget, contextEvent);
    }
*/    
}

void QQmlViewCmdExecutor::MouseMove(const int x_offset, const int y_offset, Error** error) {
/*    
	QWidget* view = getView(view_id_, error);
    if (NULL == view)
        return;

    Point prev_pos = session_->get_mouse_position();
    prev_pos.Offset(x_offset, y_offset);

	QPoint point = ConvertPointToQPoint(prev_pos);

    // Find child widget that will receive event
    QWidget *receiverWidget = view->childAt(point);
    if (NULL != receiverWidget) {
        point = receiverWidget->mapFrom(view, point);
    } else {
        receiverWidget = view;
    }

    QMouseEvent *moveEvent = new QMouseEvent(QEvent::MouseMove, point, Qt::NoButton, Qt::NoButton, Qt::NoModifier);
    QApplication::postEvent(receiverWidget, moveEvent);

    session_->set_mouse_position(prev_pos);
*/    
}

void QQmlViewCmdExecutor::MouseMove(const ElementId& element, int x_offset, const int y_offset, Error** error) {
/*    
	QWidget* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QWidget* pWidget = getElement(element, error);
    if (NULL == pWidget)
        return;

    QPoint pos = pWidget->mapTo(view, QPoint(0, 0));
    Point location = Point(pos.x(), pos.y());
    location.Offset(x_offset, y_offset);

    QPoint point = ConvertPointToQPoint(location);

    // Find child widget that will receive event
    QWidget *receiverWidget = view->childAt(point);
    if (NULL != receiverWidget) {
        point = receiverWidget->mapFrom(view, point);
    } else {
        receiverWidget = view;
    }

    QMouseEvent *moveEvent = new QMouseEvent(QEvent::MouseMove, point, Qt::NoButton, Qt::NoButton, Qt::NoModifier);
    QApplication::postEvent(receiverWidget, moveEvent);

    session_->set_mouse_position(location);
*/    
}

void QQmlViewCmdExecutor::MouseMove(const ElementId& element, Error** error) {
/*    
	QWidget* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QWidget* pWidget = getElement(element, error);
    if (NULL == pWidget)
        return;

    QPoint pos = pWidget->mapTo(view, QPoint(0, 0));
    Point location = Point(pos.x()+pWidget->width()/2, pos.y()+pWidget->height()/2);

    QPoint point = ConvertPointToQPoint(location);

    // Find child widget that will receive event
    QWidget *receiverWidget = view->childAt(point);
    if (NULL != receiverWidget) {
        point = receiverWidget->mapFrom(view, point);
    } else {
        receiverWidget = view;
    }

    QMouseEvent *moveEvent = new QMouseEvent(QEvent::MouseMove, point, Qt::NoButton, Qt::NoButton, Qt::NoModifier);
    QApplication::postEvent(receiverWidget, moveEvent);

    session_->set_mouse_position(location);
*/    
}

void QQmlViewCmdExecutor::ClickElement(const ElementId& element, Error** error) {
/*    
	QWidget* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QWidget* pWidget = getElement(element, error);
    if (NULL == pWidget)
        return;

    if (!pWidget->isVisible()) {
        *error = new Error(kElementNotVisible);
        return;
    }

    session_->logger().Log(kFineLogLevel, "Click on ");
    session_->logger().Log(kFineLogLevel, pWidget->objectName().toStdString());
    QPoint point;
    QRect rect;

    if ( qobject_cast<QRadioButton*>(pWidget) ||
         qobject_cast<QCheckBox*>(pWidget)) {

        QStyle::SubElement subElement;
        QStyleOptionButton opt;
        if (qobject_cast<QRadioButton*>(pWidget)) {
            subElement = QStyle::SE_RadioButtonClickRect;
        }

        if (qobject_cast<QCheckBox*>(pWidget)) {
            subElement = QStyle::SE_CheckBoxClickRect;
        }
        opt.initFrom(pWidget);
        rect = pWidget->style()->subElementRect(subElement, &opt, pWidget);
    } else {
        rect = pWidget->rect();
    }

    QRect visibleClickableLocation = pWidget->visibleRegion().boundingRect().intersected(rect);

    if(visibleClickableLocation.isEmpty()){
        QWidget *tmpParent = pWidget;
        while(tmpParent != NULL && qobject_cast<QScrollArea*>(tmpParent) == NULL){
            tmpParent = tmpParent->parentWidget();
        }
        if(!tmpParent){
            *error = new Error(kMoveTargetOutOfBounds, "Target is out of view in non scrolable widget");
            return;
        }
    }
    point = QPoint(rect.x() + rect.width()/2, rect.y() + rect.height()/2);

    QMouseEvent *pressEvent = new QMouseEvent(QEvent::MouseButtonPress, point, Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
    QMouseEvent *releaseEvent = new QMouseEvent(QEvent::MouseButtonRelease, point, Qt::LeftButton, Qt::NoButton, Qt::NoModifier);

    QApplication::postEvent(pWidget, pressEvent);
    QApplication::postEvent(pWidget, releaseEvent);

    // TODO: session_->set_mouse_position(location);
*/    
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
/*	QDeclarativeView* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QDeclarativeItem* pItem = getElement(element, error);
    if (NULL == pItem)
        return;

    QPoint pos = 
    pWidget->mapToScene(0.0f, 0.0f));

    *location = Point(pos.x(), pos.y());
*/
}

void QQmlViewCmdExecutor::GetElementLocationInView(const ElementId& element, Point* location, Error** error) {
/*    
	QWidget* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QWidget* pWidget = getElement(element, error);
    if (NULL == pWidget)
        return;

    QPoint pos = pWidget->mapTo(view, QPoint(0, 0));

    // TODO: take into account scrollable area
    *location = Point(pos.x(), pos.y());
*/    
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
            qDebug() << "-----------------";
            qDebug() << "className: " << child->metaObject()->className();
            qDebug() << "objectName: " << child->objectName();
            qDebug() << "prop id: " << child->property("id");
            qDebug() << "prop name: " << child->property("name");
    
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
/*    
	QWidget* view = getView(view_id_, error);
    if (NULL == view)
        return;

    QWidget *focusWidget = QApplication::focusWidget();
    if (NULL == focusWidget || view == focusWidget) {
        *error = new Error(kNoSuchElement);
        return;
    }

    // TODO: do we need to check if focusWidget is child of view?

    ElementId active_element;

    session_->AddElement(view_id_, new QElementHandle(focusWidget), &active_element);

    *element = active_element;
*/    
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

/*
void QQmlViewCmdExecutor::createUIXML(QWidget *parent, QIODevice* buff, XMLElementMap& elementsMap, Error** error, bool needAddWebSource) {
    
    QXmlStreamWriter* writer = new QXmlStreamWriter();

    writer->setDevice(buff);
    writer->setAutoFormatting(true);
    writer->writeStartDocument();

    addWidgetToXML(parent, elementsMap, writer, needAddWebSource);

    writer->writeEndDocument();

    delete writer;
    
}
*/

/*
void QQmlViewCmdExecutor::addWidgetToXML(QWidget* parent, XMLElementMap& elementsMap, QXmlStreamWriter* writer, bool needAddWebSource) {
    
    writer->writeStartElement(parent->metaObject()->className());

    if (!parent->objectName().isEmpty())
        writer->writeAttribute("id", parent->objectName());

    if (!parent->windowTitle().isEmpty())
        writer->writeAttribute("name", parent->windowTitle());

    QString elementKey = GenerateRandomID().c_str();
    elementsMap.insert(elementKey, QPointer<QWidget>(parent));
    writer->writeAttribute("elementId", elementKey);

// TODO: this executor doesnt know anything about qwebview
//    QWebView* webview = qobject_cast<QWebView*>(parent);
//    if (webview && needAddWebSource)
//    {
//        writer->writeCharacters(webview->page()->mainFrame()->toHtml());
//    }
//    else
    {
        QList<QObject*> childs = parent->children();
        foreach(QObject *child, childs) {
            QWidget* childWgt = qobject_cast<QWidget*>(child);
            if (childWgt)
                addWidgetToXML(childWgt, elementsMap, writer, needAddWebSource);
        }
    }
    writer->writeEndElement();
  
}
*/



} //namespace webdriver 
