#include "q_event_filter.h"
#include "webdriver_logging.h"

QRepaintEventFilter::QRepaintEventFilter(QObject *parent) :
    QObject(parent)
{
}

QRepaintEventFilter::~QRepaintEventFilter() {}

bool QRepaintEventFilter::eventFilter(QObject *pobject, QEvent *pevent) {
    if (pevent->type()== QEvent::Resize) {
        webdriver::GlobalLogger::Log(webdriver::kInfoLogLevel, "***** Resize ********** in QRepaintEventFilter::eventFilter() ****");
    }
    if (pevent->type()== QEvent::Paint) {
        webdriver::GlobalLogger::Log(webdriver::kInfoLogLevel, "***** Paint ********** in QRepaintEventFilter::eventFilter() ****");
        emit repainted();
    }
    if (pevent->type()== QEvent::Show) {
        webdriver::GlobalLogger::Log(webdriver::kInfoLogLevel, "***** Show ********** in QRepaintEventFilter::eventFilter() ****");
    }
    return false;
}

void QCheckPagePaint::pagePainted() {
    is_painting = true;
    //emit painted();
    webdriver::GlobalLogger::Log(webdriver::kInfoLogLevel, "******************* in QCheckPagePaint::pagePainted() ****");
}
