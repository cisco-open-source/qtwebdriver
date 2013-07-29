#include "extension_qt/vnc_event_dispatcher.h"
#include "vnc/rc_keys.h"

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

    /*if (isRemoteControlEvent(event))
    {
        _vncClient->sendKeyEvent(dynamic_cast<QKeyEvent*>(event));
        return true;
    }*/

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

bool VNCEventDispatcher::isRemoteControlEvent(QEvent *event)
{
    bool consumed = false;

    QKeyEvent *rcKey = dynamic_cast<QKeyEvent*>(event);

    if (NULL != rcKey)
    {
        switch (rcKey->key())
        {
            case RC_POWER:
            case RC_MENU:
            case RC_BACK:
            case RC_UP:
            case RC_DOWN:
            case RC_LEFT:
            case RC_RIGHT:
            case RC_OK:
            case RC_INFO:
            case RC_TEXT:
            case RC_RECORD:
            case RC_STOP:
            case RC_ONE:
            case RC_TWO:
            case RC_THREE:
            case RC_FOUR:
            case RC_FIVE:
            case RC_SIX:
            case RC_SEVEN:
            case RC_EIGHT:
            case RC_NINE:
            case RC_ZERO:
            case COMPANION_DEVICE_KEY_LIVE_SWIPE:
            case COMPANION_DEVICE_KEY_VOD_SWIPE:
            case COMPANION_DEVICE_KEY_PAD_UP:
            case COMPANION_DEVICE_KEY_PAD_DOWN:
            case COMPANION_DEVICE_KEY_PAD_LEFT:
            case COMPANION_DEVICE_KEY_PAD_RIGHT: consumed = true; break;
            default: consumed = false; break;
        }
    }

    return consumed;
}
