// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_TEST_AUTOMATION_AUTOMATION_JSON_REQUESTS_H_
#define CHROME_TEST_AUTOMATION_AUTOMATION_JSON_REQUESTS_H_

#include <string>
#include <vector>

#include <QtCore/QGlobalStatic>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWebKitWidgets/QWebView>
#else
#include <QtWebKit/QtWebKit>
#endif

#include "base/compiler_specific.h"
//#include "base/file_path.h"
//#include "base/string_number_conversions.h"
#include "chrome/common/automation_constants.h"
#include "chrome/common/automation_id.h"
#include "ui/base/keycodes/keyboard_codes.h"

struct KeyEvent {
  KeyEvent(automation::KeyEventTypes type,
              ui::KeyboardCode key_code,
              const std::string& unmodified_text,
              const std::string& modified_text,
              int modifiers);

  automation::KeyEventTypes type;
  ui::KeyboardCode key_code;
  std::string unmodified_text;
  std::string modified_text;
  int modifiers;
};

struct MouseEvent {
  MouseEvent(automation::MouseEventType type,
                automation::MouseButton button,
                int x,
                int y,
                int click_count,
                int modifiers);

  automation::MouseEventType type;
  automation::MouseButton button;
  int x;
  int y;
  int click_count;
  int modifiers;
};

// Uniquely identifies a particular WebView.
// This is needed because Chrome used to accept just tab IDs, while
// now it accepts IDs for other types of WebViews.
// TOOD(kkania): Remove this abstraction once Chrome 16 is unsupported.
class ViewId {
 public:
  // Creates an ID for the given view ID.
  static ViewId ForView(const AutomationId& view_id);

    // Creates an ID for the given view ID.
  static ViewId ForQtView(QWidget *view, int automationId);

  // Creates an ID for the given tab ID.
  static ViewId ForOldStyleTab(int tab_id);

  // Creates an invalid ID.
  ViewId();

  // Updates the given dictionary to include this ID. If the ID refers to a
  // view ID, |view_id_key| will be the key modified in the dictionary.
  //void UpdateDictionary(base::DictionaryValue* dictionary,
  //                      const std::string& view_id_key) const;

  // Returns whether this ID is valid. Even if it is valid, the object it
  // refers to may not exist.
  bool IsValid() const;

  // Returns an |AutomationId| made from this ID.
  AutomationId GetId() const;

  // Returns whether this ID refers to a tab.
  bool IsTab() const;

  // Returns whether this ID refers to app.
  bool IsApp() const;

  int tab_id() const;

  // The old style is to use a single integer ID for a tab. The new style is
  // to use an automation ID which may refer to a number of different object
  // types.
  bool old_style() const;

    // return pointer to view widget
//  QWidget* GetView() const;

 private:
  // Whether this ID is an old-style integer tab ID.
  bool old_style_;

  AutomationId id_;
  int tab_id_;
};

#endif  // CHROME_TEST_AUTOMATION_AUTOMATION_JSON_REQUESTS_H_
