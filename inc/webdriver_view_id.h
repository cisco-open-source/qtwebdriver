// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WEBDRIVER_WEBDRIVER_VIEW_ID_H_
#define WEBDRIVER_WEBDRIVER_VIEW_ID_H_

#include <string>

#include "base/memory/ref_counted.h"    

namespace webdriver {

//typedef void* ViewHandle;
typedef unsigned long int ViewType;
//#define INVALID_HANDLE  (0)

/// This class represents a Webdriver View handle.
/// It keeps implementation's dependent view pointer or object.
/// Customizer should inherit this class and implement.
class ViewHandle : public base::RefCounted<ViewHandle>{
public:
    ViewHandle() {};

    /// @return true if handle references valid view
    virtual bool is_valid() const = 0;
    /// compares two handle
    /// @param other handle to compare
    /// @return true if two handles reference same view
    virtual bool equals(const ViewHandle* other) const = 0;
protected:
    friend class base::RefCounted<ViewHandle>;
    virtual ~ViewHandle() {};
};

typedef scoped_refptr<ViewHandle> ViewHandlePtr;

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

    bool operator==(const ViewId &other) const;

    bool operator!=(const ViewId &other) const;

    bool operator<(const ViewId &other) const;

private:
    std::string id_;
    bool is_valid_;
};

}  // namespace webdriver

#endif  // WEBDRIVER_WEBDRIVER_VIEW_ID_H_
