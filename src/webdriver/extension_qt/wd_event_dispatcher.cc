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
