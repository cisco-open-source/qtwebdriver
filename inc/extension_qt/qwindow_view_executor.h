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

#ifndef WEBDRIVER_QWINDOW_VIEW_EXECUTOR_H
#define WEBDRIVER_QWINDOW_VIEW_EXECUTOR_H

#include <string>
#include <vector>
#include <map>

#include "webdriver_view_executor.h"

#include "webdriver_error.h"

#include <QtCore/QDebug>
#include <QtGui/QTouchEvent>
#include <QtGui/QTouchDevice>
#include <QtGui/QWindow>

namespace webdriver {

#define NOT_SUPPORTED_IMPL      {*error = new Error(kCommandNotSupported, "Current view doesnt support this command.");}
//#define NOT_IMPLEMENTED_IMPL    {*error = new Error(kUnknownError, "Command not implemented.");}
//#define RET_IF_ERROR(e)         {if(e) {*error = e; return;}}

/// base class for QT5 QWindow based views.
class QWindowViewCmdExecutor : public ViewCmdExecutor {
public:
    explicit QWindowViewCmdExecutor(Session* session, ViewId viewId);
    virtual ~QWindowViewCmdExecutor();

    virtual void GetTitle(std::string* title, Error **error);
    virtual void GetWindowName(std::string* name, Error ** error);
    virtual void GetBounds(Rect *bounds, Error **error);
    virtual void SetBounds(const Rect& bounds, Error** error);
    virtual void Maximize(Error** error);
    virtual void SendKeys(const string16& keys, Error** error);
    virtual void Close(Error** error);
    virtual void SwitchTo(Error** error);
    virtual void FindElement(const ElementId& root_element, const std::string& locator, const std::string& query, ElementId* element, Error** error);
    virtual void SetOrientation(const std::string &orientation, Error **error);
    virtual void GetOrientation(std::string *orientation, Error **error);

protected:
    QWindow* getView(const ViewId& viewId, Error** error);
    Rect ConvertQRectToRect(const QRect &rect);
    QRect ConvertRectToQRect(const Rect &rect);
    QPoint ConvertPointToQPoint(const Point &p);
    Qt::MouseButton ConvertMouseButtonToQtMouseButton(MouseButton button);
    QTouchEvent::TouchPoint createTouchPoint(Qt::TouchPointState state, QPointF &point, QVector2D velocity = QVector2D());
    QTouchEvent::TouchPoint createTouchPointWithId(Qt::TouchPointState state, QPointF &point, int id);
    QTouchEvent* createSimpleTouchEvent(QEvent::Type eventType, Qt::TouchPointStates touchPointStates, QPointF &point, QVector2D velocity = QVector2D());
    QTouchEvent* create2PointTouchEvent(QEvent::Type eventType, Qt::TouchPointStates touchPointStates, QPointF &point1, QPointF &point2);
    QTouchEvent* createTouchEvent(QEvent::Type eventType, Qt::TouchPointStates touchPointStates, const QList<QTouchEvent::TouchPoint> &touchPoints);

private:
    QTouchDevice touchDevice;
    DISALLOW_COPY_AND_ASSIGN(QWindowViewCmdExecutor);
};

}  // namespace webdriver

#endif  // WEBDRIVER_QWINDOW_VIEW_EXECUTOR_H
