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

/// base class for runner for view operations.
/// Contains default imlementation.
class ViewRunner {
private:
    typedef ViewRunner* (*CreateRunnerMethod)(void);
public:
    virtual ~ViewRunner() {};

    /// run view operation in runner context
    /// @param task operation to run
    /// @param done_event event to signalize finis
    virtual void RunClosure(const base::Closure& task,
                            base::WaitableEvent* done_event);

    /// Create view runner
    /// @return new ViewRunner object.
    static ViewRunner* CreateRunner(void);

    /// Register custom runner.
    /// @tparam C subclass of ViewRunner with custom implementation
    template <class C>
    static void RegisterCustomRunner(void) {
        create = &createRunner<C>;
    }

private:
    ViewRunner();

    template <class C>
    static ViewRunner* createRunner(void) { return static_cast<ViewRunner*>(new C());}

    static CreateRunnerMethod create;

    DISALLOW_COPY_AND_ASSIGN(ViewRunner);
};

}  // namespace webdriver

#endif  // WEBDRIVER_VIEW_RUNNER_H_
