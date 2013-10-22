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

} // namespace webdriver
