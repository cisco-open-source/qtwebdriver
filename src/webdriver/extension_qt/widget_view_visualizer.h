#ifndef WIDGET_VIEW_VISUALIZER_H
#define WIDGET_VIEW_VISUALIZER_H

#include <string>
#include <QtGui/QWidget>
#include "webdriver_view_id.h"

namespace webdriver {

class Error;
class Session;

class QWidgetViewVisualizerSourceCommand {
public:
    QWidgetViewVisualizerSourceCommand(Session* session, ViewId viewId, QWidget* view);
    void Execute(std::string* source, Error** error);

private:
    std::string transform(const std::string& source, const std::string& stylesheet) const;

    Session* session_;
    ViewId viewId_;
    QWidget* view_;
};

}  // namespace webdriver

#endif // WIDGET_VIEW_VISUALIZER_H

