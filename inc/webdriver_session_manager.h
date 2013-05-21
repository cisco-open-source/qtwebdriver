// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WEBDRIVER_WEBDRIVER_SESSION_MANAGER_H_
#define WEBDRIVER_WEBDRIVER_SESSION_MANAGER_H_

#include <map>
#include <string>

#include "base/memory/singleton.h"
#include "base/synchronization/lock.h"

namespace webdriver {

class Session;

/// Session manager keeps track of all of the session that are currently
/// running on the machine under test. With webdriver the user is allowed
/// multiple instances of the browser on the same machine.  So 2 sessions
/// open would mean you would have 2 instances of chrome running under
/// their own profiles.
class SessionManager {
public:
    /// Returns the singleton instance.
    static SessionManager* GetInstance();

    /// Add previously created session to SessionManager
    /// @param session pointer to Session object
    void Add(Session* session);

    /// Remove session object referenced by sessionId
    /// @param id sessionId to remove
    /// @return true - if succeed, false - if sessionId not found
    bool Remove(const std::string& id);

    /// Check if session exists
    /// @param id sessionId to check
    /// @return true - exists, false - not found
    bool Has(const std::string& id) const;

    /// Get Session object by sessionId
    /// @param id sessionId to retrieve
    /// @return pointer to Session object, NULL if not found
    Session* GetSession(const std::string& id) const;

    /// Get list of sessions
    /// @return vector of pairs (sessionId, pointer to Session)
    std::map<std::string, Session*> GetSessions();

private:
    SessionManager();
    ~SessionManager();
    friend struct DefaultSingletonTraits<SessionManager>;

    std::map<std::string, Session*> map_;
    mutable base::Lock map_lock_;

    DISALLOW_COPY_AND_ASSIGN(SessionManager);
};

}  // namespace webdriver

#endif  // WEBDRIVER_WEBDRIVER_SESSION_MANAGER_H_
