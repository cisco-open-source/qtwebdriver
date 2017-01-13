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

#include "commands/shutdown_command.h"
#include <iostream>
#include "webdriver_server.h"

#include <QtCore/qglobal.h>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    #include <QtConcurrent/QtConcurrentRun>
    #include <QtWidgets/QApplication>
#else
    #include <QtGui/QApplication>
#endif

namespace webdriver {

ShutdownCommand::ShutdownCommand(const std::vector<std::string> &path_segments,
                                 const base::DictionaryValue * const parameters)
                                : Command(path_segments, parameters) {}

ShutdownCommand::~ShutdownCommand() { }

void ShutdownCommand::ExecutePost(Response * const response)
{
    Server *wd_server = Server::GetInstance();
    QApplication::quit();
    wd_server->Stop(true);
}

bool ShutdownCommand::DoesPost() const
{
    return true;
}

bool ShutdownCommand::DoesGet() const
{
    return true;
}

void ShutdownCommand::ExecuteGet(Response * const response)
{
    ExecutePost(response);
}

}
