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
