// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "q_key_converter.h"

#include "base/format_macros.h"
#include "base/logging.h"
#include "base/stringprintf.h"
#include "base/utf_string_conversions.h"
//#include "chrome/test/webdriver/keycode_text_conversion.h"
#include "webdriver_logging.h"

#include <QtCore/QDebug>
#include <QtGui/QKeySequence>

namespace webdriver {

const QKeyConverter::ModifierMaskAndKeyCode QKeyConverter::kModifiers[] = {
    { Qt::ShiftModifier,      Qt::Key_Shift },
    { Qt::ControlModifier,    Qt::Key_Control },
    { Qt::AltModifier,        Qt::Key_Alt }
};

// TODO(kkania): Use this in KeyMap.
// Ordered list of all the key codes corresponding to special WebDriver keys.
// These WebDriver keys are defined in the Unicode Private Use Area.
// http://code.google.com/p/selenium/wiki/JsonWireProtocol#/session/:sessionId/element/:id/value
const Qt::Key QKeyConverter::kSpecialWebDriverKeys[] = {
    Qt::Key_unknown,
    Qt::Key_unknown,
    Qt::Key_Help,
    Qt::Key_Backspace,
    Qt::Key_Tab,
    Qt::Key_Clear,
    Qt::Key_Return,
    Qt::Key_Enter,
    Qt::Key_Shift,
    Qt::Key_Control,
    Qt::Key_Menu,
    Qt::Key_Menu,
    Qt::Key_Escape,
    Qt::Key_Space,
    Qt::Key_PageUp,    // page up
    Qt::Key_PageDown,     // page down
    Qt::Key_End,
    Qt::Key_Home,
    Qt::Key_Left,
    Qt::Key_Up,
    Qt::Key_Right,
    Qt::Key_Down,
    Qt::Key_Insert,
    Qt::Key_Delete,
    Qt::Key_Semicolon,     // semicolon
    Qt::Key_Equal,  // equals
    Qt::Key_0,
    Qt::Key_1,
    Qt::Key_2,
    Qt::Key_3,
    Qt::Key_4,
    Qt::Key_5,
    Qt::Key_6,
    Qt::Key_7,
    Qt::Key_8,
    Qt::Key_9,
    Qt::Key_Asterisk,
    Qt::Key_Plus,
    Qt::Key_Comma,
    Qt::Key_Minus,
    Qt::Key_Period,
    Qt::Key_Slash,
    Qt::Key_unknown,
    Qt::Key_unknown,
    Qt::Key_unknown,
    Qt::Key_unknown,
    Qt::Key_unknown,
    Qt::Key_unknown,
    Qt::Key_unknown,
    Qt::Key_F1,
    Qt::Key_F2,
    Qt::Key_F3,
    Qt::Key_F4,
    Qt::Key_F5,
    Qt::Key_F6,
    Qt::Key_F7,
    Qt::Key_F8,
    Qt::Key_F9,
    Qt::Key_F10,
    Qt::Key_F11,
    Qt::Key_F12};

const char16 kWebDriverNullKey = 0xE000U;
const char16 kWebDriverShiftKey = 0xE008U;
const char16 kWebDriverControlKey = 0xE009U;
const char16 kWebDriverAltKey = 0xE00AU;
const char16 kWebDriverCommandKey = 0xE03DU;

/// Returns whether the given key is a WebDriver key modifier.
bool QKeyConverter::IsModifierKey(char16 key) {
    switch (key) {
        case kWebDriverShiftKey:
        case kWebDriverControlKey:
        case kWebDriverAltKey:
        case kWebDriverCommandKey:
            return true;
        default:
            return false;
    }
}

/// Gets the key code associated with |key|, if it is a special WebDriver key.
/// Returns whether |key| is a special WebDriver key. If true, |key_code| will
/// be set.
bool QKeyConverter::KeyCodeFromSpecialWebDriverKey(char16 key, Qt::Key* key_code) {
    int index = static_cast<int>(key) - 0xE000U;
    bool is_special_key = index >= 0 &&
        index < static_cast<int>(arraysize(kSpecialWebDriverKeys));
    if (is_special_key)
        *key_code = kSpecialWebDriverKeys[index];
    else {
        // Key_Escape = 0x01000000. Offset from this for undefined keys
        int qtValue = 0x01000000 + index; 
        *key_code =  static_cast<Qt::Key>(qtValue);
    }
    return is_special_key;
}

/// Gets the key code associated with |key|, if it is a special shorthand key.
/// Shorthand keys are common text equivalents for keys, such as the newline
/// character, which is shorthand for the return key. Returns whether |key| is
/// a shorthand key. If true, |key_code| will be set and |client_should_skip|
/// will be set to whether the key should be skipped.
bool QKeyConverter::KeyCodeFromShorthandKey(char16 key_utf16,
                             Qt::Key* key_code,
                             bool* client_should_skip) {
    string16 key_str_utf16;
    key_str_utf16.push_back(key_utf16);
    std::string key_str_utf8 = UTF16ToUTF8(key_str_utf16);
    if (key_str_utf8.length() != 1)
        return false;
    bool should_skip = false;
    char key = key_str_utf8[0];
    if (key == '\n') {
        *key_code = Qt::Key_Return;
    } else if (key == '\t') {
        *key_code = Qt::Key_Tab;
    } else if (key == '\b') {
        *key_code = Qt::Key_Backspace;
    } else if (key == ' ') {
        *key_code = Qt::Key_Space;
    } else if (key == '\r') {
        *key_code = Qt::Key_unknown;
        should_skip = true;
    } else {
        return false;
    }
    *client_should_skip = should_skip;
    return true;
}


bool QKeyConverter::ConvertKeysToWebKeyEvents(const string16& client_keys,
                               const Logger& logger,
                               bool release_modifiers,
                               int* modifiers,
                               std::vector<QKeyEvent>* client_key_events,
                               std::string* error_msg) {
    std::vector<QKeyEvent> key_events;

    string16 keys = client_keys;
    // Add an implicit NULL character to the end of the input to depress all
    // modifiers.
    if (release_modifiers)
        keys.push_back(kWebDriverNullKey);

    //static_cast<Qt::KeyboardModifier>(modifiers)
    Qt::KeyboardModifiers sticky_modifiers(*modifiers);

    for (size_t i = 0, size = keys.size(); i < size; ++i) {
        char16 key = keys[i];

        if (key == kWebDriverNullKey) {
            // Release all modifier keys and clear |stick_modifiers|.
            if (sticky_modifiers & Qt::ShiftModifier)
                key_events.push_back(
                    QKeyEvent(QEvent::KeyRelease, Qt::Key_Shift, Qt::NoModifier));
            if (sticky_modifiers & Qt::ControlModifier)
                key_events.push_back(
                    QKeyEvent(QEvent::KeyRelease, Qt::Key_Control, Qt::NoModifier));
            if (sticky_modifiers & Qt::AltModifier)
                key_events.push_back(
                    QKeyEvent(QEvent::KeyRelease, Qt::Key_Alt, Qt::NoModifier));
            if (sticky_modifiers & Qt::MetaModifier)
                key_events.push_back(
                    QKeyEvent(QEvent::KeyRelease, Qt::Key_Meta, Qt::NoModifier));
            sticky_modifiers = Qt::NoModifier;
            continue;
        }


        if (IsModifierKey(key)) {
            // Press or release the modifier, and adjust |sticky_modifiers|.
            bool modifier_down = false;
            Qt::Key key_code = Qt::Key_unknown;
            if (key == kWebDriverShiftKey) {
                sticky_modifiers ^= Qt::ShiftModifier;
                modifier_down = sticky_modifiers & Qt::ShiftModifier;
                key_code = Qt::Key_Shift;
            } else if (key == kWebDriverControlKey) {
                sticky_modifiers ^= Qt::ControlModifier;
                modifier_down = sticky_modifiers & Qt::ControlModifier;
                key_code = Qt::Key_Control;
            } else if (key == kWebDriverAltKey) {
                sticky_modifiers ^= Qt::AltModifier;
                modifier_down = sticky_modifiers & Qt::AltModifier;
                key_code = Qt::Key_Alt;
            } else if (key == kWebDriverCommandKey) {
                sticky_modifiers ^= Qt::MetaModifier;
                modifier_down = sticky_modifiers & Qt::MetaModifier;
                key_code = Qt::Key_Meta;
            } else {
                NOTREACHED();
            }
            if (modifier_down)
                key_events.push_back(
                        QKeyEvent(QEvent::KeyPress, key_code, sticky_modifiers));
            else
                key_events.push_back(
                        QKeyEvent(QEvent::KeyRelease, key_code, sticky_modifiers));
            continue;
        }

        Qt::Key key_code = Qt::Key_unknown;
        std::string unmodified_text, modified_text;
        Qt::KeyboardModifiers all_modifiers = sticky_modifiers;

        // Get the key code, text, and modifiers for the given key.
        bool should_skip = false;
        if (KeyCodeFromSpecialWebDriverKey(key, &key_code) ||
            KeyCodeFromShorthandKey(key, &key_code, &should_skip)) {

            if (should_skip)
                continue;
            if (key_code == Qt::Key_unknown) {
                *error_msg = StringPrintf(
                    "Unknown WebDriver key(%d) at string index (%" PRIuS ")",
                    static_cast<int>(key), i);
                return false;
            }

            if (key_code == Qt::Key_Return || key_code == Qt::Key_Enter || key_code == Qt::Key_Space ||
                    key_code == Qt::Key_Asterisk || key_code == Qt::Key_Plus ||
                    key_code == Qt::Key_Comma || key_code == Qt::Key_Minus ||
                    key_code == Qt::Key_Period || key_code == Qt::Key_Slash ||
                    key_code == Qt::Key_Equal || key_code == Qt::Key_Semicolon) {
                // TODO: check this
                // For some reason Chrome expects a carriage return for the return key.
                modified_text = unmodified_text = (char)key_code;
            } else {
                // TODO: check this
                // WebDriver assumes a numpad key should translate to the number,
                // which requires NumLock to be on with some platforms. This isn't
                // formally in the spec, but is expected by their tests.
//                int webdriver_modifiers = 0;
//                if (key_code >= Qt::Key_0 && key_code <= Qt::Key_9)
//                    webdriver_modifiers = automation::kNumLockKeyMask;
//                unmodified_text = ConvertKeyCodeToText(key_code, webdriver_modifiers);
//                modified_text = ConvertKeyCodeToText(
//                    key_code,
//                    all_modifiers | webdriver_modifiers);
            }
        } else {
            Qt::KeyboardModifiers necessary_modifiers(0);
//            ConvertCharToKeyCode(key, &key_code, &necessary_modifiers);
            all_modifiers |= necessary_modifiers;
            if (key_code != Qt::Key_unknown) {
//              unmodified_text = ConvertKeyCodeToText(key_code, 0);
//              modified_text = ConvertKeyCodeToText(key_code, all_modifiers);
            }

            if (unmodified_text.empty() || modified_text.empty()) {
                // Do a best effort and use the raw key we were given.
                logger.Log(
                    kWarningLogLevel,
                    base::StringPrintf("No translation for key code. Code point: %d",
                        static_cast<int>(key)));
                if (unmodified_text.empty())
                    unmodified_text = UTF16ToUTF8(keys.substr(i, 1));
                if (modified_text.empty())
                    modified_text = UTF16ToUTF8(keys.substr(i, 1));
            }
        }

	bool autoPress = false, autoRelease = false;

        if (i < size - 1 && key == keys[i + 1]) 
            autoRelease = true;

        if (i > 0 && key == keys[i - 1]) 
            autoPress = true;

	bool sendRelease;
#if WD_ENABLE_ONE_KEYRELEASE 
        // Send only last key release
        sendRelease = !autoRelease;
 #else
        // Send key release in all cases
        sendRelease = true;
#endif
        // Create the key events.
        bool necessary_modifiers[3];
        for (int i = 0; i < 3; ++i) {
            necessary_modifiers[i] =
                all_modifiers & kModifiers[i].mask &&
                !(sticky_modifiers & kModifiers[i].mask);
            if (necessary_modifiers[i]) {
                key_events.push_back(
                    QKeyEvent(QEvent::KeyPress, kModifiers[i].key_code, sticky_modifiers, QString::null, autoPress));
            }
        }

        if (unmodified_text.length() || modified_text.length()) {
            key_events.push_back(QKeyEvent(QEvent::KeyPress, key_code, all_modifiers, unmodified_text.c_str(), autoPress));
            if (sendRelease) {
                key_events.push_back(QKeyEvent(QEvent::KeyRelease, key_code, all_modifiers, unmodified_text.c_str(), autoRelease));      
            }
        }
        else
        {
            key_events.push_back(QKeyEvent(QEvent::KeyPress, key_code, all_modifiers, QString::null, autoPress));
            if (sendRelease) {
                key_events.push_back(QKeyEvent(QEvent::KeyRelease, key_code, all_modifiers, QString::null, autoRelease));
            }
        }

        
        if (sendRelease) {
            for (int i = 2; i > -1; --i) {
                if (necessary_modifiers[i]) {
                    key_events.push_back(
                        QKeyEvent(QEvent::KeyRelease, kModifiers[i].key_code, sticky_modifiers, QString::null, autoRelease));
                }
            }
        }
    }

    client_key_events->swap(key_events);
    *modifiers = sticky_modifiers;
    return true;
}



}  // namespace webdriver
