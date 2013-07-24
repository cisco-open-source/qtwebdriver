#ifndef EVENTDISPATCHER_H
#define EVENTDISPATCHER_H

#include <QtCore/QEvent>

class EventDispatcher
{
public:
    virtual bool dispatch(QEvent *event, bool consumed)=0;
};

#endif // EVENTDISPATCHER_H
