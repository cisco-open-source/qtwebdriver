#ifndef WEBDRIVER_QT_VIEW_RUNNER_H_
#define WEBDRIVER_QT_VIEW_RUNNER_H_

#include <string>
#include <vector>

#include "webdriver_view_runner.h"

#include <QtCore/QObject>

#include "base/callback.h"
#include "base/callback_forward.h"

namespace webdriver {

class QTaskRunner : public QObject
    {
        Q_OBJECT
    public:
        explicit QTaskRunner(QObject *parent = 0) :
            QObject(parent) {};
    
    signals:
    
    public slots:
        void runTask(const base::Closure& task) { task.Run(); };
    };    

class QViewRunner : public ViewRunner {
public:
    QViewRunner();
    virtual ~QViewRunner() {};

    virtual void RunClosure(const base::Closure& task,
                            base::WaitableEvent* done_event);

private:

 //   class QTaskRunner : public QObject
 //   {
 //       Q_OBJECT
 //   public:
 //       explicit QTaskRunner(QObject *parent = 0) :
 //           QObject(parent) {};
 //   
 //   signals:
 //   
 //   public slots:
 //       void runTask(const base::Closure& task) { task.Run(); };
 //   };

    QTaskRunner qt_ui_task;
    
    DISALLOW_COPY_AND_ASSIGN(QViewRunner);
};

}  // namespace webdriver

#endif  // WEBDRIVER_QT_VIEW_RUNNER_H_


