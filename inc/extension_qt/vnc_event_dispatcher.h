#ifndef VNCEVENTDISPATCHER_H
#define VNCEVENTDISPATCHER_H

#include "event_dispatcher.h"
#include "vncclient.h"

class VNCEventDispatcher : public EventDispatcher
{
public:
    VNCEventDispatcher(VNCClient *client);
    virtual ~VNCEventDispatcher();

    bool dispatch(QEvent *event, bool consumed);

private:
    VNCClient *_vncClient;
};

#endif // VNCEVENTDISPATCHER_H
