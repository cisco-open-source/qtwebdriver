// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "webdriver_route_table.h"

#include <sstream>
#include <string.h>
#include <stdlib.h>
#include <vector>



namespace webdriver {


RouteTable::RouteTable(const std::string& url_base)
    : url_base_(url_base) {
}

RouteTable::~RouteTable() {}

void RouteTable::Remove(const std::string& pattern) {
    std::vector<webdriver::internal::RouteDetails>::iterator route;
    for (route = routes_.begin();
         route < routes_.end();
         ++route) {
        if (pattern == route->uri_regex_) {
            // TODO: delete CommandCreator object
            routes_.erase(route);
            break;
        }
    }
}

bool RouteTable::HasRoute(const std::string& pattern) {
    std::vector<webdriver::internal::RouteDetails>::const_iterator route;
    for (route = routes_.begin();
         route < routes_.end();
         ++route) {
        if (pattern == route->uri_regex_) {
            return true;
        }
    }
    return false;
}

void RouteTable::Clear() {
    // TODO: implement
}

AbstractCommandCreator* RouteTable::GetRouteForURL(const std::string& url) {
    std::vector<webdriver::internal::RouteDetails>::const_iterator route;
    for (route = routes_.begin();
         route < routes_.end();
         ++route) {
        // TODO: implement MatchPattern
        if (MatchPattern(url, route->uri_regex_)) {
            return route->creator_;
        }
    }
    return NULL;
}


void RouteTable::AddRoute(const std::string& uri_pattern,
                          webdriver::AbstractCommandCreator* creator) {
    // TODO: add sorting
    // TODO: add replace
    // TODO: add custom command check
    routes_.push_back(webdriver::internal::RouteDetails(
                         uri_pattern,
                         creator));
}

}  // namespace webdriver
