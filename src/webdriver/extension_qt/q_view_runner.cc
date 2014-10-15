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

#include "extension_qt/q_view_runner.h"

#include <QtCore/QDebug>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets/QApplication>
#else
#include <QtGui/QApplication>
#endif

#include "base/synchronization/waitable_event.h"

namespace webdriver {

QViewRunner::QViewRunner()
	: ViewRunner() {
	qt_ui_task.moveToThread(QApplication::instance()->thread());
}

void QViewRunner::RunClosure(const base::Closure& task,
                            base::WaitableEvent* done_event) {
	QMetaObject::invokeMethod(&qt_ui_task, "runTask", Qt::BlockingQueuedConnection, Q_ARG(const base::Closure&, task));
    done_event->Signal();
}

} // namespace webdriver
