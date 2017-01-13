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

#ifndef SAMPLES_SETUP_H
#define SAMPLES_SETUP_H

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
    CommandLine &cmd_line)
{
    if(widgetCreator != NULL)
    {
        widgetCreator->RegisterViewClass<WindowTestWidget>("WindowTestWidget");
        widgetCreator->RegisterViewClass<ClickTestWidget>("ClickTestWidget");
        widgetCreator->RegisterViewClass<ElementAttributeTestWidget>("ElementAttributeTestWidget");
        widgetCreator->RegisterViewClass<FindingTestWidget>("FindingTestWidget");
        widgetCreator->RegisterViewClass<CoordinatesTestWidget>("CoordinatesTestWidget");
        widgetCreator->RegisterViewClass<ClickScrollingTestWidget>("ClickScrollingTestWidget");
        widgetCreator->RegisterViewClass<ElementSelectingTestWidget>("ElementSelectingTestWidget");
        widgetCreator->RegisterViewClass<TypingTestWidget>("TypingTestWidget");
        widgetCreator->RegisterViewClass<BasicKeyboardInterfaceTestWidget>("BasicKeyboardInterfaceTestWidget");
        widgetCreator->RegisterViewClass<TextHandlingTestWidget>("TextHandlingTestWidget");
        widgetCreator->RegisterViewClass<FormHandlingTestWidget>("FormHandlingTestWidget");
        widgetCreator->RegisterViewClass<XPathElementFindingTestWidget>("XPathElementFindingTestWidget");
        widgetCreator->RegisterViewClass<StaleElementReferenceTestWidget>("StaleElementReferenceTestWidget");
        widgetCreator->RegisterViewClass<VisibilityTestWidget>("VisibilityTestWidget");
        widgetCreator->RegisterViewClass<BasicMouseInterfaceTestWidget>("BasicMouseInterfaceTestWidget");
        widgetCreator->RegisterViewClass<TouchTestWidget>("TouchTestWidget");
        widgetCreator->RegisterViewClass<MenuTestWidget>("MenuTestWidget");
    #if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
        widgetCreator->RegisterViewClass<WindowWithDeclarativeViewTestWidget>("WindowWithDeclarativeViewTestWidget");
    #endif

    #if (1 == WD_ENABLE_PLAYER) && (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        widgetCreator->RegisterViewClass<VideoTestWidget>("VideoTestWidget");
    #endif //WD_ENABLE_PLAYER

    #if (WD_ENABLE_WEB_VIEW == 1)
        /* Register som test classes */
        widgetCreator->RegisterViewClass<GraphicsWebViewTestWindows>("GraphicsWebViewTestWindows");
        widgetCreator->RegisterViewClass<WindowWithEmbeddedViewTestWidget>("WindowWithEmbeddedViewTestWidget");
        widgetCreator->RegisterViewClass<WidgetAndWebViewTestWindows>("WidgetAndWebViewTestWindows");

    #if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
        widgetCreator->RegisterViewClass<WindowWithSeparatedDeclarativeAndWebViewsTestWidget>("WindowWithSeparatedDeclarativeAndWebViewsTestWidget");
    #endif // QT_VERSION
    #endif // WD_ENABLE_WEB_VIEW
    }
#if (1 == WD_ENABLE_PLAYER) && (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    // check if --test_data_folder CL argument is present
    std::string testDataFolderSwitch = "test_data_folder";

    if (cmd_line.HasSwitch(testDataFolderSwitch)) {
      testDataFolder = cmd_line.GetSwitchValueASCII(testDataFolderSwitch);
    } else {
      testDataFolder = "./";
    }

    std::cout << "Using "<< testDataFolder << " as test data folder" << std::endl;
#endif // WD_ENABLE_PLAYER
    return 0;
}
#endif // SAMPLES_SETUP_H
