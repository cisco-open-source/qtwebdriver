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

#ifndef WEBDRIVER_QT_WD_COMMON_UTIL_H_
#define WEBDRIVER_QT_WD_COMMON_UTIL_H_

#include <string>

#include <QtCore/QHash>
#include <QtCore/QtGlobal>
#include <QtCore/QRect>
#include <QtCore/QPoint>
#include <QtCore/QSize>
//#include <QtGui/QMouseEvent>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtCore/QXmlStreamWriter>
#else
#include <QtXml/QXmlStreamWriter>
#endif

#include "webdriver_basic_types.h"


namespace webdriver {

class Session;
class ViewId;

class QCommonUtil {
public:
    static Rect ConvertQRectToRect(const QRect &rect);
    static QRect ConvertRectToQRect(const Rect &rect);
    static QPoint ConvertPointToQPoint(const Point &p);
    static QSize ConvertSizeToQSize(const Size &sz);
    static Qt::MouseButton ConvertMouseButtonToQtMouseButton(MouseButton button);
    static std::string GetQtVersion();

private:
    QCommonUtil() {}
    ~QCommonUtil() {}
};

class StringUtil {
public:
    /// Trim chars in 'symbols' argument from start and end of 'str'
    static QString trimmed(const QString& str, const QString& symbols);

    /// Return substring of str1 before last occurence of str2
    static QString substringBeforeLast(const QString& str1, const QString& str2);
};

template <class Widget>
class QViewXmlSerializer {
public:
    typedef QHash<QString, Widget*> XMLElementMap;

    QViewXmlSerializer(QIODevice* buff)
        : session_(NULL), dumpAll_(false)
    {
        writer_.setDevice(buff);
        writer_.setAutoFormatting(true);
    }

    void createXml(Widget* widget) {
        writer_.writeStartDocument();
        if (!stylesheet_.isEmpty()) {
            writer_.writeProcessingInstruction("xml-stylesheet", "href=\"" + stylesheet_ + "\"");
        }
        addWidget(widget);
        writer_.writeEndDocument();
    }

    const XMLElementMap& getElementsMap() {
        return elementsMap_;
    }

    void setDumpAll(bool dumpAll) {
        dumpAll_ = dumpAll;
    }

    void setSession(Session* session) {
        session_ = session;
    }

    void setStylesheet(const QString& stylesheet) {
        stylesheet_ = stylesheet;
    }

protected:
    virtual void addWidget(Widget* widget) = 0;

    QXmlStreamWriter writer_;
    XMLElementMap elementsMap_;
    Session* session_;
    bool dumpAll_;
    QString stylesheet_;
};

}  // namespace webdriver

#endif  // WEBDRIVER_QT_WD_COMMON_UTIL_H_
