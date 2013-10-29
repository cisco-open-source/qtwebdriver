// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "commands/command.h"

namespace webdriver {

Command::Command(const std::vector<std::string>& path_segments,
                 const DictionaryValue* const parameters)
    : path_segments_(path_segments),
      parameters_(parameters) {
}

Command::Command(const Command& other)
    : path_segments_(other.path_segments_),
      parameters_(other.parameters_) {
}

Command::~Command() {}

bool Command::DoesDelete() const {
    return false;
}

bool Command::DoesGet() const {
    return false;
}

bool Command::DoesPost() const {
    return false;
}

bool Command::Init(Response* const response) {
    return true;
}

void Command::Finish(Response* const response) {}

std::string Command::GetPathVariable(const size_t i) const {
    return i < path_segments_.size() ? path_segments_.at(i) : "";
}

bool Command::HasParameter(const std::string& key) const {
    return parameters_ && parameters_->HasKey(key);
}

bool Command::IsNullParameter(const std::string& key) const {
    const Value* value;
    return parameters_ &&
            parameters_->Get(key, &value) &&
            value->IsType(Value::TYPE_NULL);
}

bool Command::GetStringParameter(const std::string& key,
                                 string16* out) const {
    return parameters_ && parameters_->GetString(key, out);
}

bool Command::GetStringParameter(const std::string& key,
                                 std::string* out) const {
    return parameters_ && parameters_->GetString(key, out);
}

bool Command::GetStringASCIIParameter(const std::string& key,
                                      std::string* out) const {
    return parameters_ && parameters_->GetStringASCII(key, out);
}

bool Command::GetBooleanParameter(const std::string& key,
                                  bool* out) const {
    return parameters_ && parameters_->GetBoolean(key, out);
}

bool Command::GetIntegerParameter(const std::string& key,
                                  int* out) const {
    return parameters_ && parameters_->GetInteger(key, out);
}

bool Command::GetDoubleParameter(const std::string& key, double* out) const {
    return parameters_ && parameters_->GetDouble(key, out);
}

bool Command::GetDictionaryParameter(const std::string& key,
                                     const DictionaryValue** out) const {
    return parameters_ && parameters_->GetDictionary(key, out);
}

bool Command::GetListParameter(const std::string& key,
                               const ListValue** out) const {
    return parameters_ && parameters_->GetList(key, out);
}

}  // namespace webdriver
