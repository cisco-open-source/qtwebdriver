#ifndef VNCEVENTDISPATCHER_H
#define VNCEVENTDISPATCHER_H

#include "event_dispatcher.h"
#include "vnc/vncclient.h"

class VNCEventDispatcher : public EventDispatcher
{
public:
    VNCEventDispatcher(VNCClient *client);
    virtual ~VNCEventDispatcher();

    bool dispatch(QEvent *event, bool consumed);

private:
    bool isRemoteControlEvent(QEvent *event);

private:
    VNCClient *_vncClient;
};

#endif // VNCEVENTDISPATCHER_H
