#ifndef UINPUT_MANAGER_H
#define UINPUT_MANAGER_H

#ifdef OS_LINUX

#include <QtGui/QKeyEvent>

#include "webdriver_logging.h"

using namespace webdriver;

class UInputManager
{
public:

    static UInputManager* getInstance();

    ~UInputManager();

    bool registerUinputDevice();
    int injectKeyEvent(QKeyEvent *event);
    bool isReady();

private:
    UInputManager();
    void registerHandledKeys();
    int injectSynEvent();

private:
    int _deviceDescriptor;
    Logger *_logger;
    bool _isReady;

    static UInputManager* _instance;
};

#endif // OS_LINUX

#endif // UINPUT_MANAGER_H
