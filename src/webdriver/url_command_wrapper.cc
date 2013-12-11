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
#include "webdriver_view_transitions.h"
#include "webdriver_basic_types.h"


namespace webdriver {

CommandWrapper::CommandWrapper(Command* delegate)
        : Command(*delegate),
		  delegate_(delegate) {
}

CommandWrapper::~CommandWrapper() { }

bool CommandWrapper::DoesDelete() const {
	return delegate_->DoesDelete();
}

bool CommandWrapper::DoesGet() const {
	return delegate_->DoesGet();
}

bool CommandWrapper::DoesPost() const {
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

UrlCommandWrapper::UrlCommandWrapper(Command* delegate)
    : CommandWrapper(delegate) {
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
            
            ViewHandle* viewHandle = NULL;
    	    ViewId viewId;

            std::string window_size;
            int w, h;
            scoped_ptr<Size> size(NULL);
            if (session->get_desired_caps()->GetString(Capabilities::kWindowSize, &window_size)) {
                if (GetTwoIntsFromString(window_size, w, h)) {
                    size.reset(new Size(w, h));
                }
            }

            std::string window_position;
            int x, y;
            scoped_ptr<Point> position(NULL);
            if (session->get_desired_caps()->GetString(Capabilities::kWindowPosition, &window_position)) {

                if (GetTwoIntsFromString(window_position, x, y)) {
                    position.reset(new Point(x, y));
                }
            }

            typedef void (ViewFactory::*CreateViewForUrl)(const Logger&, const std::string&,
                                                          const Point*, const Size*, ViewHandle**) const;
            CreateViewForUrl createViewForUrl = static_cast<CreateViewForUrl>(&ViewFactory::CreateViewForUrl);

            session->RunSessionTask(base::Bind(
                    createViewForUrl,
                    base::Unretained(ViewFactory::GetInstance()),
                    session->logger(),
                    url,
                    position.get(),
                    size.get(),
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

            ViewTransitionManager::HandleOldView(session, current_view);

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

    delegate_->ExecutePost(response);
}

} // namespace webdriver
