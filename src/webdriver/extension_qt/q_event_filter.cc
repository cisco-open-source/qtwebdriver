#include "q_event_filter.h"

QRepaintEventFilter::QRepaintEventFilter(QObject *parent) :
    QObject(parent)
{
}

QRepaintEventFilter::~QRepaintEventFilter() {}

bool QRepaintEventFilter::eventFilter(QObject *pobject, QEvent *pevent) {
    if (pevent->type()== QEvent::Paint) {
        emit repainted();
    }
    return false;
}

void QCheckPagePaint::pagePainted() {
    is_painting = true;
    emit painted();
}
