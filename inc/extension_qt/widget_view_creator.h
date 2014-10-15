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

#ifndef WEBDRIVER_QT_WIDGET_VIEW_CREATOR_H_
#define WEBDRIVER_QT_WIDGET_VIEW_CREATOR_H_

#include <string>

#include "webdriver_view_factory.h"
#include "webdriver_logging.h"

namespace webdriver {

/// base class for custom view's creators
class QWidgetViewCreator : public ViewCreator {
public:
    QWidgetViewCreator();
    virtual ~QWidgetViewCreator(){}

    virtual bool CreateViewByClassName(const Logger& logger, const std::string& className,
                                       const Point* position, const Size* size, ViewHandle** view) const;

    virtual bool CreateViewForUrl(const Logger& logger, const std::string& url,
                                  const Point* position, const Size* size, ViewHandle** view) const;

private:

    DISALLOW_COPY_AND_ASSIGN(QWidgetViewCreator);
};


}  // namespace webdriver

#endif  // WEBDRIVER_QT_WIDGET_VIEW_CREATOR_H_
