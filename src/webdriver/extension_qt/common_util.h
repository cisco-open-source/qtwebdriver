#ifndef WEBDRIVER_QT_WD_COMMON_UTIL_H_
#define WEBDRIVER_QT_WD_COMMON_UTIL_H_

#include <string>

#include <QtCore/QtGlobal>
#include <QtCore/QRect>
#include <QtCore/QPoint>
//#include <QtGui/QMouseEvent>

#include "webdriver_basic_types.h"


namespace webdriver {

class Session;	
class ViewId;

class QCommonUtil {
public:
    static Rect ConvertQRectToRect(const QRect &rect);
    static QRect ConvertRectToQRect(const Rect &rect);
    static QPoint ConvertPointToQPoint(const Point &p);
    static Qt::MouseButton ConvertMouseButtonToQtMouseButton(MouseButton button);
    static std::string GetQtVersion();

private:
	QCommonUtil() {};
    ~QCommonUtil(){}
};


}  // namespace webdriver

#endif  // WEBDRIVER_QT_WD_COMMON_UTIL_H_
