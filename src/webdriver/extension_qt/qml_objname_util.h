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

#ifndef WEBDRIVER_QT_QML_OBJN_UTIL_H_
#define WEBDRIVER_QT_QML_OBJN_UTIL_H_

#include <string>
#include <QtCore/QDebug>
#include <QtQuick/QQuickItem>
#include "common_util.h"

namespace webdriver {

class ObjectNameUtils
   : public QObject
{
    Q_OBJECT
public:
    ObjectNameUtils(QQuickItem *root);
    ~ObjectNameUtils();
    Q_INVOKABLE QObject *findChild(const QString& name);
protected:
    QQuickItem* root_;

    QObject * findChild(const QQuickItem* item, const QString& name) {
        QList<QQuickItem*> childs = item->childItems();

        foreach(QQuickItem *child, childs) {
            if (!child) continue;
            if (name == child->objectName())
               return child;

            QObject* q = findChild(child, name);

            if (q)
                return q;
        }

        return NULL;
    }

};
}  // namespace webdriver

#endif  // WEBDRIVER_QT_QML_OBJN_UTIL_H_
