// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WEBDRIVER_WEBDRIVER_ELEMENT_ID_H_
#define WEBDRIVER_WEBDRIVER_ELEMENT_ID_H_

#include <string>

#include "base/memory/ref_counted.h"

#include "value_conversion_traits.h"

namespace base {
class Value;
}

namespace webdriver {

/// This class represents a Webdriver Element handle. Custom implementation should inherit and implement.
class ElementHandle : public base::RefCounted<ElementHandle>{
public:
    ElementHandle() {};

    /// @return true if handle references valid element (widget)
    virtual bool is_valid() const = 0;
    /// compares two handle
    /// @param other handle to compare
    /// @return true if two handles reference same element(widget)
    virtual bool equals(const ElementHandle* other) const = 0;
protected:
    friend class base::RefCounted<ElementHandle>;
    virtual ~ElementHandle() {};    
};

/// This class represents a WebDriver Element ID. These IDs are mapped to
/// objects in a page.
class ElementId {
public:
    /// Creates an invalid ElementId.
    ElementId();

    /// Creates a valid ElementId using the ID of an element. An empty string
    /// can be used to refer to the root document of the page.
    /// @param id elementId
    explicit ElementId(const std::string& id);

    /// Creates a ElementId from an element dictionary returned by a WebDriver
    /// atom. It will be valid if the dictionary is correctly constructed.
    /// @param value elementId from parsed JSON
    explicit ElementId(const base::Value* value);

    ~ElementId();

    /// Returns the appropriate Value type to be used to identify the element
    /// to a WebDriver atom. The client takes ownership.
    base::Value* ToValue() const;

    /// Returns whether this ID is valid. Even if the ID is valid, it may not refer
    /// to a valid ID on the page.
    bool is_valid() const;

    /// Returns string form of id
    const std::string& id() const { return id_; }

private:
    std::string id_;
    bool is_valid_;
};

/// WebDriver element locators. These constants are used to identify different
/// ways to locate an element to WebDriver atoms. Struct is used for grouping
/// purposes.
struct LocatorType {
    static const char kClassName[];
    static const char kCss[];
    static const char kId[];
    static const char kLinkText[];
    static const char kName[];
    static const char kPartialLinkText[];
    static const char kTagName[];
    static const char kXpath[];
};

}  // namespace webdriver

template <>
struct ValueConversionTraits<webdriver::ElementId> {
    static base::Value* CreateValueFrom(const webdriver::ElementId& t);
    static bool SetFromValue(
        const base::Value* value, webdriver::ElementId* t);
    static bool CanConvert(const base::Value* value);
};

#endif  // WEBDRIVER_WEBDRIVER_ELEMENT_ID_H_
