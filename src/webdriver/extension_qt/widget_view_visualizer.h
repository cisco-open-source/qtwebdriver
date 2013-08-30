#ifndef WIDGET_VIEW_VISUALIZER_H
#define WIDGET_VIEW_VISUALIZER_H

#include <string>
#include <QtCore/QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets/QWidget>
#else
#include <QtGui/QWidget>
#endif
#include "webdriver_view_id.h"

namespace webdriver {

class Error;
class Session;

class QWidgetViewVisualizerSourceCommand {
public:
    QWidgetViewVisualizerSourceCommand(Session* session, ViewId viewId, QWidget* view);
    ~QWidgetViewVisualizerSourceCommand();
    void Execute(std::string* source, Error** error);

private:
    void removeAddedElements();
    std::string transform(const std::string& source, const std::string& stylesheet) const;

    Session* session_;
    ViewId viewId_;
    QWidget* view_;
    QList<QString> addedElements_;
};

}  // namespace webdriver

#endif // WIDGET_VIEW_VISUALIZER_H

