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

#ifndef WEBDRIVER_QT_VIEW_RUNNER_H_
#define WEBDRIVER_QT_VIEW_RUNNER_H_

#include <string>
#include <vector>

#include "webdriver_view_runner.h"

#include <QtCore/QObject>

#include "base/callback.h"
#include "base/callback_forward.h"

namespace webdriver {

class QTaskRunner : public QObject {
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


