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
#include "webdriver_session_manager.h"


namespace webdriver {

CommandWrapper::CommandWrapper(const std::vector<std::string>& path_segments,
             				const DictionaryValue* const parameters,
             				Command* delegate) 
		: Command(path_segments, parameters),
		  delegate_(delegate) {
}

CommandWrapper::~CommandWrapper() { }

bool CommandWrapper::DoesDelete() {
	return delegate_->DoesDelete();
}

bool CommandWrapper::DoesGet() {
	return delegate_->DoesGet();
}

bool CommandWrapper::DoesPost() {
	return delegate_->DoesPost();
}

bool CommandWrapper::Init(Response* const response) {
	return delegate_->Init(response);
}

void CommandWrapper::Finish(Response* const response) {
	delegate_->Finish(response);
}

void CommandWrapper::ExecuteDelete(Response* const response) {
	delegate_->ExecuteDelete(response);
}

void CommandWrapper::ExecuteGet(Response* const response) {
	delegate_->ExecuteGet(response);
}

void CommandWrapper::ExecutePost(Response* const response) {
	delegate_->ExecutePost(response);
}

UrlCommandWrapper::UrlCommandWrapper(const std::vector<std::string>& path_segments,
             const DictionaryValue* const parameters,
             Command* delegate) 
	: CommandWrapper(path_segments, parameters, delegate) {
}

UrlCommandWrapper::~UrlCommandWrapper() {}

typedef scoped_ptr<ViewCmdExecutor> ExecutorPtr;    

void UrlCommandWrapper::ExecutePost(Response* const response) {
    Session* session;
    std::string session_id;
    Error* error = NULL;

    do {
        // There should be at least 3 path segments to match "/session/$id".
        session_id = GetPathVariable(2);
        if (session_id.length() == 0) {
            break;
        }

        session = SessionManager::GetInstance()->GetSession(session_id);
        if (session == NULL) {
            break;
        }
    
	    std::string url;
	    bool can_handle;

        if (!GetStringASCIIParameter("url", &url)) {
            break;
        }

        ViewId current_view = session->current_view();
	    ExecutorPtr executor(ViewCmdExecutorFactory::GetInstance()->CreateExecutor(session, current_view));
        if (NULL == executor.get()) {
            break;
        }

        session->RunSessionTask(base::Bind(
                    &ViewCmdExecutor::CanHandleUrl,
                    base::Unretained(executor.get()),
                    url,
                    &can_handle,
                    &error));

        if (error) {
            break;
        }

        if (!can_handle) {
            session->logger().Log(kInfoLogLevel, "Current view cant handle url, try to create another - "+url);
            // TODO:
    	    //Step 2: Y->handleOldViews() can be customized
		    //Y.get/createInstance()->handleOldViews(oldViews);

            ViewHandle* viewHandle = NULL;
    	    ViewId viewId;

		    session->RunSessionTask(base::Bind(
                        &ViewFactory::CreateViewForUrl,
                        base::Unretained(ViewFactory::GetInstance()),
                        session->logger(),
                        url,
                        &viewHandle));

            if (NULL == viewHandle) {
			    session->logger().Log(kSevereLogLevel, "cant create view able to handle url.");
			    break;
    		}

            session->AddNewView(viewHandle, &viewId);
            if (!viewId.is_valid()) {
                viewHandle->Release();
                session->logger().Log(kSevereLogLevel, "Cant add view handle to session.");
                break;
            }

            session->logger().Log(kInfoLogLevel, "New view("+viewId.id()+") created for url - "+url);

		    executor.reset(ViewCmdExecutorFactory::GetInstance()->CreateExecutor(session, viewId));
   			if (NULL == executor.get()) {
   				session->logger().Log(kSevereLogLevel, "cant get executor for new view.");
        		break;
	    	}

			session->RunSessionTask(base::Bind(
            	&ViewCmdExecutor::SwitchTo,
            	base::Unretained(executor.get()),
            	&error));
    	}

    	if (error) {
    		break;
    	}

    } while(0);

    scoped_ptr<Error> ignore_error(error);

	// we need to release scoped_ptr parameters_.
	// Because it references same data as delegate.		
    const void* ignore_pointer = parameters_.release();
    // to avoid "unused variable" warning
    (void)ignore_pointer;
    
    delegate_->ExecutePost(response);
}

} // namespace webdriver
