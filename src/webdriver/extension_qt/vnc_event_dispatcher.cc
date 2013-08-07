#include "extension_qt/vnc_event_dispatcher.h"

VNCEventDispatcher::VNCEventDispatcher(VNCClient *client)
    : _vncClient(client)
{
}

VNCEventDispatcher::~VNCEventDispatcher()
{
}

bool VNCEventDispatcher::dispatch(QEvent *event, bool consumed)
{
    if (consumed)
        return false;

    switch(event->type())
    {
        case QEvent::KeyPress:
        case QEvent::KeyRelease: _vncClient->sendKeyEvent(dynamic_cast<QKeyEvent*>(event)); break;
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
        case QEvent::MouseButtonDblClick: _vncClient->sendMouseEvent(dynamic_cast<QMouseEvent*>(event)); break;
        default: break;
    }

    return true;
}
