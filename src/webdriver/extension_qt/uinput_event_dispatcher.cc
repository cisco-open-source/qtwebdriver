#include "build/build_config.h"
#include "extension_qt/uinput_event_dispatcher.h"

#if defined(OS_LINUX)

UInputEventDispatcher::UInputEventDispatcher(UInputManager *manager)
    : _eventManager(manager)
{
}

UInputEventDispatcher::~UInputEventDispatcher()
{
}

bool UInputEventDispatcher::dispatch(QEvent *event, bool consumed)
{
    if (consumed)
        return false;

    QKeyEvent *keyEvent = dynamic_cast<QKeyEvent*>(event);
    if(NULL != keyEvent)
    {
        _eventManager->injectKeyEvent(keyEvent);

        return true;
    }

    return false;
}

#endif // OS_LINUX
