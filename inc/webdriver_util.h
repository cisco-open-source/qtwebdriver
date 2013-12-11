// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WEBDRIVER_WEBDRIVER_UTIL_H_
#define WEBDRIVER_WEBDRIVER_UTIL_H_

#include <string>
#include <vector>

#include "base/basictypes.h"
#include "base/compiler_specific.h"
#include "base/values.h"
#include "value_conversion_traits.h"
#include "webdriver_error.h"

class FilePath;
class ViewId;

namespace webdriver {

/// Generates a random, 32-character hexidecimal ID.
std::string GenerateRandomID();

/// Decodes the given base64-encoded string, after removing any newlines,
/// which are required in some base64 standards.
/// Returns true on success.
bool Base64Decode(const std::string& base64, std::string* bytes);

/// Returns the equivalent JSON string for the given value.
std::string JsonStringify(const base::Value* value);

/// Returns the JSON string for the given value, with the exception that
/// long strings are shortened for easier display.
std::string JsonStringifyForDisplay(const base::Value* value);

/// Returns the string representation of the given type, for display purposes.
const char* GetJsonTypeName(base::Value::Type type);

/// Flattens the given list of strings into one.
Error* FlattenStringArray(const ListValue* src, string16* dest);

/// Gives string, e.g.-"400, 600", split it, and assigns given number to first and second.
/// Returns true on success.
bool GetTwoIntsFromString(const std::string& src, int& first, int& second);

#if defined(OS_MACOSX)
// Gets the paths to the user and local application directory.
void GetApplicationDirs(std::vector<FilePath>* app_dirs);
#endif

/// Parses a given value.
class ValueParser {
public:
    virtual ~ValueParser();
    virtual bool Parse(base::Value* value) const = 0;

protected:
    ValueParser();

private:
    DISALLOW_COPY_AND_ASSIGN(ValueParser);
};

/// Define a special type and constant that allows users to skip parsing a value.
/// Useful when wanting to skip parsing for one value out of many in a list.
enum SkipParsing { };
extern SkipParsing* kSkipParsing;

/// Parses a given value using the |ValueConversionTraits| to the appropriate
/// type. This assumes that a direct conversion can be performed without
/// pulling the value out of a dictionary or list.
template <typename T>
class DirectValueParser : public ValueParser {
public:
    explicit DirectValueParser(T* t) : t_(t) { }

    virtual ~DirectValueParser() { }

    virtual bool Parse(base::Value* value) const OVERRIDE {
        return ValueConversionTraits<T>::SetFromValue(value, t_);
  }

private:
    T* t_;
    DISALLOW_COPY_AND_ASSIGN(DirectValueParser);
};

/// Convenience function for creating a DirectValueParser.
template <typename T>
DirectValueParser<T>* CreateDirectValueParser(T* t) {
    return new DirectValueParser<T>(t);
}

}  // namespace webdriver

// Value conversion traits for SkipParsing, which just return true.
template <>
struct ValueConversionTraits<webdriver::SkipParsing> {
    static bool SetFromValue(const base::Value* value,
                            const webdriver::SkipParsing* t);
    static bool CanConvert(const base::Value* value);
};

#endif  // WEBDRIVER_WEBDRIVER_UTIL_H_
