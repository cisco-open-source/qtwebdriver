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
#include "commands/create_session.h"
#include "commands/session_with_id.h"
#include "commands/sessions.h"
#include "commands/set_timeout_commands.h"
#include "commands/log_command.h"
#include "commands/window_commands.h"
#include "commands/title_command.h"
#include "commands/navigate_commands.h"
#include "commands/screenshot_command.h"
#include "commands/keys_command.h"
#include "commands/mouse_commands.h"
#include "commands/element_commands.h"
#include "commands/find_element_commands.h"
#include "commands/target_locator_commands.h"
#include "commands/source_command.h"
#include "commands/url_command.h"
#include "commands/execute_command.h"
#include "commands/execute_async_script_command.h"
#include "commands/appcache_status_command.h"
#include "commands/alert_commands.h"
#include "commands/cookie_commands.h"
#include "commands/html5_storage_commands.h"
#include "commands/html5_location_commands.h"
#include "commands/cisco_player_commands.h"
#include "commands/xdrpc_command.h"
#include "commands/touch_commands.h"
#include "commands/orientation_command.h"
#include "commands/visualizer_commands.h"
#include "commands/browser_connection_command.h"
#include "webdriver_logging.h"
#include "base/string_split.h"

namespace webdriver {


RouteTable::RouteTable(){
}

RouteTable::RouteTable(const RouteTable &obj)
    : routes_(obj.routes_) {
}
 
RouteTable& RouteTable::operator= (const RouteTable &obj) {
    routes_ = obj.routes_;
    return *this;
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

CommandCreatorPtr RouteTable::GetRouteForURL(const std::string& url, std::string* pattern) const {
    std::vector<webdriver::internal::RouteDetails>::const_iterator route;
    for (route = routes_.begin();
         route < routes_.end();
         ++route) {
        if (MatchPattern(url, route->uri_regex_)) {
            if (NULL != pattern) {
                *pattern = route->uri_regex_;
            }
            return route->creator_;
        }
    }

    return NULL;
}

bool RouteTable::AddRoute(const std::string& uri_pattern,
                          const CommandCreatorPtr& creator) {
    // custom command check
    if (!CommandRoutes::IsStandardRoute(uri_pattern)) {
        std::vector<std::string> url_segments;
        base::SplitString(uri_pattern, '/', &url_segments);

        if (url_segments.size() < 2
                || !url_segments[0].empty()) {
            GlobalLogger::Log(kWarningLogLevel, "Custom commands too short");
            return false;
        }
        bool hasCustomPrefix = false;
        for (size_t i = 0; i < url_segments.size(); ++i)
        {
            hasCustomPrefix = CheckCustomPrefix(url_segments[i]);
            if (hasCustomPrefix)
                break;
        }
        if (!hasCustomPrefix) {
            GlobalLogger::Log(kWarningLogLevel, "Comand " + uri_pattern +" has no valid custom prefix");
            return false;
        }
    }
    std::vector<webdriver::internal::RouteDetails>::iterator route;
    for (route = routes_.begin();
         route < routes_.end();
         ++route) {
        if (route->uri_regex_ == uri_pattern) {
            // replace command for pattern
            *route = webdriver::internal::RouteDetails(uri_pattern, creator);
            return true;
        }

        if (CompareBestMatch(uri_pattern, route->uri_regex_)) {
            // put best match pattern before other
            routes_.insert(route, webdriver::internal::RouteDetails(uri_pattern, creator));
            return true;
        }
    }

    routes_.push_back(webdriver::internal::RouteDetails(
                         uri_pattern,
                         creator));
    return true;
}

bool RouteTable::CheckCustomPrefix(const std::string& prefix) {
    std::vector<std::string> prefix_segments;
    base::SplitString(prefix, '-', &prefix_segments);
#if defined(OS_WIN)
    if (prefix_segments.size() < 3 || !prefix_segments[0].empty() || prefix_segments[2].empty()
            || prefix_segments[1].empty() || !isalpha(prefix_segments[1].at(0))) {
#else //OS_WIN
    if (prefix_segments.size() < 3 || !prefix_segments[0].empty() || prefix_segments[2].empty()
            || prefix_segments[1].empty() || !std::isalpha(prefix_segments[1].at(0))) {
#endif //OS_WIN
        return false;
    }
    return true;
}

bool RouteTable::MatchPattern(const std::string& url, const std::string& pattern) const {
    std::vector<std::string> url_segments;
    std::vector<std::string> pattern_segments;

    base::SplitString(url, '/', &url_segments);
    base::SplitString(pattern, '/', &pattern_segments);

    unsigned int segments_num = url_segments.size();

    if (segments_num != pattern_segments.size()) {
        // different segments num
        return false;
    }

    for (unsigned int i = 0; i < segments_num; i++) {
        if (pattern_segments.at(i) == "*")
            continue;
        if (pattern_segments.at(i) != url_segments.at(i))
            return false;
    }

    return true;
}

bool RouteTable::CompareBestMatch(const std::string& uri_pattern1, const std::string& uri_pattern2) {

    std::vector<std::string> segments1;
    std::vector<std::string> segments2;
    base::SplitString(uri_pattern1, '/', &segments1);
    base::SplitString(uri_pattern2, '/', &segments2);

    unsigned int segments_num = segments1.size();

    if (segments_num != segments2.size()) {
        // different segments num
        return false;
    }

    for (unsigned int i = 0; i < segments_num; i++) {
        if (segments1.at(i) == segments2.at(i))
            continue;
        if (segments1.at(i) == "*")
            return false;
        if (segments2.at(i) == "*")
            return true;
        return false;
    }
    return false;
}

DefaultRouteTable::DefaultRouteTable() 
    : RouteTable() {

    // Place default commands registration here
    Add<StatusCommand>                  (CommandRoutes::kStatus);
    Add<GlobalLogCommand>               (CommandRoutes::kGetLog);
    Add<CreateSession>                  (CommandRoutes::kNewSession);
    Add<SessionWithID>                  (CommandRoutes::kSession);
    Add<Sessions>                       (CommandRoutes::kSessions);
    Add<SetTimeoutCommand>              (CommandRoutes::kTimeOuts);
    Add<SetAsyncScriptTimeoutCommand>   (CommandRoutes::kSetScriptTimeout);
    Add<ImplicitWaitCommand>            (CommandRoutes::kImplicitlyWait);
    Add<LogTypesCommand>                (CommandRoutes::kGetAvailableLogTypes);
    Add<LogCommand>                     (CommandRoutes::kGetSessionLogs);
    Add<WindowSizeCommand>              (CommandRoutes::kWindowSize);
    Add<WindowPositionCommand>          (CommandRoutes::kWindowPosition);
    Add<WindowMaximizeCommand>          (CommandRoutes::kMaximizeWindow);
    Add<TitleCommand>                   (CommandRoutes::kGetTitle);
    Add<ForwardCommand>                 (CommandRoutes::kGoForward);
    Add<BackCommand>                    (CommandRoutes::kGoBack);
    Add<RefreshCommand>                 (CommandRoutes::kRefresh);
    Add<ScreenshotCommand>              (CommandRoutes::kScreenshot);
    Add<ElementScreenshotCommand>       (CommandRoutes::kElementScreenshot);
    Add<KeysCommand>                    (CommandRoutes::kSendKeys);
    Add<MoveAndClickCommand>            (CommandRoutes::kClickElement);
    Add<HoverCommand>                   (CommandRoutes::kHoverOverElement);
    Add<ClickCommand>                   (CommandRoutes::kMouseClick);
    Add<DoubleClickCommand>             (CommandRoutes::kMouseDoubleClick);
    Add<ButtonDownCommand>              (CommandRoutes::kMouseButtonDown);
    Add<ButtonUpCommand>                (CommandRoutes::kMouseButtonUp);
    Add<MoveToCommand>                  (CommandRoutes::kMouseMoveTo);
    Add<WheelCommand>                   (CommandRoutes::kMouseWheel);
    Add<ElementClearCommand>            (CommandRoutes::kClearElement);
    Add<ElementValueCommand>            (CommandRoutes::kSendKeysToElement);
    Add<ElementSubmitCommand>           (CommandRoutes::kSubmitElement);
    Add<ElementTextCommand>             (CommandRoutes::kGetElementText);
    Add<ElementNameCommand>             (CommandRoutes::kGetElementTagName);
    Add<ElementSelectedCommand>         (CommandRoutes::kIsElementSelected);
    Add<ElementEnabledCommand>          (CommandRoutes::kIsElementEnabled);
    Add<ElementDisplayedCommand>        (CommandRoutes::kIsElementDisplayed);
    Add<ElementLocationCommand>         (CommandRoutes::kGetElementLocation);
    Add<ElementLocationInViewCommand>   (CommandRoutes::kGetElementLocationInView);
    Add<ElementSizeCommand>             (CommandRoutes::kGetElementSize);
    Add<ElementAttributeCommand>        (CommandRoutes::kGetElementAttribute);
    Add<ElementCssCommand>              (CommandRoutes::kGetElementCssProp);
    Add<ElementEqualsCommand>           (CommandRoutes::kElementEquals);
    Add<FindOneElementCommand>          (CommandRoutes::kFindElement);
    Add<FindManyElementsCommand>        (CommandRoutes::kFindElements);
    Add<FindOneElementCommand>          (CommandRoutes::kFindChildElement);
    Add<FindManyElementsCommand>        (CommandRoutes::kFindChildElements);
    Add<SwitchFrameCommand>             (CommandRoutes::kSwitchToFrame);
    Add<ActiveElementCommand>           (CommandRoutes::kGetActiveElement);
    Add<WindowHandleCommand>            (CommandRoutes::kGetCurrentWindowHandle);
    Add<WindowHandlesCommand>           (CommandRoutes::kGetWindowHandles);
    Add<WindowCommand>                  (CommandRoutes::kWindowCmd);
    Add<SourceCommand>                  (CommandRoutes::kGetPageSource);
    Add<URLCommand>                     (CommandRoutes::kUrlCmd);
    Add<ExecuteCommand>                 (CommandRoutes::kExecuteScript);
    Add<ExecuteAsyncScriptCommand>      (CommandRoutes::kExecuteAsyncScript);
    Add<AppCacheStatusCommand>          (CommandRoutes::kGetAppCache);
    Add<BrowserConnectionCommand>       (CommandRoutes::kBrowserConnection);
    Add<AlertTextCommand>               (CommandRoutes::kAlertText);
    Add<AcceptAlertCommand>             (CommandRoutes::kAcceptAlert);
    Add<DismissAlertCommand>            (CommandRoutes::kDismissAlert);
    Add<CookieCommand>                  (CommandRoutes::kCookie);
    Add<NamedCookieCommand>             (CommandRoutes::kNamedCookie);
    Add<LocalStorageCommand>            (CommandRoutes::kLocalStorage);
    Add<LocalStorageKeyCommand>         (CommandRoutes::kLocalStorageKey);
    Add<LocalStorageSizeCommand>        (CommandRoutes::kLocalStorageSize);
    Add<SessionStorageCommand>          (CommandRoutes::kSessionStorage);
    Add<SessionStorageKeyCommand>       (CommandRoutes::kSessionStorageKey);
    Add<SessionStorageSizeCommand>      (CommandRoutes::kSessionStorageSize);
    Add<HTML5LocationCommand>           (CommandRoutes::kGetLocation);
    Add<TouchClickCommand>              (CommandRoutes::kTouchClick);
    Add<TouchDownCommand>               (CommandRoutes::kTouchDown);
    Add<TouchUpCommand>                 (CommandRoutes::kTouchUp);
    Add<TouchMoveCommand>               (CommandRoutes::kTouchMove);
    Add<TouchScrollCommand>             (CommandRoutes::kTouchScroll);
    Add<TouchDoubleClickCommand>        (CommandRoutes::kTouchDoubleClick);
    Add<TouchLongClickCommand>          (CommandRoutes::kTouchLongClick);
    Add<TouchFlickCommand>              (CommandRoutes::kTouchFlick);
    Add<OrientationCommand>             (CommandRoutes::kOrientation);
    Add<XDRPCCommand>                   (CommandRoutes::kXdrpc);
    Add<CISCO_StateCommand>             (CommandRoutes::kCiscoPlayerState);
    Add<CISCO_VolumeCommand>            (CommandRoutes::kCiscoPlayerVolume);
    Add<CISCO_SeekCommand>              (CommandRoutes::kCiscoPlayingPosition);
    Add<CISCO_MuteCommand>              (CommandRoutes::kCiscoPlayerMute);
    Add<CISCO_PlaybackSpeedCommand>     (CommandRoutes::kCiscoPlayerPlayingSpeed);
    Add<VisualizerSourceCommand>        (CommandRoutes::kVisualizerSource);
    Add<VisualizerShowPointCommand>     (CommandRoutes::kVisualizerShowPoint);
    Add<TouchPinchZoomCommand>          (CommandRoutes::kTouchPinchZoom);
    Add<TouchPinchRotateCommand>        (CommandRoutes::kTouchPinchRotate);

}

DefaultRouteTable::~DefaultRouteTable() {}

}  // namespace webdriver
