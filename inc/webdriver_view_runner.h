#ifndef WEBDRIVER_VIEW_RUNNER_H_
#define WEBDRIVER_VIEW_RUNNER_H_

#include <string>
#include <vector>

#include "base/basictypes.h"
#include "base/callback_forward.h"

namespace base {
class WaitableEvent;
}

namespace webdriver {

/// Base class for runner for view operations.
/// It allows to run operations in other context(ie - UI thread).
/// Has default imlementation which runs closure in same context.
class ViewRunner {
protected:
    typedef ViewRunner* (*CreateRunnerMethod)(void);
public:
    ViewRunner();
    virtual ~ViewRunner() {};

    /// run view operation in runner context.
    /// Finish if operation should be signaled in done_event.
    /// @param task operation to run
    /// @param done_event event to signalize finis
    virtual void RunClosure(const base::Closure& task,
                            base::WaitableEvent* done_event);

    /// Create view runner
    /// @return new ViewRunner object.
    static ViewRunner* CreateRunner(void);

    /// Register custom runner. There is can be only one runner per driver instance.
    /// @tparam C subclass of ViewRunner with custom implementation
    template <class C>
    static void RegisterCustomRunner(void) {
        create = &createRunner<C>;
    }

protected:
    template <class C>
    static ViewRunner* createRunner(void) { return static_cast<ViewRunner*>(new C());}

    static CreateRunnerMethod create;
private:

    DISALLOW_COPY_AND_ASSIGN(ViewRunner);
};

}  // namespace webdriver

#endif  // WEBDRIVER_VIEW_RUNNER_H_
