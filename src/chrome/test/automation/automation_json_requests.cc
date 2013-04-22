// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/test/automation/automation_json_requests.h"

KeyEvent::KeyEvent(automation::KeyEventTypes type,
                         ui::KeyboardCode key_code,
                         const std::string& unmodified_text,
                         const std::string& modified_text,
                         int modifiers)
    : type(type),
      key_code(key_code),
      unmodified_text(unmodified_text),
      modified_text(modified_text),
      modifiers(modifiers) {}

MouseEvent::MouseEvent(automation::MouseEventType type,
                             automation::MouseButton button,
                             int x,
                             int y,
                             int click_count,
                             int modifiers)
    : type(type),
      button(button),
      x(x),
      y(y),
      click_count(click_count),
      modifiers(modifiers) {}

// static
//WebViewId WebViewId::ForView(const AutomationId& view_id, QWebView *view)
ViewId ViewId::ForView(const AutomationId& view_id)
{
  ViewId id;
  id.old_style_ = false;
  id.id_ = view_id;
  return id;
}

ViewId  ViewId::ForQtView(QWidget *view, int automationId)
{
    ViewId id;
    id.old_style_ = false;

    QWebView* pView = qobject_cast<QWebView*>(view);
    if (pView != NULL)
    {
        AutomationId aId(AutomationId::kTypeTab, QString::number(automationId).toStdString());
        id.id_ = aId;
    }
    else
    {
        AutomationId aId(AutomationId::kTypeAppShell, QString::number(automationId).toStdString());
        id.id_ = aId;
    }
    return id;
}


// static
ViewId ViewId::ForOldStyleTab(int tab_id)
{
  ViewId id;
  id.old_style_ = true;
  id.tab_id_ = tab_id;
  //TODO: get *view by tab_id
  //id.webView = view;
  return id;
}

ViewId::ViewId()
    : old_style_(true)
{}

bool ViewId::IsValid() const {
  if (old_style_)
    return tab_id_ != 0;
  else
    return id_.is_valid();
}

AutomationId ViewId::GetId() const {
  /*if (old_style_)
    return AutomationId(AutomationId::kTypeTab, base::IntToString(tab_id_));
  else*/
    return id_;
}

bool ViewId::IsTab() const {
  return old_style_ || id_.type() == AutomationId::kTypeTab;
}

bool ViewId::IsApp() const {
  return id_.type() == AutomationId::kTypeAppShell;
}

int ViewId::tab_id() const {
  return tab_id_;
}

bool ViewId::old_style() const
{
  return old_style_;
}
