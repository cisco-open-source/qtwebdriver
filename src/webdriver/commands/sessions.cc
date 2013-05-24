#include "commands/sessions.h"
#include <map>
#include <string>

#include "base/values.h"
#include "base/sys_info.h"
#include "commands/response.h"
#include "webdriver_session_manager.h"
#include "webdriver_session.h"

namespace webdriver {

Sessions::Sessions(const std::vector<std::string>& path_segments,
                             const DictionaryValue* const parameters)
    : Command(path_segments, parameters) {}

Sessions::~Sessions() {}

bool Sessions::DoesGet() {
    return true;
}

void Sessions::ExecuteGet(Response* const response) {
    // TODO: finish implementation
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
      //capabilities_value->SetString("browserName", "QtWebkit");
      //capabilities_value->SetString("version", session->GetBrowserVersion());

      capabilities_value->SetString("platform", base::SysInfo::OperatingSystemName());

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

      if (session->capabilities().proxy != NULL)
          capabilities_value->Set("proxy", session->capabilities().proxy->DeepCopy());

      session_value->Set("capabilities", capabilities_value);



      temp_value->Set(listCount, session_value);
      ++listCount;
      ++sessionsItr;
  }

  response->SetValue(temp_value);
}

}
