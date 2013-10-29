#ifndef WEBDRIVER_QWINDOW_VIEW_HANDLE_H_
#define WEBDRIVER_QWINDOW_VIEW_HANDLE_H_

#include <string>

#include "webdriver_view_id.h"

#include <QtCore/QPointer>

#include <QtCore/QtGlobal>
#include <QtGui/QWindow>

namespace webdriver {

class QWindowViewHandle : public ViewHandle {
public:
    QWindowViewHandle();
    QWindowViewHandle(QWindow* view);
    
    virtual bool is_valid() const { return !view_.isNull(); };
    virtual bool equals(const ViewHandle* other) const;
    QWindow* get() { return view_.data(); };
    
protected:
    QPointer<QWindow> view_;
//private:
	virtual ~QWindowViewHandle() {};
};

}  // namespace webdriver

#endif  // WEBDRIVER_QWINDOW_VIEW_HANDLE_H_
