/****************************************************************************
**
** Copyright © 1992-2014 Cisco and/or its affiliates. All rights reserved.
** All rights reserved.
** 
** $CISCO_BEGIN_LICENSE:LGPL$
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** $CISCO_END_LICENSE$
**
****************************************************************************/

/*! \file uinput_manager.h
    \page UInput manager
  UInput manager - WebDriver module, which allows sending events to Linux OS.
  UInput manager register new device as /dev/uinput and transmit events through it.

  Currently supported keys for Remote Control:
    0xE000U   POWER
    0xEF00U   MENU
    0xE002U   BACK
    0xE100U    UP
    0xE101U    DOWN
    0xE102U    LEFT
    0xE103U    RIGHT
    0xE001U    OK
    0xE00EU    INFO
    0xE00FU    TEXT
    0xE403U    RECOERD
    0xE402U    STOP
    0xE301U    ONE
    0xE302U    TWO
    0xE303U    THREE
    0xE304U    FOUR
    0xE305U    FIVE
    0xE306U    SIX
    0xE307U    SEVEN
    0xE308U    EIGHT
    0xE309U    NINE
    0xE300U    ZERO
    0xEE01U    COMPANION_DEVICE_KEY_LIVE_SWIPE
    0xEE02U    COMPANION_DEVICE_KEY_VOD_SWIPE
    0xEE03U    COMPANION_DEVICE_KEY_PAD_UP
    0xEE04U    COMPANION_DEVICE_KEY_PAD_DOWN
    0xEE05U    COMPANION_DEVICE_KEY_PAD_LEFT
    0xEE06U    COMPANION_DEVICE_KEY_PAD_RIGHT
*/

#ifndef UINPUT_MANAGER_H
#define UINPUT_MANAGER_H

#ifdef OS_LINUX

#include <QtGui/QKeyEvent>

#include "webdriver_logging.h"

using namespace webdriver;

/**
  Implements functionality for using user input device
 */
class UInputManager
{
public:

    /**
      Returns pointer to UInputManager. Will create new instance, if it not exist
      @return pointer to @sa UInputManager
     */
    static UInputManager* getInstance();

    /**
      Destructor
     */
    ~UInputManager();

    /**
      Register user input devices in OS
      @return true, if device registered successfully, else - false
     */
    bool registerUinputDevice();

    /**
      Send key event to device
      @param event - pointer to event data
      @return keycode value
     */
    int injectKeyEvent(QKeyEvent *event);

    /**
      Check wether uinput device initialized
      @return true, if device initialized, false if not
     */
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
