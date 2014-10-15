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

#include "qml_view_util.h"
#include "webdriver_session.h"
#include "webdriver_error.h"
#include "webdriver_util.h"
#include "q_content_type_resolver.h"

#include <QtNetwork/QNetworkAccessManager>
#include <QtCore/QFileInfo>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include "extension_qt/qwindow_view_handle.h"
#include <QtQuick/QQuickView>
//#include <QtQuick/QQuickItem>
#else
#include "extension_qt/widget_view_handle.h"
#include <QtDeclarative/QDeclarativeView>
//#include <QtDeclarative/QDeclarativeItem>
#endif

namespace webdriver {

bool QQmlViewUtil::isUrlSupported(const std::string& url) {
    QUrl qUrl(url.c_str());
    QString scheme = qUrl.scheme();

    if ( (0 == scheme.compare("http", Qt::CaseInsensitive)) ||
         (0 == scheme.compare("https", Qt::CaseInsensitive)) ) {

        scoped_ptr<QNetworkAccessManager> pmanager(new QNetworkAccessManager());
        scoped_ptr<QContentTypeResolver> presolver(new QContentTypeResolver(pmanager.get()));
        std::string contentType;

        scoped_ptr<Error> ignore_err(presolver->resolveContentType(url, contentType));

        if (ignore_err != NULL) {
            return false;
        }

        return isContentTypeSupported(contentType);
    } else {
        // detection fy filename
        QFileInfo fileInfo(qUrl.path());
        if (0 == fileInfo.suffix().compare("qml", Qt::CaseInsensitive)) {
            return true;
        }
    }


    return false;
}

bool QQmlViewUtil::isContentTypeSupported(const std::string& mime) {
    if (mime == "text/x-qml") 
        return true;

    return false;
}

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
void QQmlViewUtil::removeInternalSuffixes(QString& str) {
    str.remove(QRegExp(QLatin1String("_QMLTYPE_\\d+")));
    str.remove(QRegExp(QLatin1String("_QML_\\d+")));
    if (str.startsWith(QLatin1String("QQuick"))) str = str.mid(6);
}

QWindow* QQmlViewUtil::getQWindowView(Session* session, const ViewId& viewId) {
    ViewHandle* viewHandle =  session->GetViewHandle(viewId);
    if (NULL == viewHandle) 
        return NULL;

    QWindowViewHandle* qViewHandle = dynamic_cast<QWindowViewHandle*>(viewHandle);
    if (NULL == qViewHandle)
        return NULL;

    return qobject_cast<QWindow*>(qViewHandle->get());
}

QQuickView* QQmlViewUtil::getQMLView(Session* session, const ViewId& viewId) {
    ViewHandle* viewHandle =  session->GetViewHandle(viewId);
    if (NULL == viewHandle) 
        return NULL;

    QWindowViewHandle* qViewHandle = dynamic_cast<QWindowViewHandle*>(viewHandle);
    if (NULL == qViewHandle)
        return NULL;

    return qobject_cast<QQuickView*>(qViewHandle->get());
}
#else
void QQmlViewUtil::removeInternalSuffixes(QString& str) {
    str.remove(QRegExp(QLatin1String("_QMLTYPE_\\d+")));
    str.remove(QRegExp(QLatin1String("_QML_\\d+")));
    if (str.startsWith(QLatin1String("QDeclarative"))) str = str.mid(12);
}

QDeclarativeView* QQmlViewUtil::getQMLView(Session* session, const ViewId& viewId) {
	ViewHandle* viewHandle =  session->GetViewHandle(viewId);
	if (NULL == viewHandle) 
		return NULL;

    QViewHandle* qViewHandle = dynamic_cast<QViewHandle*>(viewHandle);
    if (NULL == qViewHandle)
    	return NULL;

    return qobject_cast<QDeclarativeView*>(qViewHandle->get());
}
#endif

#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
void QQmlXmlSerializer::addWidget(QDeclarativeItem* item) {
    if (NULL == item) {
        session_->logger().Log(kWarningLogLevel, "parent item is NULL.");
        return;
    }

    QString className(item->metaObject()->className());
    QQmlViewUtil::removeInternalSuffixes(className);

    writer_.writeStartElement(className);

    if (!item->objectName().isEmpty())
        writer_.writeAttribute("id", item->objectName());

    QString elementKey = GenerateRandomID().c_str();
    elementsMap_.insert(elementKey, QPointer<QDeclarativeItem>(item));
    writer_.writeAttribute("elementId", elementKey);

    if (dumpAll_) {
        QStringList writtenAttributes;
        for (int propertyIndex = 0; propertyIndex < item->metaObject()->propertyCount(); propertyIndex++) {
            const QMetaProperty& property = item->metaObject()->property(propertyIndex);
            if (!writtenAttributes.contains(property.name())) {
                writer_.writeAttribute(property.name(), property.read(item).toString());
                writtenAttributes.append(property.name());
            }
        }
    }

    QList<QGraphicsItem*> childs = item->childItems();
    foreach(QGraphicsItem *child, childs) {
        QDeclarativeItem* childItem = qobject_cast<QDeclarativeItem*>(child);
        if (childItem)
            addWidget(childItem);
    }

    writer_.writeEndElement();
}
#else
void QQmlXmlSerializer::addWidget(QQuickItem* item) {
    if (NULL == item) {
        session_->logger().Log(kWarningLogLevel, "parent item is NULL.");
        return;
    }

    QString className(item->metaObject()->className());
    QQmlViewUtil::removeInternalSuffixes(className);

    writer_.writeStartElement(className);

    if (!item->objectName().isEmpty())
        writer_.writeAttribute("id", item->objectName());

    QString elementKey = GenerateRandomID().c_str();
    elementsMap_.insert(elementKey, QPointer<QQuickItem>(item));
    writer_.writeAttribute("elementId", elementKey);

    if (dumpAll_) {
        QStringList writtenAttributes;
        for (int propertyIndex = 0; propertyIndex < item->metaObject()->propertyCount(); propertyIndex++) {
            const QMetaProperty& property = item->metaObject()->property(propertyIndex);
            if (!writtenAttributes.contains(property.name())) {
                writer_.writeAttribute(property.name(), property.read(item).toString());
                writtenAttributes.append(property.name());
            }
        }
    }

    QList<QQuickItem*> childs = item->childItems();
    foreach(QQuickItem *child, childs) {
        if (child) addWidget(child);
    }

    writer_.writeEndElement();
}
#endif

} // namespace webdriver
