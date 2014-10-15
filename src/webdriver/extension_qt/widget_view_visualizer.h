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

#ifndef WIDGET_VIEW_VISUALIZER_H
#define WIDGET_VIEW_VISUALIZER_H

#include <string>
#include <QtCore/QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets/QWidget>
#else
#include <QtGui/QWidget>
#endif
#include "webdriver_view_id.h"

namespace webdriver {

class Error;
class Session;

class QWidgetViewVisualizerSourceCommand {
public:
    QWidgetViewVisualizerSourceCommand(Session* session, ViewId viewId, QWidget* view);
    void Execute(std::string* source, Error** error);

private:
#if defined(OS_LINUX)
    std::string transform(const std::string& source, const std::string& stylesheet) const;
    std::string transform(const std::string& source, const std::wstring& stylesheet) const;
#endif

    Session* session_;
    ViewId viewId_;
    QWidget* view_;
};

}  // namespace webdriver

#endif // WIDGET_VIEW_VISUALIZER_H

