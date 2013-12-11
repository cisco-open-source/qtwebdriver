// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "webdriver_util.h"

#include "base/base64.h"
#include "base/basictypes.h"
#include "base/file_util.h"
#include "base/format_macros.h"
#include "base/json/json_reader.h"
#include "base/json/json_writer.h"
#include "base/memory/scoped_ptr.h"
#include "base/rand_util.h"
#include "base/scoped_temp_dir.h"
#include "base/stringprintf.h"
#include "base/string_number_conversions.h"
#include "base/string_split.h"
#include "base/string_util.h"
#include "base/third_party/icu/icu_utf.h"

using base::DictionaryValue;
using base::ListValue;
using base::Value;

namespace webdriver {

SkipParsing* kSkipParsing = NULL;

std::string GenerateRandomID() {
    uint64 msb = base::RandUint64();
    uint64 lsb = base::RandUint64();
    return base::StringPrintf("%016" PRIx64 "%016" PRIx64, msb, lsb);
}

bool Base64Decode(const std::string& base64,
                  std::string* bytes) {
    std::string copy = base64;
    // Some WebDriver client base64 encoders follow RFC 1521, which require that
    // 'encoded lines be no more than 76 characters long'. Just remove any
    // newlines.
    RemoveChars(copy, "\n", &copy);
    return base::Base64Decode(copy, bytes);
}

std::string JsonStringify(const Value* value) {
    std::string json;
    base::JSONWriter::Write(value, &json);
    return json;
}

namespace {

// Truncates the given string to 100 characters, adding an ellipsis if
// truncation was necessary.
void TruncateString(std::string* data) {
    const size_t kMaxLength = 100;
    if (data->length() > kMaxLength) {
        data->resize(kMaxLength);
        data->replace(kMaxLength - 3, 3, "...");
    }
}

// Truncates all strings contained in the given value.
void TruncateContainedStrings(Value* value) {
    ListValue* list;
    if (value->IsType(Value::TYPE_DICTIONARY)) {
        DictionaryValue* dict = static_cast<DictionaryValue*>(value);
        DictionaryValue::key_iterator key = dict->begin_keys();
        for (; key != dict->end_keys(); ++key) {
            Value* child;
            if (!dict->GetWithoutPathExpansion(*key, &child))
                continue;
            std::string data;
            if (child->GetAsString(&data)) {
                TruncateString(&data);
                dict->SetWithoutPathExpansion(*key, Value::CreateStringValue(data));
            } else {
                TruncateContainedStrings(child);
            }
        }
    } else if (value->GetAsList(&list)) {
        for (size_t i = 0; i < list->GetSize(); ++i) {
            Value* child;
            if (!list->Get(i, &child))
                continue;
            std::string data;
            if (child->GetAsString(&data)) {
                TruncateString(&data);
                list->Set(i, Value::CreateStringValue(data));
            } else {
                TruncateContainedStrings(child);
            }
        }
    }
}

}  // namespace

std::string JsonStringifyForDisplay(const Value* value) {
    scoped_ptr<Value> copy;
    if (value->IsType(Value::TYPE_STRING)) {
        std::string data;
        value->GetAsString(&data);
        TruncateString(&data);
        copy.reset(Value::CreateStringValue(data));
    } else {
        copy.reset(value->DeepCopy());
        TruncateContainedStrings(copy.get());
    }
    std::string json;
    base::JSONWriter::WriteWithOptions(copy.get(),
                                     base::JSONWriter::OPTIONS_PRETTY_PRINT,
                                     &json);
    return json;
}

const char* GetJsonTypeName(Value::Type type) {
    switch (type) {
    case Value::TYPE_NULL:
        return "null";
    case Value::TYPE_BOOLEAN:
        return "boolean";
    case Value::TYPE_INTEGER:
        return "integer";
    case Value::TYPE_DOUBLE:
        return "double";
    case Value::TYPE_STRING:
        return "string";
    case Value::TYPE_BINARY:
        return "binary";
    case Value::TYPE_DICTIONARY:
        return "dictionary";
    case Value::TYPE_LIST:
        return "list";
    }
    return "unknown";
}

Error* FlattenStringArray(const ListValue* src, string16* dest) {
    string16 keys;
    for (size_t i = 0; i < src->GetSize(); ++i) {
        string16 keys_list_part;
        src->GetString(i, &keys_list_part);
        for (size_t j = 0; j < keys_list_part.size(); ++j) {
            if (CBU16_IS_SURROGATE(keys_list_part[j])) {
                return new Error(kBadRequest,
                         "WebDriver only supports characters in the BMP");
            }
        }
        keys.append(keys_list_part);
    }
    *dest = keys;
    return NULL;
}

bool GetTwoIntsFromString(const std::string& src, int& first, int& second) {
    std::vector<std::string> vect;
    base::SplitString(src, ',', &vect);
    if (vect.size() == 2) {
        base::StringToInt(vect.at(0), &first);
        base::StringToInt(vect.at(1), &second);
        return true;
    }
    return false;
}

ValueParser::ValueParser() { }

ValueParser::~ValueParser() { }

}  // namespace webdriver

bool ValueConversionTraits<webdriver::SkipParsing>::SetFromValue(
        const Value* value, const webdriver::SkipParsing* t) {
    return true;
}

bool ValueConversionTraits<webdriver::SkipParsing>::CanConvert(
        const Value* value) {
    return true;
}
