
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
		standardCommandRoutes.insert(kClickElement);
		standardCommandRoutes.insert(kHoverOverElement);
		standardCommandRoutes.insert(kMouseClick);
		standardCommandRoutes.insert(kMouseDoubleClick);
		standardCommandRoutes.insert(kMouseButtonDown);
		standardCommandRoutes.insert(kMouseButtonUp);
		standardCommandRoutes.insert(kMouseMoveTo);
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
		standardCommandRoutes.insert(kFindElement);
		standardCommandRoutes.insert(kFindElements);
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
		standardCommandRoutes.insert(kAlertText);
		standardCommandRoutes.insert(kAcceptAlert);
		standardCommandRoutes.insert(kDismissAlert);

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
const char CommandRoutes::kSendKeys[] 					= "/session/*/keys";
const char CommandRoutes::kClickElement[] 				= "/session/*/element/*/click";
const char CommandRoutes::kHoverOverElement[] 			= "/session/*/element/*/hover";
const char CommandRoutes::kMouseClick[] 				= "/session/*/click";
const char CommandRoutes::kMouseDoubleClick[] 			= "/session/*/doubleclick";
const char CommandRoutes::kMouseButtonDown[]			= "/session/*/buttondown";
const char CommandRoutes::kMouseButtonUp[] 				= "/session/*/buttonup";
const char CommandRoutes::kMouseMoveTo[] 				= "/session/*/moveto";
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
const char CommandRoutes::kFindChildElement[] 				= "/session/*/element/*/element";
const char CommandRoutes::kFindChildElements[] 				= "/session/*/elements/*/elements";
const char CommandRoutes::kSwitchToFrame[] 				= "/session/*/frame";
const char CommandRoutes::kGetActiveElement[] 			= "/session/*/element/active";
const char CommandRoutes::kGetCurrentWindowHandle[]		= "/session/*/window_handle";
const char CommandRoutes::kGetWindowHandles[] 			= "/session/*/window_handle";
const char CommandRoutes::kWindowCmd[] 					= "/session/*/window";
const char CommandRoutes::kGetPageSource[] 				= "/session/*/source";
const char CommandRoutes::kUrlCmd[] 					= "/session/*/url";
const char CommandRoutes::kExecuteScript[] 				= "/session/*/execute";
const char CommandRoutes::kExecuteAsyncScript[] 		= "/session/*/execute_async";
const char CommandRoutes::kGetAppCache[] 				= "/session/*/application_cache/status";
const char CommandRoutes::kAlertText[] 					= "/session/*/alert_text";
const char CommandRoutes::kAcceptAlert[] 				= "/session/*/accept_alert";
const char CommandRoutes::kDismissAlert[] 				= "/session/*/dismiss_alert";

/*

const char CommandRoutes::kGet[] = "get";
const char CommandRoutes::kAddCookie[] = "addCookie";
const char CommandRoutes::kGetCookie[] = "getCookie";
const char CommandRoutes::kGetCookies[] = "getCookies";
const char CommandRoutes::kDeleteCookie[] = "deleteCookie";
const char CommandRoutes::kDeleteAllCookies[] = "deleteAllCookies";

const char CommandRoutes::kSendKeysToActiveElement[] = "sendKeysToActiveElement";

const char CommandRoutes::kSetBrowserVisible[] = "setBrowserVisible";
const char CommandRoutes::kIsBrowserVisible[] = "isBrowserVisible";
const char CommandRoutes::kSetScriptTimeout[] = "setScriptTimeout";
const char CommandRoutes::kExecuteSQL[] = "executeSQL";
const char CommandRoutes::kGetLocation[] = "getLocation";
const char CommandRoutes::kSetLocation[] = "setLocation";

const char CommandRoutes::kGetStatus[] = "getStatus";
const char CommandRoutes::kClearAppCache[] = "clearAppCache";
const char CommandRoutes::kIsBrowserOnline[] = "isBrowserOnline";
const char CommandRoutes::kSetBrowserOnline[] = "setBrowserOnline";
const char CommandRoutes::kGetLocalStorageItem[] = "getLocalStorageItem";
const char CommandRoutes::kGetLocalStorageKeys[] = "getLocalStorageKeys";
const char CommandRoutes::kSetLocalStorageItem[] = "setLocalStorageItem";
const char CommandRoutes::kRemoveLocalStorageItem[] = "removeLocalStorageItem";
const char CommandRoutes::kClearLocalStorage[] = "clearLocalStorage";
const char CommandRoutes::kGetLocalStorageSize[] = "getLocalStorageSize";
const char CommandRoutes::kGetSessionStorageItem[] = "getSessionStorageItem";
const char CommandRoutes::kGetSessionStorageKey[] = "getSessionStorageKey";
const char CommandRoutes::kSetSessionStorageItem[] = "setSessionStorageItem";
const char CommandRoutes::kRemoveSessionStorageItem[] =
    "removeSessionStorageItem";
const char CommandRoutes::kClearSessionStorage[] = "clearSessionStorage";
const char CommandRoutes::kGetSessionStorageSize[] = "getSessionStorageSize";
const char CommandRoutes::kSetScreenOrientation[] = "setScreenOrientation";
const char CommandRoutes::kGetScreenOrientation[] = "getScreenOrientation";
const char CommandRoutes::kImeGetAvailableEngines[] = "imeGetAvailableEngines";
const char CommandRoutes::kImeGetActiveEngine[] = "imeGetActiveEngine";
const char CommandRoutes::kImeIsActivated[] = "imeIsActivated";
const char CommandRoutes::kImeDeactivate[] = "imeDeactivate";
const char CommandRoutes::kImeActivateEngine[] = "imeActivateEngine";
const char CommandRoutes::kTouchSingleTap[] = "touchSingleTap";
const char CommandRoutes::kTouchDown[] = "touchDown";
const char CommandRoutes::kTouchUp[] = "touchUp";
const char CommandRoutes::kTouchMove[] = "touchMove";
const char CommandRoutes::kTouchScroll[] = "touchScroll";
const char CommandRoutes::kTouchDoubleTap[] = "touchDoubleTap";
const char CommandRoutes::kTouchLongPress[] = "touchLongPress";
const char CommandRoutes::kTouchFlick[] = "touchFlick";
*/

} // namespace webdriver
