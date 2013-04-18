 /* WindowTest.h
   */

#include <QtCore/qglobal.h>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets/QWidget>
#else
#include <QtGui/QWidget>
#endif

class WindowTestWidget : public QWidget
{
public:
    WindowTestWidget();
    ~WindowTestWidget();
};
