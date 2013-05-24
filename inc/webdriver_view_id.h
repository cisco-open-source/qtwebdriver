// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WEBDRIVER_WEBDRIVER_VIEW_ID_H_
#define WEBDRIVER_WEBDRIVER_VIEW_ID_H_

#include <string>

namespace webdriver {

typedef void* ViewHandle;
typedef unsigned long int ViewType;
#define INVALID_HANDLE  (0)

/// This class represents a Webdriver View ID. These IDs are mapped to objects in a page.
class ViewId {
public:
    /// Creates an invalid ID.
    ViewId();

    /// Creates a valid ViewId using the ID of an view.
    /// @param id view Id
    explicit ViewId(const std::string& id);

    ~ViewId();

    /// Returns whether this ID is valid.
    bool is_valid() const;

    /// Returns string form of id
    const std::string& id() const { return id_; }

private:
    std::string id_;
    bool is_valid_;
};

}  // namespace webdriver

#endif  // WEBDRIVER_WEBDRIVER_VIEW_ID_H_
