
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

	}

	// check if paatern is presence in set
	std::set<std::string>::const_iterator got = standardCommandRoutes.find(pattern);

	return (got != standardCommandRoutes.end());
}

const char CommandRoutes::kStatus[] = "/status";
const char CommandRoutes::kGetLog[] = "/log";
const char CommandRoutes::kNewSession[] = "/session";
const char CommandRoutes::kSession[] = "/session/*";
const char CommandRoutes::kSessions[] = "/sessions";
const char CommandRoutes::kImplicitlyWait[] = "/session/*/timeouts/implicit_wait";
const char CommandRoutes::kSetScriptTimeout[] = "/session/*/timeouts/async_script";
const char CommandRoutes::kGetAvailableLogTypes[] = "/session/*/log/types";
const char CommandRoutes::kGetSessionLogs[] = "/session/*/log";
const char CommandRoutes::kWindowSize[] = "/session/*/window/*/size";
const char CommandRoutes::kWindowPosition[] = "/session/*/window/*/position";
const char CommandRoutes::kMaximizeWindow[] = "/session/*/window/*/maximize";

/*

const char CommandRoutes::kGet[] = "get";
const char CommandRoutes::kGoBack[] = "goBack";
const char CommandRoutes::kGoForward[] = "goForward";
const char CommandRoutes::kRefresh[] = "refresh";
const char CommandRoutes::kAddCookie[] = "addCookie";
const char CommandRoutes::kGetCookie[] = "getCookie";
const char CommandRoutes::kGetCookies[] = "getCookies";
const char CommandRoutes::kDeleteCookie[] = "deleteCookie";
const char CommandRoutes::kDeleteAllCookies[] = "deleteAllCookies";
const char CommandRoutes::kFindElement[] = "findElement";
const char CommandRoutes::kFindElements[] = "findElements";
const char CommandRoutes::kFindChildElement[] = "findChildElement";
const char CommandRoutes::kFindChildElements[] = "findChildElements";
const char CommandRoutes::kClearElement[] = "clearElement";
const char CommandRoutes::kClickElement[] = "clickElement";
const char CommandRoutes::kHoverOverElement[] = "hoverOverElement";
const char CommandRoutes::kSendKeysToElement[] = "sendKeysToElement";
const char CommandRoutes::kSendKeysToActiveElement[] = "sendKeysToActiveElement";
const char CommandRoutes::kSubmitElement[] = "submitElement";
const char CommandRoutes::kUploadFile[] = "uploadFile";
const char CommandRoutes::kGetCurrentWindowHandle[] = "getCurrentWindowHandle";
const char CommandRoutes::kGetWindowHandles[] = "getWindowHandles";
const char CommandRoutes::kSwitchToWindow[] = "switchToWindow";
const char CommandRoutes::kSwitchToFrame[] = "switchToFrame";
const char CommandRoutes::kGetActiveElement[] = "getActiveElement";
const char CommandRoutes::kGetCurrentUrl[] = "getCurrentUrl";
const char CommandRoutes::kGetPageSource[] = "getPageSource";
const char CommandRoutes::kGetTitle[] = "getTitle";
const char CommandRoutes::kExecuteScript[] = "executeScript";
const char CommandRoutes::kExecuteAsyncScript[] = "executeAsyncScript";
const char CommandRoutes::kSetBrowserVisible[] = "setBrowserVisible";
const char CommandRoutes::kIsBrowserVisible[] = "isBrowserVisible";
const char CommandRoutes::kGetElementText[] = "getElementText";
const char CommandRoutes::kGetElementValue[] = "getElementValue";
const char CommandRoutes::kGetElementTagName[] = "getElementTagName";
const char CommandRoutes::kDragElement[] = "dragElement";
const char CommandRoutes::kIsElementSelected[] = "isElementSelected";
const char CommandRoutes::kIsElementEnabled[] = "isElementEnabled";
const char CommandRoutes::kIsElementDisplayed[] = "isElementDisplayed";
const char CommandRoutes::kGetElementLocation[] = "getElementLocation";
const char CommandRoutes::kGetElementLocationOnceScrolledIntoView[] =
    "getElementLocationOnceScrolledIntoView";
const char CommandRoutes::kGetElementSize[] = "getElementSize";
const char CommandRoutes::kGetElementAttribute[] = "getElementAttribute";
const char CommandRoutes::kGetElementValueOfCssProperty[] =
    "getElementValueOfCssProperty";
const char CommandRoutes::kElementEquals[] = "elementEquals";
const char CommandRoutes::kScreenshot[] = "screenshot";
const char CommandRoutes::kGetAlert[] = "getAlert";
const char CommandRoutes::kAcceptAlert[] = "acceptAlert";
const char CommandRoutes::kDismissAlert[] = "dismissAlert";
const char CommandRoutes::kGetAlertText[] = "getAlertText";
const char CommandRoutes::kSetAlertValue[] = "setAlertValue";
const char CommandRoutes::kSetScriptTimeout[] = "setScriptTimeout";
const char CommandRoutes::kExecuteSQL[] = "executeSQL";
const char CommandRoutes::kGetLocation[] = "getLocation";
const char CommandRoutes::kSetLocation[] = "setLocation";
const char CommandRoutes::kGetAppCache[] = "getAppCache";
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
const char CommandRoutes::kMouseClick[] = "mouseClick";
const char CommandRoutes::kMouseDoubleClick[] = "mouseDoubleClick";
const char CommandRoutes::kMouseButtonDown[] = "mouseButtonDown";
const char CommandRoutes::kMouseButtonUp[] = "mouseButtonUp";
const char CommandRoutes::kMouseMoveTo[] = "mouseMoveTo";
const char CommandRoutes::kSendKeys[] = "sendKeys";
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
