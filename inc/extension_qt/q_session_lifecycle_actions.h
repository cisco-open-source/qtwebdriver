#ifndef WEBDRIVER_QT_SESSION_ACTIONS_H_
#define WEBDRIVER_QT_SESSION_ACTIONS_H_

#include <string>
#include <vector>

#include "webdriver_session.h"

#include <QtNetwork/QNetworkProxy>

namespace webdriver {

class QSessionLifeCycleActions : public SessionLifeCycleActions {
public:
    QSessionLifeCycleActions(Session* session);
    virtual ~QSessionLifeCycleActions() {};

    virtual Error* PostInit(const base::DictionaryValue* desired_capabilities_dict,
                const base::DictionaryValue* required_capabilities_dict);

    virtual void BeforeTerminate(void);

protected:
private:
    bool restore_proxy_;
    QNetworkProxy saved_proxy_;

    Error* ParseAndApplyProxySettings(const base::DictionaryValue* proxy_dict);

    DISALLOW_COPY_AND_ASSIGN(QSessionLifeCycleActions);
};

}  // namespace webdriver

#endif  // WEBDRIVER_QT_SESSION_ACTIONS_H_


