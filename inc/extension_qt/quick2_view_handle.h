#ifndef WEBDRIVER_QUICK2_VIEW_HANDLE_H_
#define WEBDRIVER_QUICK2_VIEW_HANDLE_H_

#include <string>

#include "webdriver_view_id.h"

#include <QtCore/QPointer>

#include <QtCore/QtGlobal>
#include <QtQuick/QQuickWindow>

namespace webdriver {

class Quick2ViewHandle : public ViewHandle {
public:
    Quick2ViewHandle();
    Quick2ViewHandle(QQuickWindow* view);
    
    virtual bool is_valid() const { return !view_.isNull(); };
    virtual bool equals(const ViewHandle* other) const;
    QQuickWindow* get() { return view_.data(); };
    
protected:
    QPointer<QQuickWindow> view_;
//private:
	virtual ~Quick2ViewHandle() {};
};

}  // namespace webdriver

#endif  // WEBDRIVER_QUICK2_VIEW_HANDLE_H_
