#include "extension_qt/q_session_lifecycle_actions.h"
#include "extension_qt/wd_event_dispatcher.h"
#include "extension_qt/vnc_event_dispatcher.h"
#include "webdriver_switches.h"
#include "webdriver_server.h"

#include "q_proxy_parser.h"

namespace webdriver {

QSessionLifeCycleActions::QSessionLifeCycleActions(Session* session)
	: SessionLifeCycleActions(session),
	  restore_proxy_(false) {}

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

	if (restore_proxy_) {
		QNetworkProxy::setApplicationProxy(saved_proxy_);
		restore_proxy_ = false;
	}
}

Error* QSessionLifeCycleActions::ParseAndApplyProxySettings(const base::DictionaryValue* proxy_dict) {
	QNetworkProxy proxy;

    QProxyCapabilitiesParser parser(proxy_dict, session_->logger(), &proxy);
    Error* error = parser.Parse();
    if (error) {
    	session_->logger().Log(kWarningLogLevel, "cant parse proxy settings.");
        return error;
    }

    saved_proxy_ = QNetworkProxy::applicationProxy();
    restore_proxy_ = true;
    QNetworkProxy::setApplicationProxy(proxy);

    return NULL;
}

} // namespace webdriver
