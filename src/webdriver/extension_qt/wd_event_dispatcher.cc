#include "extension_qt/wd_event_dispatcher.h"

WDEventDispatcher* WDEventDispatcher::_instance = NULL;

WDEventDispatcher::WDEventDispatcher()
{
}

WDEventDispatcher::~WDEventDispatcher()
{
    qDeleteAll(_dispatchers);
}

WDEventDispatcher* WDEventDispatcher::getInstance()
{
    if (NULL == _instance)
    {
        _instance = new WDEventDispatcher;
    }

    return _instance;
}

void WDEventDispatcher::add(EventDispatcher *dispatcher)
{
    _dispatchers.append(dispatcher);
}

QVector<EventDispatcher*>& WDEventDispatcher::getDispatchers()
{
    return _dispatchers;
}

bool WDEventDispatcher::dispatch(QEvent *event)
{
    bool consumed = false;
    QVector<EventDispatcher*> dispatchers = WDEventDispatcher::getInstance()->getDispatchers();
    foreach (EventDispatcher* item, dispatchers)
    {
        consumed |= item->dispatch(event, consumed);
    }

    return consumed;
}
