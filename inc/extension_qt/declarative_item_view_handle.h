#ifndef WEBDRIVER_QT_DECLARATIVE_ITEM_VIEW_H_
#define WEBDRIVER_QT_DECLARATIVE_ITEM_VIEW_H_

#include <string>

#include "webdriver_view_id.h"

#include <QtDeclarative/QDeclarativeItem>
#include <QtDeclarative/QDeclarativeView>
#include <QtCore/QPointer>
#include <QtCore/QtGlobal>

namespace webdriver {

class QDeclarativeItemViewHandle : public ViewHandle {
public:
    QDeclarativeItemViewHandle();
    QDeclarativeItemViewHandle(QDeclarativeItem* view, QDeclarativeView* container);
    
    virtual bool is_valid() const { return !view_.isNull(); };
    virtual bool equals(const ViewHandle* other) const;
    QDeclarativeItem* get() { return view_.data(); };
    QDeclarativeView* getContainer() { return container_.data(); };
    
protected:
    QPointer<QDeclarativeItem> view_;
    QPointer<QDeclarativeView> container_;
//private:
	virtual ~QDeclarativeItemViewHandle() {};
};

}  // namespace webdriver

#endif  // WEBDRIVER_QT_DECLARATIVE_ITEM_VIEW_H_
