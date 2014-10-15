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

#include "build/build_config.h"

#if defined(OS_LINUX)

#include "extension_qt/uinput_manager.h"

#include <linux/input.h>
#include <linux/uinput.h>
#include <QtCore/qnamespace.h>
#include <fcntl.h>
#include <stdio.h>

static int lookup_code(int keysym);

UInputManager* UInputManager::_instance = NULL;

UInputManager* UInputManager::getInstance()
{
    if (NULL == _instance)
        _instance = new UInputManager();

    return _instance;
}

UInputManager::UInputManager()
    : _deviceDescriptor(0),
      _isReady(false)
{
    _logger = new Logger();
}

UInputManager::~UInputManager()
{
    delete _logger;
    ioctl(_deviceDescriptor, UI_DEV_DESTROY);   // try destroy device
}

bool UInputManager::registerUinputDevice()
{
    struct uinput_user_dev uidev;

    _deviceDescriptor = open("/dev/uinput", O_WRONLY | O_NONBLOCK | O_CREAT | O_NDELAY, S_IREAD | S_IWRITE);
    _logger->Log(kInfoLogLevel, std::string("#### Device descriptor: ") + QString::number(_deviceDescriptor).toStdString());

    if (0 > _deviceDescriptor)
    {
        _logger->Log(kWarningLogLevel, "Can't open uinput device");
        return false;
    }

    // enable Key and Synchronization events
    int ret = ioctl(_deviceDescriptor, UI_SET_EVBIT, EV_KEY);
    if (0 > ret)
    {
        _logger->Log(kWarningLogLevel, "Can't register uinput key events");
        return false;
    }
    ret = ioctl(_deviceDescriptor, UI_SET_EVBIT, EV_SYN);
    if (0 > ret)
    {
        _logger->Log(kWarningLogLevel, "Can't register uinput synchronization events");
        return false;
    }

    // initialize device info
    memset(&uidev, 0, sizeof(uidev));
    uidev.id.bustype = BUS_USB;
    uidev.id.version = 0x01;
    snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "wd_key_input");
    ret = write(_deviceDescriptor, &uidev, sizeof(uidev));

    if (0 > ret)
    {
        _logger->Log(kWarningLogLevel, "Can not initialize user input device");
        return false;
    }
    registerHandledKeys();

    ret = ioctl(_deviceDescriptor, UI_DEV_CREATE); // create device
    if (0 > ret)
    {
        _logger->Log(kWarningLogLevel, "Can not create user input device");
        return false;
    }

    _isReady = true;
    return true;
}

int UInputManager::injectKeyEvent(QKeyEvent *event)
{
    struct input_event ev;
    int res = -1;

    memset(&ev, 0, sizeof(ev));

    gettimeofday(&(ev.time), NULL);

    if (QKeyEvent::KeyPress == event->type())
    {
        ev.value = 1;
    }
    else if (QKeyEvent::KeyRelease == event->type())
    {
        ev.value = 0;
    }
    printf("#### Key code: %d\n", event->key());
    printf("#### Key text: %s, modifiers: %d\n", event->text().toStdString().c_str(), (int)event->modifiers());
    int key_text = event->text().toStdString().c_str()[0];

    // Check keyCode for capital letters
    if ((key_text>='>' && key_text<='Z') ||       // '>','?','@'  included
            (key_text>='!' && key_text<='&') ||   // '!' - '&'
            (key_text>='(' && key_text<='+') ||    // '(' - '+'
            (key_text>='^' && key_text<='_') ||    // '^','_'
            (key_text>='{' && key_text<='}') ||    // '{' - '}'
            '<' == key_text )
    {
        ev.type = EV_KEY;
        ev.code = KEY_RIGHTSHIFT;
        res = write(_deviceDescriptor, &ev, sizeof(ev));
    }

    ev.type = EV_KEY;
    ev.code = lookup_code(event->key());

    res = write(_deviceDescriptor, &ev, sizeof(ev));
    _logger->Log(kInfoLogLevel, std::string("#### Write event time: ") +
                 QString::number(ev.time.tv_sec).toStdString() + std::string(".") +
                 QString::number(ev.time.tv_usec).toStdString() + std::string(", res: ") +
                 QString::number(res).toStdString() + std::string(", errno: ") +
                 QString::number(errno).toStdString() );

    ev.type = EV_SYN;
    ev.code = SYN_REPORT;
    ev.value = 0;

    res = write(_deviceDescriptor, &ev, sizeof(ev));

    return res;
}

void UInputManager::registerHandledKeys()
{
    // set range of keys
    for (int i=0; i<256; i++)
    {
        ioctl(_deviceDescriptor, UI_SET_KEYBIT, i);
    }
    ioctl(_deviceDescriptor, UI_SET_KEYBIT, 0xE000U);   // POWER
    ioctl(_deviceDescriptor, UI_SET_KEYBIT, 0xEF00U);   // MENU
    ioctl(_deviceDescriptor, UI_SET_KEYBIT, 0xE002U);   // BACK
    ioctl(_deviceDescriptor, UI_SET_KEYBIT, 0xE100U);   // UP
    ioctl(_deviceDescriptor, UI_SET_KEYBIT, 0xE101U);   // DOWN
    ioctl(_deviceDescriptor, UI_SET_KEYBIT, 0xE102U);   // LEFT
    ioctl(_deviceDescriptor, UI_SET_KEYBIT, 0xE103U);   // RIGHT
    ioctl(_deviceDescriptor, UI_SET_KEYBIT, 0xE001U);   // OK
    ioctl(_deviceDescriptor, UI_SET_KEYBIT, 0xE00EU);   // INFO
    ioctl(_deviceDescriptor, UI_SET_KEYBIT, 0xE00FU);   // TEXT
    ioctl(_deviceDescriptor, UI_SET_KEYBIT, 0xE403U);   // RECOERD
    ioctl(_deviceDescriptor, UI_SET_KEYBIT, 0xE402U);   // STOP
    ioctl(_deviceDescriptor, UI_SET_KEYBIT, 0xE301U);   // ONE
    ioctl(_deviceDescriptor, UI_SET_KEYBIT, 0xE302U);   // TWO
    ioctl(_deviceDescriptor, UI_SET_KEYBIT, 0xE303U);   // THREE
    ioctl(_deviceDescriptor, UI_SET_KEYBIT, 0xE304U);   // FOUR
    ioctl(_deviceDescriptor, UI_SET_KEYBIT, 0xE305U);   // FIVE
    ioctl(_deviceDescriptor, UI_SET_KEYBIT, 0xE306U);   // SIX
    ioctl(_deviceDescriptor, UI_SET_KEYBIT, 0xE307U);   // SEVEN
    ioctl(_deviceDescriptor, UI_SET_KEYBIT, 0xE308U);   // EIGHT
    ioctl(_deviceDescriptor, UI_SET_KEYBIT, 0xE309U);   // NINE
    ioctl(_deviceDescriptor, UI_SET_KEYBIT, 0xE300U);   // ZERO
    ioctl(_deviceDescriptor, UI_SET_KEYBIT, 0xEE01U);   // COMPANION_DEVICE_KEY_LIVE_SWIPE
    ioctl(_deviceDescriptor, UI_SET_KEYBIT, 0xEE02U);   // COMPANION_DEVICE_KEY_VOD_SWIPE
    ioctl(_deviceDescriptor, UI_SET_KEYBIT, 0xEE03U);   // COMPANION_DEVICE_KEY_PAD_UP
    ioctl(_deviceDescriptor, UI_SET_KEYBIT, 0xEE04U);   // COMPANION_DEVICE_KEY_PAD_DOWN
    ioctl(_deviceDescriptor, UI_SET_KEYBIT, 0xEE05U);   // COMPANION_DEVICE_KEY_PAD_LEFT
    ioctl(_deviceDescriptor, UI_SET_KEYBIT, 0xEE06U);   // COMPANION_DEVICE_KEY_PAD_RIGHT
}

bool UInputManager::isReady()
{
    return _isReady;
}

int UInputManager::injectSynEvent()
{
    struct input_event ev;

    memset(&ev, 0, sizeof(ev));

    ev.type = EV_SYN;
    ev.code = SYN_REPORT;
    ev.value = 0;
    gettimeofday(&(ev.time), NULL);

    int res = write(_deviceDescriptor, &ev, sizeof(ev));

    return res;
}

static int lookup_code(int keysym) {

    switch(keysym) {
    case Qt::Key_Escape:	return KEY_ESC;
    case Qt::Key_1:         return KEY_1;
    case Qt::Key_2:         return KEY_2;
    case Qt::Key_3:         return KEY_3;
    case Qt::Key_4:         return KEY_4;
    case Qt::Key_5:         return KEY_5;
    case Qt::Key_6:         return KEY_6;
    case Qt::Key_7:         return KEY_7;
    case Qt::Key_8:         return KEY_8;
    case Qt::Key_9:         return KEY_9;
    case Qt::Key_0:         return KEY_0;
    case Qt::Key_Exclam:	return KEY_1;
    case Qt::Key_At:		return KEY_2;
    case Qt::Key_NumberSign:	return KEY_3;
    case Qt::Key_Dollar:	return KEY_4;
    case Qt::Key_Percent:	return KEY_5;
    case Qt::Key_AsciiCircum:	return KEY_6;
    case Qt::Key_Ampersand:	return KEY_7;
    case Qt::Key_Asterisk:	return KEY_8;
    case Qt::Key_ParenLeft:	return KEY_9;
    case Qt::Key_ParenRight:	return KEY_0;
    case Qt::Key_Minus:     return KEY_MINUS;
    case Qt::Key_Underscore:	return KEY_MINUS;
    case Qt::Key_Equal:     return KEY_EQUAL;
    case Qt::Key_Plus:      return KEY_EQUAL;
    case Qt::Key_Backspace:	return KEY_BACKSPACE;
    case Qt::Key_Tab:		return KEY_TAB;
    case Qt::Key_Q:         return KEY_Q;
    case Qt::Key_W:         return KEY_W;
    case Qt::Key_E:         return KEY_E;
    case Qt::Key_R:         return KEY_R;
    case Qt::Key_T:         return KEY_T;
    case Qt::Key_Y:         return KEY_Y;
    case Qt::Key_U:         return KEY_U;
    case Qt::Key_I:         return KEY_I;
    case Qt::Key_O:         return KEY_O;
    case Qt::Key_P:         return KEY_P;
    case Qt::Key_BraceLeft:     return KEY_LEFTBRACE;
    case Qt::Key_BraceRight:	return KEY_RIGHTBRACE;
    case Qt::Key_BracketLeft:	return KEY_LEFTBRACE;
    case Qt::Key_BracketRight:	return KEY_RIGHTBRACE;
    case Qt::Key_Return:	return KEY_ENTER;
    case Qt::Key_Control:	return KEY_LEFTCTRL;
    case Qt::Key_A:         return KEY_A;
    case Qt::Key_S:         return KEY_S;
    case Qt::Key_D:         return KEY_D;
    case Qt::Key_F:         return KEY_F;
    case Qt::Key_G:         return KEY_G;
    case Qt::Key_H:         return KEY_H;
    case Qt::Key_J:         return KEY_J;
    case Qt::Key_K:         return KEY_K;
    case Qt::Key_L:         return KEY_L;
    case Qt::Key_Semicolon:      return KEY_SEMICOLON;
    case Qt::Key_Colon:     return KEY_SEMICOLON;
    case Qt::Key_Apostrophe:	return KEY_APOSTROPHE;
    case Qt::Key_QuoteDbl:	return KEY_APOSTROPHE;
    case Qt::Key_AsciiTilde:	return KEY_GRAVE;
    case Qt::Key_Shift:     return KEY_LEFTSHIFT;
    case Qt::Key_Backslash:	return KEY_BACKSLASH;
    case Qt::Key_Bar:		return KEY_BACKSLASH;
    case Qt::Key_Z:         return KEY_Z;
    case Qt::Key_X:         return KEY_X;
    case Qt::Key_C:         return KEY_C;
    case Qt::Key_V:         return KEY_V;
    case Qt::Key_B:         return KEY_B;
    case Qt::Key_N:         return KEY_N;
    case Qt::Key_M:         return KEY_M;
    case Qt::Key_Comma:     return KEY_COMMA;
    case Qt::Key_Less:      return KEY_COMMA;
    case Qt::Key_Period:	return KEY_DOT;
    case Qt::Key_Greater:	return KEY_DOT;
    case Qt::Key_Slash:     return KEY_SLASH;
    case Qt::Key_Question:	return KEY_SLASH;
    case Qt::Key_Alt:       return KEY_LEFTALT;
    case Qt::Key_Space:     return KEY_SPACE;
    case Qt::Key_CapsLock:	return KEY_CAPSLOCK;
    case Qt::Key_F1:		return KEY_F1;
    case Qt::Key_F2:		return KEY_F2;
    case Qt::Key_F3:		return KEY_F3;
    case Qt::Key_F4:		return KEY_F4;
    case Qt::Key_F5:		return KEY_F5;
    case Qt::Key_F6:		return KEY_F6;
    case Qt::Key_F7:		return KEY_F7;
    case Qt::Key_F8:		return KEY_F8;
    case Qt::Key_F9:		return KEY_F9;
    case Qt::Key_F10:		return KEY_F10;
    case Qt::Key_NumLock:	return KEY_NUMLOCK;
    case Qt::Key_ScrollLock:	return KEY_SCROLLLOCK;
    case Qt::Key_Enter:     return KEY_KPENTER;
    case Qt::Key_SysReq:	return KEY_SYSRQ;
    case Qt::Key_Home:		return KEY_HOME;
    case Qt::Key_Up:		return KEY_UP;
    case Qt::Key_PageUp:	return KEY_PAGEUP;
    case Qt::Key_Left:		return KEY_LEFT;
    case Qt::Key_Right:     return KEY_RIGHT;
    case Qt::Key_End:		return KEY_END;
    case Qt::Key_Down:		return KEY_DOWN;
    case Qt::Key_PageDown:	return KEY_PAGEDOWN;
    case Qt::Key_Insert:	return KEY_INSERT;
    case Qt::Key_Delete:	return KEY_DELETE;
    case Qt::Key_Pause:     return KEY_PAUSE;
    case Qt::Key_Meta:      return KEY_LEFTMETA;

    default:		return keysym;
    }
}

#endif // OS_LINUX
