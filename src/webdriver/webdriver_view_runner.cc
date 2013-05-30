#include "webdriver_view_runner.h"

#include "base/bind.h"
#include "base/callback.h"
#include "base/synchronization/waitable_event.h"

namespace webdriver {

ViewRunner::CreateRunnerMethod ViewRunner::create = &(ViewRunner::createRunner<ViewRunner>);

ViewRunner::ViewRunner() {}

void ViewRunner::RunClosure(const base::Closure& task, base::WaitableEvent* done_event) {
	task.Run();
    done_event->Signal();
}

ViewRunner* ViewRunner::CreateRunner(void) {
	return create();
}

} //namespace webdriver 
