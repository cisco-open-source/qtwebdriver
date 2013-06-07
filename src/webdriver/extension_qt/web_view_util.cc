#include "web_view_util.h"

namespace webdriver {

bool QWebViewUtil::isUrlSupported(const std::string& url) {
	// TODO: implement content detection
	const std::string web_prefix("http://");

	if (url.compare(0, web_prefix.length(), web_prefix) == 0) {
		return true;
	}

	return false;
}	


} // namespace webdriver
