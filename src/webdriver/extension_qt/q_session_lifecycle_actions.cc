#include "extension_qt/q_session_lifecycle_actions.h"

namespace webdriver {

QSessionLifeCycleActions::QSessionLifeCycleActions(Session* session)
	: SessionLifeCycleActions(session) {}

Error* QSessionLifeCycleActions::PostInit(const base::DictionaryValue* desired_capabilities_dict,
                const base::DictionaryValue* required_capabilities_dict) {
	session_->logger().Log(kFineLogLevel, "Custom QT PostInit action.");
	return NULL;
}

void QSessionLifeCycleActions::BeforeTerminate(void) {
	session_->logger().Log(kFineLogLevel, "Custom QT BeforeTerminate action.");
}

} // namespace webdriver
