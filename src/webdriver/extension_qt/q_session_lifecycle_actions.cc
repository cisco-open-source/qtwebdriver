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

#include "extension_qt/q_session_lifecycle_actions.h"
#include "extension_qt/wd_event_dispatcher.h"
#include "extension_qt/vnc_event_dispatcher.h"
#include "webdriver_switches.h"
#include "webdriver_server.h"
#include "webdriver_view_executor.h"
#include "webdriver_error.h"

#include "q_proxy_parser.h"
#include "common_util.h"
#include "base/string_util.h"
#include "base/memory/scoped_ptr.h"

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
	} else {
		session_->logger().Log(kInfoLogLevel, "no proxy settings requsted.");
        session_->logger().Log(kInfoLogLevel, "Applying system proxy by default.");
        scoped_ptr<DictionaryValue> proxy_system(new DictionaryValue());
        proxy_system.get()->SetString("proxyType", "SYSTEM");
        error = ParseAndApplyProxySettings(proxy_system.get());
    }
    if (error)
        return error;

    AddActualQtVersion();
    if (required_capabilities_dict)
        error = CheckRequiredQtVersion(required_capabilities_dict);
    if (error) {
        session_->logger().Log(kWarningLogLevel, "qt version doesn't match...");
        return error;
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

bool QSessionLifeCycleActions::AddActualQtVersion() {
    base::DictionaryValue *hybrid;
    session_->capabilities().caps->GetDictionary(Capabilities::kHybrid, &hybrid);
    hybrid->SetString("qtVersion", QCommonUtil::GetQtVersion());

    return true;
}

Error* QSessionLifeCycleActions::CheckRequiredQtVersion(const base::DictionaryValue *capabilities_dict) {
    const base::DictionaryValue *required_hybrid_caps;
    base::DictionaryValue *actual_hybrid_caps;

    if (capabilities_dict->GetDictionary(Capabilities::kHybrid, &required_hybrid_caps)) {
        if (!session_->capabilities().caps->GetDictionary(Capabilities::kHybrid, &actual_hybrid_caps)) {
            return new Error(kUnknownError);
        }
        std::string required_qtVersion;
        std::string actual_qtVersion;
        if (required_hybrid_caps->GetString("qtVersion", &required_qtVersion)) {
            if (!actual_hybrid_caps->GetString("qtVersion", &actual_qtVersion)) {
                return new Error(kUnknownError);
            }
            if (StringToUpperASCII(required_qtVersion).compare("ANY")
                    && required_qtVersion.compare("") && required_qtVersion.compare("*")) {
                if(required_qtVersion.at(0) != actual_qtVersion.at(0))
                    return new Error(kUnknownError);
            }
        }
    }

    return NULL;
}

} // namespace webdriver
