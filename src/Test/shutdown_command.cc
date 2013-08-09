#include "shutdown_command.h"
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
