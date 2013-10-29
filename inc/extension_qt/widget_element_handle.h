// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WEBDRIVER_QT_WIDGET_ELEMENT_H_
#define WEBDRIVER_QT_WIDGET_ELEMENT_H_

#include <string>

#include "webdriver_element_id.h"

#include <QtCore/QPointer>


namespace webdriver {


class QElementHandle : public ElementHandle {
public:
    QElementHandle();
    QElementHandle(QObject* element);
    
    virtual bool is_valid() const { return !element_.isNull(); }
    virtual bool equals(const ElementHandle* other) const;
    QObject* get() { return element_.data(); }
    
protected:
    QPointer<QObject> element_;
    virtual ~QElementHandle() {}
};

}  // namespace webdriver

#endif  // WEBDRIVER_QT_WIDGET_ELEMENT_H_
