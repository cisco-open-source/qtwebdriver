#ifndef QTASKRUNNER_H
#define QTASKRUNNER_H

#include <QtCore/QObject>
#include "base/callback.h"
#include "base/callback_forward.h"

class QTaskRunner : public QObject
{
    Q_OBJECT
public:
    explicit QTaskRunner(QObject *parent = 0);
    
signals:
    
public slots:
    void runTask(const base::Closure& task);
    
};

#endif // QTASKRUNNER_H
