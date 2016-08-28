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
#include <QtCore/QObject>
#include <QtCore/QTextCodec>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    #include <QtConcurrent/QtConcurrentRun>
    #if (WD_ENABLE_WEB_VIEW == 1)
        #include <QtWebKitWidgets/QWebView>
    #endif
    #include <QtWidgets/QApplication>
#else
    #include <QtGui/QApplication>
    #if (WD_ENABLE_WEB_VIEW == 1)
        #include <QtWebKit/QtWebKit>
    #endif
#endif

#include <iostream>

#include "base/at_exit.h"
#include "webdriver_server.h"
#include "webdriver_view_transitions.h"
#include "versioninfo.h"
#include "webdriver_route_table.h"
#include "shutdown_command.h"
#include "webdriver_route_patterns.h"

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    // headers for Quick2 extension
    #include "extension_qt/quick2_view_creator.h"
    #include "extension_qt/quick2_view_executor.h"
    #include "extension_qt/quick2_view_enumerator.h"
#else
    #include <QtDeclarative/QDeclarativeView>
    // headers for Quick1 extension
    #include "extension_qt/qml_view_creator.h"
    #include "extension_qt/qml_view_executor.h"
    #include "extension_qt/qml_view_enumerator.h"
#if (WD_ENABLE_WEB_VIEW == 1)
    #include "extension_qt/qdeclarativewebview.h"
    #include "extension_qt/qml_web_view_enumerator.h"
    #include "extension_qt/qml_web_view_executor.h"
#endif //WD_ENABLE_WEB_VIEW
#endif //QT_VERSION

#if (WD_ENABLE_WEB_VIEW == 1)
#include "extension_qt/web_view_creator.h"
#include "extension_qt/web_view_executor.h"
#include "extension_qt/web_view_enumerator.h"
#include "extension_qt/qwebviewext.h"
#include "extension_qt/graphics_web_view_executor.h"
#include "extension_qt/graphics_web_view_enumerator.h"
#endif //WD_ENABLE_WEB_VIEW

#include "extension_qt/q_view_runner.h"
#include "extension_qt/q_session_lifecycle_actions.h"
#include "extension_qt/widget_view_creator.h"
#include "extension_qt/widget_view_enumerator.h"
#include "extension_qt/widget_view_executor.h"
#include "extension_qt/wd_event_dispatcher.h"
#include "extension_qt/vnc_event_dispatcher.h"
#include "extension_qt/uinput_event_dispatcher.h"

#include "extension_qt/vncclient.h"
#include "extension_qt/uinput_manager.h"
#include "webdriver_switches.h"

int wd_setup(int argc, char *argv[]);