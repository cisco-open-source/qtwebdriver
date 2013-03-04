#include "sessions.h"
#include <map>
#include <string>

#include "base/values.h"
#include "chrome/app/chrome_command_ids.h"
#include "chrome/common/chrome_constants.h"
#include "chrome/test/webdriver/commands/response.h"
#include "chrome/test/webdriver/webdriver_session_manager.h"
#include "chrome/test/webdriver/webdriver_session.h"

namespace webdriver {

Sessions::Sessions(const std::vector<std::string>& path_segments,
                             const DictionaryValue* const parameters)
    : Command(path_segments, parameters) {}

Sessions::~Sessions() {}

bool Sessions::DoesGet() {
  return true;
}

void Sessions::ExecuteGet(Response* const response) {
  ListValue *temp_value = new ListValue();

  std::map<std::string, Session*> sessions = SessionManager::GetInstance()->GetSessions();


  std::map<std::string, Session*>::iterator sessionsItr = sessions.begin();

  int listCount = 0;

  while (sessionsItr != sessions.end())
  {
      DictionaryValue *session_value = new DictionaryValue();
      session_value->SetString("id", (*sessionsItr).first);

      Session* session = (*sessionsItr).second;

      DictionaryValue *capabilities_value = new DictionaryValue();

      // Standard capabilities defined at
      // http://code.google.com/p/selenium/wiki/JsonWireProtocol#Capabilities_JSON_Object
      capabilities_value->SetString("browserName", "chrome");
      capabilities_value->SetString("version", session->GetBrowserVersion());

    #if defined(OS_WIN)
      capabilities_value->SetString("platform", "windows");
    #elif defined(OS_MACOSX)
      capabilities_value->SetString("platform", "mac");
    #elif defined(OS_CHROMEOS)
      capabilities_value->SetString("platform", "chromeos");
    #elif defined(OS_LINUX)
      capabilities_value->SetString("platform", "linux");
    #else
      capabilities_value->SetString("platform", "unknown");
    #endif

      capabilities_value->SetBoolean("javascriptEnabled", true);
      capabilities_value->SetBoolean("takesScreenshot", true);
      capabilities_value->SetBoolean("handlesAlerts", true);
      capabilities_value->SetBoolean("databaseEnabled", true);
      capabilities_value->SetBoolean("locationContextEnabled", true);
      capabilities_value->SetBoolean("applicationCacheEnabled", true);
      capabilities_value->SetBoolean("browserConnectionEnabled", false);
      capabilities_value->SetBoolean("cssSelectorsEnabled", true);
      capabilities_value->SetBoolean("webStorageEnabled", true);
      capabilities_value->SetBoolean("rotatable", false);
      capabilities_value->SetBoolean("acceptSslCerts", false);
      // Even when ChromeDriver does not OS-events, the input simulation produces
      // the same effect for most purposes (except IME).
      capabilities_value->SetBoolean("nativeEvents", true);

      // Custom non-standard session info.
      capabilities_value->SetWithoutPathExpansion(
          "chrome.chromedriverVersion",
          Value::CreateStringValue("QtWebKit"/*chrome::kChromeVersion*/));
      capabilities_value->SetWithoutPathExpansion(
          "chrome.nativeEvents",
          Value::CreateBooleanValue(session->capabilities().native_events));

      session_value->Set("capabilities", capabilities_value);

      temp_value->Set(listCount, session_value);
      ++listCount;
      ++sessionsItr;
  }

  response->SetValue(temp_value);
}

}
