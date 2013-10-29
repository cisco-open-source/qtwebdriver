// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WEBDRIVER_QT_KEY_CONVERTER_H_
#define WEBDRIVER_QT_KEY_CONVERTER_H_

#include <string>
#include <vector>

#include "base/string16.h"

#include <QtGui/QKeyEvent>

namespace webdriver {

class Logger;

class QKeyConverter {
public:

	/// Converts keys into appropriate |WebKeyEvent|s. This will do a best effort
	/// conversion. However, if the input is invalid it will return false and set
	/// an error message. If |release_modifiers| is true, add an implicit NULL
	/// character to the end of the input to depress all modifiers. |modifiers|
	/// acts both an input and an output, however, only when the conversion
	/// process is successful will |modifiers| be changed.
    static bool ConvertKeysToWebKeyEvents(const string16& client_keys,
                               const Logger& logger,
                               bool release_modifiers,
                               int* modifiers,
                               std::vector<QKeyEvent>* client_key_events,
                               std::string* error_msg);
private:
    QKeyConverter() {};
    ~QKeyConverter() {};

    struct ModifierMaskAndKeyCode {
    	int mask;
    	Qt::Key key_code;
	};

    static const ModifierMaskAndKeyCode kModifiers[];
    static const Qt::Key kSpecialWebDriverKeys[];

    static bool IsModifierKey(char16 key);
    static bool KeyCodeFromSpecialWebDriverKey(char16 key, Qt::Key* key_code);
    static bool KeyCodeFromShorthandKey(char16 key_utf16,
                             Qt::Key* key_code,
                             bool* client_should_skip);

};


}  // namespace webdriver

#endif  // WEBDRIVER_QT_KEY_CONVERTER_H_
