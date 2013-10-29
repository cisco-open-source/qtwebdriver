#ifndef WEBDRIVER_QT_GRAPHICS_WEB_VIEW_H_
#define WEBDRIVER_QT_GRAPHICS_WEB_VIEW_H_

#include <string>

#include "webdriver_view_id.h"

#include <QtCore/QPointer>

#include <QtCore/QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWebKitWidgets/QGraphicsWebView>
#else
#include <QtWebKit/QGraphicsWebView>
#endif

namespace webdriver {

class GraphicsWebViewHandle : public ViewHandle {
public:
    GraphicsWebViewHandle();
    GraphicsWebViewHandle(QGraphicsWebView* view);
    
    virtual bool is_valid() const { return !view_.isNull(); };
    virtual bool equals(const ViewHandle* other) const;
    QGraphicsWebView* get() { return view_.data(); };
    
protected:
    QPointer<QGraphicsWebView> view_;
//private:
	virtual ~GraphicsWebViewHandle() {};
};

}  // namespace webdriver

#endif  // WEBDRIVER_QT_GRAPHICS_WEB_VIEW_H_
