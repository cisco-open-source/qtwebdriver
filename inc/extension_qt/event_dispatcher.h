#ifndef EVENTDISPATCHER_H
#define EVENTDISPATCHER_H

#include <QtCore/QEvent>

/// Abstract class which provides interface for event dispatchers
class EventDispatcher
{
public:

    /// Abstract method, should be implemented in descendants
    virtual bool dispatch(QEvent *event, bool consumed)=0;
};

#endif // EVENTDISPATCHER_H
