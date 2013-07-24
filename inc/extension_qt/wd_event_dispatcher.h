#ifndef WDEVENTDISPATCHER_H
#define WDEVENTDISPATCHER_H

#include <QtCore/QVector>

#include "event_dispatcher.h"

class WDEventDispatcher
{
private:
    WDEventDispatcher();
    virtual ~WDEventDispatcher();

public:
    static WDEventDispatcher *getInstance();
    void add(EventDispatcher *dispatcher);
    QVector<EventDispatcher*>& getDispatchers();

private:
    static WDEventDispatcher *_instance;
    QVector<EventDispatcher*> _dispatchers;
};

#endif // WDEVENTDISPATCHER_H
