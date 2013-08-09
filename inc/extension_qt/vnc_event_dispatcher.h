#ifndef VNCEVENTDISPATCHER_H
#define VNCEVENTDISPATCHER_H

#include "event_dispatcher.h"
#include "extension_qt/vncclient.h"

/// This class dispatch events to VNC module.
class VNCEventDispatcher : public EventDispatcher
{
public:
    /// Constructor with parameters
    /// @param client - VNC client pointer
    VNCEventDispatcher(VNCClient *client);
    /// Destructor
    virtual ~VNCEventDispatcher();

    /// Constructor with parameters
    /// @param event - event for dispatching
    /// @param consumed - flag, whether event was consumed by previous dispatchers
    /// @return if event consumed - return true, else false
    bool dispatch(QEvent *event, bool consumed);

private:
    bool isRemoteControlEvent(QEvent *event);

private:
    VNCClient *_vncClient;
};

#endif // VNCEVENTDISPATCHER_H
