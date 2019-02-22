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

void QWidgetXmlSerializer::addWidget(QObject* widget) {
    QWidget* pWidget = qobject_cast<QWidget*>(widget);
    if (NULL == pWidget)
        return;

    QString elementName = getElementName(pWidget);
    writer_.writeStartElement(elementName);

    if (dumpAll_) {
        for (int propertyIndex = 0; propertyIndex < pWidget->metaObject()->propertyCount(); propertyIndex++) {
            const QMetaProperty& property = pWidget->metaObject()->property(propertyIndex);
            writer_.writeAttribute(property.name(), property.read(pWidget).toString());
        }
    }

    if (!pWidget->objectName().isEmpty())
        writer_.writeAttribute("id", pWidget->objectName());

    if (!pWidget->windowTitle().isEmpty())
        writer_.writeAttribute("name", pWidget->windowTitle());

    QString elementKey;
    if (session_) {
        ElementId elementId = session_->GetElementIdForHandle(viewId_, new QElementHandle(pWidget));
        if (!elementId.is_valid())
            session_->AddElement(viewId_, new QElementHandle(pWidget), &elementId);
        elementKey = QString::fromStdString(elementId.id());
    } else {
        elementKey = GenerateRandomID().c_str();
    }
    elementsMap_.insert(elementKey, QPointer<QObject>(pWidget));
    writer_.writeAttribute("elementId", elementKey);

    writer_.writeAttribute("className", pWidget->metaObject()->className());

    // write QAction items
    QList<QAction*> actions = pWidget->actions();
    foreach(QAction* action, actions) {
        QString actionName = action->metaObject()->className();
        writer_.writeStartElement(actionName);

        if (!action->objectName().isEmpty())
            writer_.writeAttribute("id", action->objectName());

        writer_.writeAttribute("className", action->metaObject()->className());
        writer_.writeAttribute("text", action->text());

        elementKey = GenerateRandomID().c_str();
        elementsMap_.insert(elementKey, QPointer<QObject>(action));
        writer_.writeAttribute("elementId", elementKey);

        QString skipAttr("text");
        if (dumpAll_) {
            for (int propertyIndex = 0; propertyIndex < action->metaObject()->propertyCount(); propertyIndex++) {
                const QMetaProperty& property = action->metaObject()->property(propertyIndex);
                if (0 == skipAttr.compare(property.name())) continue;
                writer_.writeAttribute(property.name(), property.read(action).toString());
            }
        }
        
        writer_.writeEndElement();
    }

    // write child widgets 
    QList<QObject*> childs = pWidget->children();
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
