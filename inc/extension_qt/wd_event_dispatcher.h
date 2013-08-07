#ifndef WDEVENTDISPATCHER_H
#define WDEVENTDISPATCHER_H

#include <QtCore/QVector>

#include "event_dispatcher.h"

/// This class contains dispathers chain.
class WDEventDispatcher
{
private:
    /// Constructor
    WDEventDispatcher();
    /// Destructor
    virtual ~WDEventDispatcher();

public:
    /// Method returns WDEventDispatcher instance
    /// @return WDEventDispatcher pointer
    static WDEventDispatcher *getInstance();
    /// Add dispatcher item to dispatchers chain
    /// @param dispatcher - pointer to new dispatcher instance
    void add(EventDispatcher *dispatcher);
    /// Return container with dispatcher chain
    /// @return QVector with EventDispatcher pointers
    QVector<EventDispatcher*>& getDispatchers();
    /// Method dispatch event
    /// @param event - event for dispatching
    /// @return true if event was consumed, else false
    bool dispatch(QEvent *event);

private:
    static WDEventDispatcher *_instance;
    QVector<EventDispatcher*> _dispatchers;
};

#endif // WDEVENTDISPATCHER_H
