/****************************************************************************
**
** Copyright © 1992-2014 Cisco and/or its affiliates. All rights reserved.
** All rights reserved.
** 
** $CISCO_BEGIN_LICENSE:LGPL$
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** $CISCO_END_LICENSE$
**
****************************************************************************/


#include "webdriver_route_patterns.h"

namespace webdriver {

std::set<std::string> CommandRoutes::standardCommandRoutes;	

bool CommandRoutes::IsStandardRoute(const std::string& pattern) {
	// lazy set init
	if (standardCommandRoutes.empty()) {

		standardCommandRoutes.insert(kStatus);
		standardCommandRoutes.insert(kGetLog);
		standardCommandRoutes.insert(kNewSession);
		standardCommandRoutes.insert(kSession);
		standardCommandRoutes.insert(kSessions);
		standardCommandRoutes.insert(kTimeOuts);
		standardCommandRoutes.insert(kImplicitlyWait);
		standardCommandRoutes.insert(kSetScriptTimeout);
		standardCommandRoutes.insert(kGetAvailableLogTypes);
		standardCommandRoutes.insert(kGetSessionLogs);
		standardCommandRoutes.insert(kWindowSize);
		standardCommandRoutes.insert(kWindowPosition);
		standardCommandRoutes.insert(kMaximizeWindow);
		standardCommandRoutes.insert(kGetTitle);
		standardCommandRoutes.insert(kGoForward);
		standardCommandRoutes.insert(kGoBack);
		standardCommandRoutes.insert(kRefresh);
		standardCommandRoutes.insert(kScreenshot);
		standardCommandRoutes.insert(kElementScreenshot);
		standardCommandRoutes.insert(kSendKeys);
		standardCommandRoutes.insert(kClickElement);
		standardCommandRoutes.insert(kHoverOverElement);
		standardCommandRoutes.insert(kMouseClick);
		standardCommandRoutes.insert(kMouseDoubleClick);
		standardCommandRoutes.insert(kMouseButtonDown);
		standardCommandRoutes.insert(kMouseButtonUp);
		standardCommandRoutes.insert(kMouseMoveTo);
		standardCommandRoutes.insert(kMouseWheel);
		standardCommandRoutes.insert(kFindElement);
		standardCommandRoutes.insert(kFindElements);
		standardCommandRoutes.insert(kClearElement);
		standardCommandRoutes.insert(kSendKeysToElement);
		standardCommandRoutes.insert(kSubmitElement);
		standardCommandRoutes.insert(kGetElementText);
		standardCommandRoutes.insert(kGetElementTagName);
		standardCommandRoutes.insert(kIsElementSelected);
		standardCommandRoutes.insert(kIsElementEnabled);
		standardCommandRoutes.insert(kIsElementDisplayed);
		standardCommandRoutes.insert(kGetElementLocation);
		standardCommandRoutes.insert(kGetElementLocationInView);
		standardCommandRoutes.insert(kGetElementSize);
		standardCommandRoutes.insert(kGetElementAttribute);
		standardCommandRoutes.insert(kGetElementCssProp);
		standardCommandRoutes.insert(kElementEquals);
		standardCommandRoutes.insert(kFindChildElement);
		standardCommandRoutes.insert(kFindChildElements);
		standardCommandRoutes.insert(kSwitchToFrame);
		standardCommandRoutes.insert(kGetActiveElement);
		standardCommandRoutes.insert(kGetCurrentWindowHandle);
		standardCommandRoutes.insert(kGetWindowHandles);
		standardCommandRoutes.insert(kWindowCmd);
		standardCommandRoutes.insert(kGetPageSource);
		standardCommandRoutes.insert(kUrlCmd);
		standardCommandRoutes.insert(kExecuteScript);
		standardCommandRoutes.insert(kExecuteAsyncScript);
		standardCommandRoutes.insert(kGetAppCache);
		standardCommandRoutes.insert(kBrowserConnection);
		standardCommandRoutes.insert(kAlertText);
		standardCommandRoutes.insert(kAcceptAlert);
		standardCommandRoutes.insert(kDismissAlert);
		standardCommandRoutes.insert(kCookie);
		standardCommandRoutes.insert(kNamedCookie);
		standardCommandRoutes.insert(kLocalStorage);
		standardCommandRoutes.insert(kLocalStorageKey);
		standardCommandRoutes.insert(kLocalStorageSize);
		standardCommandRoutes.insert(kSessionStorage);
		standardCommandRoutes.insert(kSessionStorageKey);
		standardCommandRoutes.insert(kSessionStorageSize);
		standardCommandRoutes.insert(kGetLocation);
		standardCommandRoutes.insert(kTouchClick);
		standardCommandRoutes.insert(kTouchDown);
		standardCommandRoutes.insert(kTouchUp);
		standardCommandRoutes.insert(kTouchMove);
		standardCommandRoutes.insert(kTouchScroll);
		standardCommandRoutes.insert(kTouchDoubleClick);
		standardCommandRoutes.insert(kTouchLongClick);
		standardCommandRoutes.insert(kTouchFlick);
		standardCommandRoutes.insert(kOrientation);
		standardCommandRoutes.insert(kXdrpc);
		standardCommandRoutes.insert(kCiscoPlayerState);
		standardCommandRoutes.insert(kCiscoPlayerVolume);
		standardCommandRoutes.insert(kCiscoPlayingPosition);
		standardCommandRoutes.insert(kShutdown);
	}

	// check if paatern is presence in set
	std::set<std::string>::const_iterator got = standardCommandRoutes.find(pattern);

	return (got != standardCommandRoutes.end());
}

const char CommandRoutes::kStatus[] 					= "/status";
const char CommandRoutes::kGetLog[] 					= "/log";
const char CommandRoutes::kNewSession[] 				= "/session";
const char CommandRoutes::kSession[] 					= "/session/*";
const char CommandRoutes::kSessions[] 					= "/sessions";
const char CommandRoutes::kTimeOuts[]					= "/session/*/timeouts";
const char CommandRoutes::kImplicitlyWait[] 			= "/session/*/timeouts/implicit_wait";
const char CommandRoutes::kSetScriptTimeout[] 			= "/session/*/timeouts/async_script";
const char CommandRoutes::kGetAvailableLogTypes[] 		= "/session/*/log/types";
const char CommandRoutes::kGetSessionLogs[] 			= "/session/*/log";
const char CommandRoutes::kWindowSize[] 				= "/session/*/window/*/size";
const char CommandRoutes::kWindowPosition[] 			= "/session/*/window/*/position";
const char CommandRoutes::kMaximizeWindow[] 			= "/session/*/window/*/maximize";
const char CommandRoutes::kGetTitle[] 					= "/session/*/title";
const char CommandRoutes::kGoForward[] 					= "/session/*/forward";
const char CommandRoutes::kGoBack[] 					= "/session/*/back";
const char CommandRoutes::kRefresh[] 					= "/session/*/refresh";
const char CommandRoutes::kScreenshot[] 				= "/session/*/screenshot";
const char CommandRoutes::kElementScreenshot[] 			= "/session/*/element/*/screenshot";
const char CommandRoutes::kSendKeys[] 					= "/session/*/keys";
const char CommandRoutes::kClickElement[] 				= "/session/*/element/*/click";
const char CommandRoutes::kHoverOverElement[] 			= "/session/*/element/*/hover";
const char CommandRoutes::kMouseClick[] 				= "/session/*/click";
const char CommandRoutes::kMouseDoubleClick[] 			= "/session/*/doubleclick";
const char CommandRoutes::kMouseButtonDown[]			= "/session/*/buttondown";
const char CommandRoutes::kMouseButtonUp[] 				= "/session/*/buttonup";
const char CommandRoutes::kMouseMoveTo[] 				= "/session/*/moveto";
const char CommandRoutes::kMouseWheel[] 				= "/session/*/wheel";
const char CommandRoutes::kFindElement[] 				= "/session/*/element";
const char CommandRoutes::kFindElements[] 				= "/session/*/elements";
const char CommandRoutes::kClearElement[] 				= "/session/*/element/*/clear";
const char CommandRoutes::kSendKeysToElement[] 			= "/session/*/element/*/value";
const char CommandRoutes::kSubmitElement[] 				= "/session/*/element/*/submit";
const char CommandRoutes::kGetElementText[] 			= "/session/*/element/*/text";
const char CommandRoutes::kGetElementTagName[] 			= "/session/*/element/*/name";
const char CommandRoutes::kIsElementSelected[] 			= "/session/*/element/*/selected";
const char CommandRoutes::kIsElementEnabled[] 			= "/session/*/element/*/enabled";
const char CommandRoutes::kIsElementDisplayed[]			= "/session/*/element/*/displayed";
const char CommandRoutes::kGetElementLocation[] 		= "/session/*/element/*/location";
const char CommandRoutes::kGetElementLocationInView[] 	= "/session/*/element/*/location_in_view";
const char CommandRoutes::kGetElementSize[] 			= "/session/*/element/*/size";
const char CommandRoutes::kGetElementAttribute[] 		= "/session/*/element/*/attribute/*";
const char CommandRoutes::kGetElementCssProp[] 			= "/session/*/element/*/css/*";
const char CommandRoutes::kElementEquals[] 				= "/session/*/element/*/equals/*";
const char CommandRoutes::kFindChildElement[]			= "/session/*/element/*/element";
const char CommandRoutes::kFindChildElements[]			= "/session/*/element/*/elements";
const char CommandRoutes::kSwitchToFrame[] 				= "/session/*/frame";
const char CommandRoutes::kGetActiveElement[] 			= "/session/*/element/active";
const char CommandRoutes::kGetCurrentWindowHandle[]		= "/session/*/window_handle";
const char CommandRoutes::kGetWindowHandles[] 			= "/session/*/window_handles";
const char CommandRoutes::kWindowCmd[] 					= "/session/*/window";
const char CommandRoutes::kGetPageSource[] 				= "/session/*/source";
const char CommandRoutes::kUrlCmd[] 					= "/session/*/url";
const char CommandRoutes::kExecuteScript[] 				= "/session/*/execute";
const char CommandRoutes::kExecuteAsyncScript[] 		= "/session/*/execute_async";
const char CommandRoutes::kGetAppCache[] 				= "/session/*/application_cache/status";
const char CommandRoutes::kBrowserConnection[]			= "/session/*/browser_connection";
const char CommandRoutes::kAlertText[] 					= "/session/*/alert_text";
const char CommandRoutes::kAcceptAlert[] 				= "/session/*/accept_alert";
const char CommandRoutes::kDismissAlert[] 				= "/session/*/dismiss_alert";
const char CommandRoutes::kCookie[] 					= "/session/*/cookie";
const char CommandRoutes::kNamedCookie[] 				= "/session/*/cookie/*";
const char CommandRoutes::kLocalStorage[] 				= "/session/*/local_storage";
const char CommandRoutes::kLocalStorageKey[] 			= "/session/*/local_storage/key/*";
const char CommandRoutes::kLocalStorageSize[] 			= "/session/*/local_storage/size";
const char CommandRoutes::kSessionStorage[] 			= "/session/*/session_storage";
const char CommandRoutes::kSessionStorageKey[] 			= "/session/*/session_storage/key/*";
const char CommandRoutes::kSessionStorageSize[] 		= "/session/*/session_storage/size";
const char CommandRoutes::kGetLocation[]				= "/session/*/location";
const char CommandRoutes::kTouchClick[] 				= "/session/*/touch/click";
const char CommandRoutes::kTouchDown[]  				= "/session/*/touch/down";
const char CommandRoutes::kTouchUp[]    				= "/session/*/touch/up";
const char CommandRoutes::kTouchMove[]  				= "/session/*/touch/move";
const char CommandRoutes::kTouchScroll[]  				= "/session/*/touch/scroll";
const char CommandRoutes::kTouchDoubleClick[]			= "/session/*/touch/doubleclick";
const char CommandRoutes::kTouchLongClick[]  			= "/session/*/touch/longclick";
const char CommandRoutes::kTouchFlick[]					= "/session/*/touch/flick";
const char CommandRoutes::kOrientation[]				= "/session/*/orientation";
const char CommandRoutes::kXdrpc[]						= "/xdrpc";
const char CommandRoutes::kCiscoPlayerState[]           = "/session/*/element/*/-cisco-player-element/state";
const char CommandRoutes::kCiscoPlayerVolume[]          = "/session/*/element/*/-cisco-player-element/volume";
const char CommandRoutes::kCiscoPlayingPosition[]       = "/session/*/element/*/-cisco-player-element/seek";
const char CommandRoutes::kCiscoPlayerMute[]            = "/session/*/element/*/-cisco-player-element/mute";
const char CommandRoutes::kCiscoPlayerPlayingSpeed[]    = "/session/*/element/*/-cisco-player-element/speed";
const char CommandRoutes::kVisualizerSource[]           = "/session/*/-cisco-visualizer-source";
const char CommandRoutes::kVisualizerShowPoint[]        = "/session/*/-cisco-visualizer-show-point";
const char CommandRoutes::kTouchPinchZoom[]             = "/session/*/touch/-cisco-pinch-zoom";
const char CommandRoutes::kTouchPinchRotate[]           = "/session/*/touch/-cisco-pinch-rotate";
const char CommandRoutes::kShutdown[]                   = "/shutdown";

} // namespace webdriver
