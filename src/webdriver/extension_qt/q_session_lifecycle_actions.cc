#include "extension_qt/q_session_lifecycle_actions.h"

#include "q_proxy_parser.h"

namespace webdriver {

QSessionLifeCycleActions::QSessionLifeCycleActions(Session* session)
	: SessionLifeCycleActions(session) {}

Error* QSessionLifeCycleActions::PostInit(const base::DictionaryValue* desired_capabilities_dict,
                const base::DictionaryValue* required_capabilities_dict) {
	Error* error = NULL;
	session_->logger().Log(kInfoLogLevel, "Custom QT PostInit action.");

	const DictionaryValue* proxy_settings = NULL;
	if (required_capabilities_dict) 
		required_capabilities_dict->GetDictionary(Capabilities::kProxy, &proxy_settings);

	if (!proxy_settings && desired_capabilities_dict)
		desired_capabilities_dict->GetDictionary(Capabilities::kProxy, &proxy_settings);

	if (proxy_settings) {
		session_->logger().Log(kInfoLogLevel, "proxy settings requsted, aplying...");
		error = ParseAndApplyProxySettings(proxy_settings);
		if (error)
			return error;
	} else {
		session_->logger().Log(kInfoLogLevel, "no proxy settings requsted.");
	}

	return error;
}

void QSessionLifeCycleActions::BeforeTerminate(void) {
	session_->logger().Log(kInfoLogLevel, "Custom QT BeforeTerminate action.");
}

Error* QSessionLifeCycleActions::ParseAndApplyProxySettings(const base::DictionaryValue* proxy_dict) {
	QNetworkProxy proxy;

    QProxyCapabilitiesParser parser(proxy_dict, session_->logger(), &proxy);
    Error* error = parser.Parse();
    if (error) {
    	session_->logger().Log(kWarningLogLevel, "cant parse proxy settings.");
        return error;
    }

    QNetworkProxy::setApplicationProxy(proxy);

    return NULL;
}

} // namespace webdriver
