// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "webdriver_route_table.h"

#include <sstream>
#include <string.h>
#include <stdlib.h>
#include <vector>

#include "webdriver_route_patterns.h"
#include "commands/non_session_commands.h"
#include "webdriver_logging.h"

namespace webdriver {


RouteTable::RouteTable(){
}

RouteTable::~RouteTable() {}

void RouteTable::Remove(const std::string& pattern) {
    std::vector<webdriver::internal::RouteDetails>::iterator route;
    for (route = routes_.begin();
         route < routes_.end();
         ++route) {
        if (pattern == route->uri_regex_) {
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
    routes_.clear();
}

std::vector<std::string> RouteTable::GetRoutes() {
    std::vector<std::string> routes_to_ret;

    std::vector<webdriver::internal::RouteDetails>::const_iterator route;
    for (route = routes_.begin();
         route < routes_.end();
         ++route) {
        routes_to_ret.push_back(route->uri_regex_);
    }

    return routes_to_ret;
}

CommandCreatorPtr RouteTable::GetRouteForURL(const std::string& url) {
    std::vector<webdriver::internal::RouteDetails>::const_iterator route;
    for (route = routes_.begin();
         route < routes_.end();
         ++route) {
        // TODO: implement MatchPattern
        //if (MatchPattern(url, route->uri_regex_))
        {
            return route->creator_;
        }
    }

    return NULL;
}


void RouteTable::AddRoute(const std::string& uri_pattern,
                          const CommandCreatorPtr& creator) {
    // TODO: add custom command check

    std::vector<webdriver::internal::RouteDetails>::iterator route;
    for (route = routes_.begin();
         route < routes_.end();
         ++route) {
        if (route->uri_regex_ == uri_pattern) {
            // replace command for pattern
            *route = webdriver::internal::RouteDetails(uri_pattern, creator);
            return;
        }

        if (CompareBestMatch(uri_pattern, route->uri_regex_)) {
            // put best match pattern before other
            routes_.insert(route, webdriver::internal::RouteDetails(uri_pattern, creator));
            return;
        }
    }

    routes_.push_back(webdriver::internal::RouteDetails(
                         uri_pattern,
                         creator));
}

bool RouteTable::CompareBestMatch(const std::string& uri_pattern1, const std::string& uri_pattern2) { 
    // TODO: implement
    return false;
}

DefaultRouteTable::DefaultRouteTable() 
    : RouteTable() {

    // Place default commands registration here
    Add<StatusCommand>(CommandRoutes::kStatus);
    Add<GlobalLogCommand>(CommandRoutes::kGetLog);
   
}

DefaultRouteTable::~DefaultRouteTable() {}

}  // namespace webdriver
