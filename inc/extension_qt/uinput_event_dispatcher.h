#ifndef UINPUT_EVENT_DISPATCHER_H
#define UINPUT_EVENT_DISPATCHER_H

#ifdef OS_LINUX

#include "extension_qt/event_dispatcher.h"
#include "extension_qt/uinput_manager.h"

class UInputEventDispatcher : public EventDispatcher
{
public:
    /// Constructor
    /// @param manager - pointer to user events manager
    UInputEventDispatcher(UInputManager *manager);
    /// Destructor
    ~UInputEventDispatcher();

    /// Dispatch event to user input device
    /// @param event - pointer to event for dispatching
    /// @param consumed - flag whether event was consumed by previous dispatchers
    /// @return true, if event was consumed, else false
    virtual bool dispatch(QEvent *event, bool consumed);

private:
    UInputManager* _eventManager;
};

#endif // OS_LINUX

#endif // UINPUT_EVENT_DISPATCHER_H
