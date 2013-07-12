#ifndef Q_EVENT_FILTER_H
#define Q_EVENT_FILTER_H

#include <QtCore/QObject>
#include <QtCore/qcoreevent.h>

class QRepaintEventFilter : public QObject
{
    Q_OBJECT
signals:
    void repainted();
protected:
    virtual bool eventFilter(QObject *pobject, QEvent *pevent);

public:
    explicit QRepaintEventFilter(QObject *parent = 0);
    virtual ~QRepaintEventFilter();
};

#endif // Q_EVENT_FILTER_H
