#include "widget_view_util.h"
#include "webdriver_session.h"
#include "webdriver_util.h"
#include "extension_qt/widget_element_handle.h"
#include "extension_qt/widget_view_handle.h"

#include <QtCore/QMetaProperty>

namespace webdriver {

const char QWidgetViewUtil::url_protocol[] = "qtwidget://";

bool QWidgetViewUtil::isUrlSupported(const std::string& url) {
	const std::string widget_prefix(url_protocol);

	if (url.compare(0, widget_prefix.length(), widget_prefix) == 0) {
		return true;
	}

	return false;
}	

std::string QWidgetViewUtil::extractClassName(const std::string& url) {
	const std::string widget_prefix(url_protocol);	

	return url.substr(widget_prefix.length());
}

std::string QWidgetViewUtil::makeUrlByClassName(const std::string& className) {
	const std::string widget_prefix(url_protocol);

	return widget_prefix + className;	
}

QWidget* QWidgetViewUtil::getView(Session* session, const ViewId& viewId) {
	ViewHandle* viewHandle =  session->GetViewHandle(viewId);
	if (NULL == viewHandle) 
		return NULL;

    QViewHandle* qViewHandle = dynamic_cast<QViewHandle*>(viewHandle);
    if (NULL == qViewHandle)
    	return NULL;

    return qViewHandle->get();
}

void QWidgetXmlSerializer::addWidget(QWidget* widget) {
    QString elementName = getElementName(widget);
    writer_.writeStartElement(elementName);

    if (dumpAll_) {
        for (int propertyIndex = 0; propertyIndex < widget->metaObject()->propertyCount(); propertyIndex++) {
            const QMetaProperty& property = widget->metaObject()->property(propertyIndex);
            writer_.writeAttribute(property.name(), property.read(widget).toString());
        }
    }

    if (!widget->objectName().isEmpty())
        writer_.writeAttribute("id", widget->objectName());

    if (!widget->windowTitle().isEmpty())
        writer_.writeAttribute("name", widget->windowTitle());

    QString elementKey;
    if (session_) {
        ElementId elementId = session_->GetElementIdForHandle(viewId_, new QElementHandle(widget));
        if (!elementId.is_valid())
            session_->AddElement(viewId_, new QElementHandle(widget), &elementId);
        elementKey = QString::fromStdString(elementId.id());
    } else {
        elementKey = GenerateRandomID().c_str();
    }
    elementsMap_.insert(elementKey, QPointer<QWidget>(widget));
    writer_.writeAttribute("elementId", elementKey);

    writer_.writeAttribute("className", widget->metaObject()->className());

    QList<QObject*> childs = widget->children();
    foreach(QObject* child, childs) {
        QWidget* childWgt = qobject_cast<QWidget*>(child);
        if (childWgt)
            addWidget(childWgt);
    }

    writer_.writeEndElement();
}

QString QWidgetXmlSerializer::getElementName(const QObject* object) const {
    QString elementName = object->metaObject()->className();
    if (supportedClasses_.empty())
        return elementName;

    const QMetaObject* metaObject = object->metaObject();
    while (!supportedClasses_.contains(metaObject->className()) &&
           metaObject->superClass() != NULL) {
        metaObject = metaObject->superClass();
    }
    if (supportedClasses_.contains(metaObject->className()))
        elementName = metaObject->className();

    return elementName;
}

} // namespace webdriver
