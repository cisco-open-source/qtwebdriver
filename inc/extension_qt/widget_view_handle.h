// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WEBDRIVER_QT_WIDGET_VIEW_H_
#define WEBDRIVER_QT_WIDGET_VIEW_H_

#include <string>

#include "webdriver_view_id.h"

#include <QtCore/QPointer>

#include <QtCore/QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets/QWidget>
#include <QtWidgets/QAction>
#else
#include <QtGui/QWidget>
#include <QtGui/QAction>
#endif

namespace webdriver {

class QViewHandle : public ViewHandle {
public:
    QViewHandle();
    QViewHandle(QWidget* view);
    
    virtual bool is_valid() const { return !view_.isNull(); };
    virtual bool equals(const ViewHandle* other) const;
    QWidget* get() { return view_.data(); };
    
protected:
    QPointer<QWidget> view_;
//private:
	virtual ~QViewHandle() {};
};

}  // namespace webdriver

#endif  // WEBDRIVER_QT_WIDGET_VIEW_H_
