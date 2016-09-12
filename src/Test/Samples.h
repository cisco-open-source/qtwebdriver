/****************************************************************************
**
** Copyright © 1992-2014 Cisco and/or its affiliates. All rights reserved.
** All rights reserved.
** 
** $CISCO_BEGIN_LICENSE:LGPL$
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** $CISCO_END_LICENSE$
**
****************************************************************************/

#include "WindowTest.h"
#include "ClickTest.h"
#include "ElementAttributeTest.h"
#include "FindingTest.h"
#include "CoordinatesTest.h"
#include "ClickScrollingTest.h"
#include "ElementSelectingTest.h"
#include "TypingTest.h"
#include "BasicKeyboardInterfaceTest.h"
#include "TextHandlingTest.h"
#include "FormHandlingTest.h"
#include "XPathElementFindingTest.h"
#include "StaleElementReferenceTest.h"
#include "VisibilityTest.h"
#include "BasicMouseInterfaceTest.h"
#include "TouchTest.h"
#include "MenuTest.h"

#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
#include "WindowWithDeclarativeViewTest.h"
#endif

#if (1 == WD_ENABLE_PLAYER) && (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include "VideoTest.h"
extern std::string testDataFolder;
#endif // WD_ENABLE_PLAYER

#if (WD_ENABLE_WEB_VIEW == 1)
#include "GraphicsWebViewTest.h"
#include "WindowWithEmbeddedViewTest.h"
#include "WidgetAndWebViewTest.h"
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
#include "WindowWithSeparatedDeclarativeAndWebViewsTest.h"
#endif //QT_VERSION
#endif //WD_ENABLE_WEB_VIEW

int wd_samples_setup(webdriver::ViewCreator* widgetCreator, 
	webdriver::ViewCreator* webCreator, 
	webdriver::ViewCreator* qmlCreator,
	CommandLine &cmd_line);