#include "webdriver_view_transitions.h"

#include "base/bind.h"
#include "base/memory/scoped_ptr.h"
#include "webdriver_error.h"
#include "webdriver_session.h"
#include "webdriver_view_executor.h"

namespace webdriver {

URLTransitionActionPtr ViewTransitionManager::urlTransitionAction_(NULL);

void ViewTransitionManager::SetURLTransitionAction(URLTransitionAction* transitionAction) {
	urlTransitionAction_.reset(transitionAction);
}

void ViewTransitionManager::HandleOldView(Session* session, const ViewId& viewId) {
	if (NULL != urlTransitionAction_.get()) {
		urlTransitionAction_->HandleOldView(session, viewId);
	}
}

void URLTransitionAction_CloseOldView::HandleOldView(Session* session, const ViewId& viewId) const {
	typedef scoped_ptr<ViewCmdExecutor> ExecutorPtr;        

    Error* error = NULL;
    scoped_ptr<Error> ignore_error(NULL);

    ExecutorPtr executor(ViewCmdExecutorFactory::GetInstance()->CreateExecutor(session, viewId));

    if (NULL == executor.get()) {
        return;
    }

    session->RunSessionTask(base::Bind(
                &ViewCmdExecutor::Close,
                base::Unretained(executor.get()),
                &error));

    ignore_error.reset(error);
}

} // namespace webdriver