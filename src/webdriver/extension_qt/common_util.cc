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

#include "common_util.h"

namespace webdriver {

Rect QCommonUtil::ConvertQRectToRect(const QRect &rect) {
    return Rect(rect.x(), rect.y(), rect.width(), rect.height());
}

QRect QCommonUtil::ConvertRectToQRect(const Rect &rect) {
    QRect resultRect;
    resultRect.setX(rect.x());
    resultRect.setY(rect.y());
    resultRect.setWidth(rect.width());
    resultRect.setHeight(rect.height());

    return resultRect;
}

QPoint QCommonUtil::ConvertPointToQPoint(const Point &p) {
    QPoint resultPoint;
    resultPoint.setX(p.x());
    resultPoint.setY(p.y());

    return resultPoint;
}

QSize QCommonUtil::ConvertSizeToQSize(const Size &sz) {
    QSize resultSize;
    resultSize.setWidth(sz.width());
    resultSize.setHeight(sz.height());

    return resultSize;
}

Qt::MouseButton QCommonUtil::ConvertMouseButtonToQtMouseButton(MouseButton button) {
    Qt::MouseButton result = Qt::NoButton;

    switch(button)
    {
        case kLeftButton: result = Qt::LeftButton; break;
        case kMiddleButton: result = Qt::MidButton; break;
        case kRightButton: result = Qt::RightButton; break;
        default: result = Qt::NoButton;
    }

    return result;
}

std::string QCommonUtil::GetQtVersion() {
    return QT_VERSION_STR;
}

QString StringUtil::trimmed(const QString& str, const QString& symbols) {
    int start = 0;
    while (start < str.length() && symbols.contains(str.at(start))) {
        start++;
    }

    int end = str.length() - 1;
    while (end >= 0 && symbols.contains(str.at(end))) {
        end--;
    }

    return str.mid(start, end + 1 - start);
}

/// Return substring of str1 before last occurence of str2
QString StringUtil::substringBeforeLast(const QString& str1, const QString& str2) {
    int position = str1.lastIndexOf(str2);
    if (position == -1)
        return str1;
    return str1.left(position);
}

} // namespace webdriver
