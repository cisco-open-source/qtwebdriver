#include "qtaskrunner.h"

QTaskRunner::QTaskRunner(QObject *parent) :
    QObject(parent)
{
}

void QTaskRunner::runTask(const base::Closure &task)
{
    task.Run();
}
