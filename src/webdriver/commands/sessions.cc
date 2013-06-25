#include "commands/sessions.h"
#include <map>
#include <string>

#include "base/values.h"
#include "commands/response.h"
#include "webdriver_session_manager.h"
#include "webdriver_session.h"

namespace webdriver {

Sessions::Sessions(const std::vector<std::string>& path_segments,
                             const DictionaryValue* const parameters)
    : Command(path_segments, parameters) {}

Sessions::~Sessions() {}

bool Sessions::DoesGet() const {
    return true;
}

void Sessions::ExecuteGet(Response* const response) {
    
    ListValue *temp_value = new ListValue();

    std::map<std::string, Session*> sessions = SessionManager::GetInstance()->GetSessions();
    std::map<std::string, Session*>::iterator sessionsItr = sessions.begin();

    int listCount = 0;

    while (sessionsItr != sessions.end()) {
        DictionaryValue *session_value = new DictionaryValue();
        session_value->SetString("id", (*sessionsItr).first);

        Session* session = (*sessionsItr).second;

        DictionaryValue *capabilities_value = session->capabilities().caps->DeepCopy();

        session_value->Set("capabilities", capabilities_value);

        temp_value->Set(listCount, session_value);
        ++listCount;
        ++sessionsItr;
    }

    response->SetValue(temp_value);
}

}
