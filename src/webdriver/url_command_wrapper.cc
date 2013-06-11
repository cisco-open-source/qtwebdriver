#include "url_command_wrapper.h"

#include "base/values.h"
#include "base/bind.h"
#include "base/memory/scoped_ptr.h"
#include "commands/response.h"
#include "webdriver_error.h"
#include "webdriver_session.h"
#include "webdriver_util.h"
#include "webdriver_view_executor.h"
#include "webdriver_view_factory.h"


namespace webdriver {

WebDriverCommandWrapper::WebDriverCommandWrapper(const std::vector<std::string>& path_segments,
             				const DictionaryValue* const parameters,
             				WebDriverCommand* delegate) 
		: WebDriverCommand(path_segments, parameters),
		  delegate_(delegate) {
}

WebDriverCommandWrapper::~WebDriverCommandWrapper() { }

bool WebDriverCommandWrapper::DoesDelete() {
	return delegate_->DoesDelete();
}

bool WebDriverCommandWrapper::DoesGet() {
	return delegate_->DoesGet();
}

bool WebDriverCommandWrapper::DoesPost() {
	return delegate_->DoesPost();
}

bool WebDriverCommandWrapper::Init(Response* const response) {
	return delegate_->Init(response);
}

void WebDriverCommandWrapper::Finish(Response* const response) {
	return delegate_->Finish(response);
}

void WebDriverCommandWrapper::ExecuteDelete(Response* const response) {
	return delegate_->ExecuteDelete(response);
}

void WebDriverCommandWrapper::ExecuteGet(Response* const response) {
	return delegate_->ExecuteGet(response);
}

void WebDriverCommandWrapper::ExecutePost(Response* const response) {
	return delegate_->ExecutePost(response);
}

UrlCommandWrapper::UrlCommandWrapper(const std::vector<std::string>& path_segments,
             const DictionaryValue* const parameters,
             WebDriverCommand* delegate) 
	: WebDriverCommandWrapper(path_segments, parameters, delegate) {
}

UrlCommandWrapper::~UrlCommandWrapper() {}

typedef scoped_ptr<ViewCmdExecutor> ExecutorPtr;    

void UrlCommandWrapper::ExecutePost(Response* const response) {

// TODO:
#if 0    
	std::string url;
	bool can_handle;
    Error* error = NULL;

    if (!GetStringASCIIParameter("url", &url)) {
        response->SetError(new Error(kBadRequest, "Missing 'url' parameter"));
        return;
    }

	ExecutorPtr executor(ViewCmdExecutorFactory::GetInstance()->CreateExecutor(session_, session_->current_view()));
    if (NULL == executor.get()) {
        response->SetError(new Error(kBadRequest, "cant get view executor."));
        return;
    }

    session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::CanHandleUrl,
            base::Unretained(executor.get()),
            url,
            &can_handle,
            &error));

    if (error) {
        response->SetError(error);
        return;
    }

    if (!can_handle) {
    	session_->logger().Log(kInfoLogLevel, "Current view cant handle url, try to create another - "+url);
    	// TODO:
    	//Step 2: Y->handleOldViews() can be customized
		//Y.get/createInstance()->handleOldViews(oldViews);

    	ViewId viewId;

		session_->RunSessionTask(base::Bind(
            &ViewFactory::CreateViewForUrl,
            base::Unretained(ViewFactory::GetInstance()),
            session_,
            url,
            &viewId));

		if (!viewId.is_valid()) {
			session_->logger().Log(kSevereLogLevel, "cant create view able to handle url.");
			response->SetError(new Error(kBadRequest, "cant create view able to handle url."));
			return;
		}

		executor.reset(ViewCmdExecutorFactory::GetInstance()->CreateExecutor(session_, viewId));
   		if (NULL == executor.get()) {
   			session_->logger().Log(kSevereLogLevel, "cant get executor for new view.");
        	response->SetError(new Error(kBadRequest, "cant get view executor."));
        	return;
    	}

		session_->RunSessionTask(base::Bind(
            &ViewCmdExecutor::SwitchTo,
            base::Unretained(executor.get()),
            &error));
    }
		
	if (error) {
        response->SetError(error);
        return;
    }
#endif
    delegate_->ExecutePost(response);
}

} // namespace webdriver
