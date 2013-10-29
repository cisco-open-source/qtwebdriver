// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "webdriver_view_id.h"

namespace webdriver {

ViewId::ViewId() : is_valid_(false) {}

ViewId::ViewId(const std::string& id) : id_(id), is_valid_(true) {}

ViewId::~ViewId() {}

bool ViewId::is_valid() const {
    return is_valid_;
}    

bool ViewId::operator==(const ViewId &other) const {
	return (id_ == other.id());
}

bool ViewId::operator!=(const ViewId &other) const {
    return !(*this == other);
}

bool ViewId::operator<(const ViewId &other) const {
	return (id_ < other.id());
}


}  // namespace webdriver