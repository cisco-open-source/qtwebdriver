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

#ifndef WEBDRIVER_QT_QML_VIEW_UTIL_H_
#define WEBDRIVER_QT_QML_VIEW_UTIL_H_

#include <string>

#include <QtCore/QDebug>

#include "common_util.h"

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
class QWindow;
class QQuickView;
#include <QtQuick/QQuickItem>
#else
class QDeclarativeView;
#include <QtDeclarative/QDeclarativeItem>
#endif

namespace webdriver {

class Session;
class ViewId;
class Error;

class QQmlViewUtil {
public:
    static bool isUrlSupported(const std::string& url);
    static bool isContentTypeSupported(const std::string& mime);
    static void removeInternalSuffixes(QString& str);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    static QWindow* getQWindowView(Session* session, const ViewId& viewId);
    static QQuickView* getQMLView(Session* session, const ViewId& viewId);
#else    
    static QDeclarativeView* getQMLView(Session* session, const ViewId& viewId);
#endif
};

#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
class QQmlXmlSerializer : public QViewXmlSerializer<QDeclarativeItem>
{
public:
    QQmlXmlSerializer(QIODevice* buff)
        : QViewXmlSerializer<QDeclarativeItem>(buff)
    {}

protected:
    virtual void addWidget(QDeclarativeItem* item);
};
#else
class QQmlXmlSerializer : public QViewXmlSerializer<QQuickItem>
{
public:
    QQmlXmlSerializer(QIODevice* buff)
        : QViewXmlSerializer<QQuickItem>(buff)
    {}

protected:
    virtual void addWidget(QQuickItem* item);
};
#endif

}  // namespace webdriver

#endif  // WEBDRIVER_QT_QML_VIEW_UTIL_H_
