# to run test suite type next in the command line:
# > python ./selenium_test_suite.py

import unittest
import cookie_tests
import frame_switching_tests
import executing_javascript_tests
import executing_async_javascript_tests
import driver_element_finding_tests
import appcache_tests
import api_example_tests
import rendered_webelement_tests
import opacity_tests
import window_tests
import typing_tests
import text_handling_tests
import children_finding_tests
import element_attribute_tests
import clear_tests
import visibility_tests
import click_tests
import form_handling_tests
import click_scrolling_tests
import implicit_waits_tests
import page_loading_tests
import correct_event_firing_tests
import alerts_tests

import HTMLTestRunner
from optparse import OptionParser

import sys
from selenium import webdriver
from selenium.webdriver.remote.webdriver import WebDriver

capabilities = webdriver.DesiredCapabilities.CHROME
driver = webdriver.Remote("http://localhost:9517", desired_capabilities=capabilities)

def suite():
    cookie_tests.CookieTest.driver = driver
    frame_switching_tests.FrameSwitchingTest.driver = driver
    executing_javascript_tests.ExecutingJavaScriptTests.driver = driver
    executing_async_javascript_tests.ExecutingAsyncJavaScriptTests.driver = driver
    driver_element_finding_tests.DriverElementFindingTests.driver = driver
    appcache_tests.AppCacheTests.driver = driver
    api_example_tests.ApiExampleTest.driver = driver
    rendered_webelement_tests.RenderedWebElementTests.driver = driver
    opacity_tests.OpacityTests.driver = driver
    window_tests.WindowTests.driver = driver
    typing_tests.TypingTests.driver = driver
    text_handling_tests.TextHandlingTests.driver = driver
    children_finding_tests.ChildrenFindingTests.driver = driver
    element_attribute_tests.ElementAttributeTests.driver = driver
    clear_tests.ClearTests.driver = driver
    visibility_tests.VisibilityTests.driver = driver
    click_tests.ClickTest.driver = driver
    click_scrolling_tests.ClickScrollingTest.driver = driver
    form_handling_tests.FormHandlingTests.driver = driver
    implicit_waits_tests.ImplicitWaitTest.driver = driver
    page_loading_tests.PageLoadingTests.driver = driver
    correct_event_firing_tests.CorrectEventFiringTests.driver = driver
    alerts_tests.AlertsTest.driver = driver

    return unittest.TestSuite((\
        unittest.makeSuite(alerts_tests.AlertsTest),
        unittest.makeSuite(click_scrolling_tests.ClickScrollingTest),
        unittest.makeSuite(click_tests.ClickTest),
        unittest.makeSuite(visibility_tests.VisibilityTests),
        unittest.makeSuite(cookie_tests.CookieTest),
        unittest.makeSuite(frame_switching_tests.FrameSwitchingTest),
        unittest.makeSuite(executing_javascript_tests.ExecutingJavaScriptTests),
        unittest.makeSuite(executing_async_javascript_tests.ExecutingAsyncJavaScriptTests),
        unittest.makeSuite(driver_element_finding_tests.DriverElementFindingTests),
        unittest.makeSuite(appcache_tests.AppCacheTests),
        unittest.makeSuite(api_example_tests.ApiExampleTest),
        unittest.makeSuite(rendered_webelement_tests.RenderedWebElementTests),
        unittest.makeSuite(opacity_tests.OpacityTests),
        unittest.makeSuite(text_handling_tests.TextHandlingTests),
        unittest.makeSuite(window_tests.WindowTests),
        unittest.makeSuite(typing_tests.TypingTests),
        unittest.makeSuite(children_finding_tests.ChildrenFindingTests),
        unittest.makeSuite(element_attribute_tests.ElementAttributeTests),
        unittest.makeSuite(clear_tests.ClearTests),
        unittest.makeSuite(form_handling_tests.FormHandlingTests),
        unittest.makeSuite(implicit_waits_tests.ImplicitWaitTest),
        unittest.makeSuite(page_loading_tests.PageLoadingTests),
        unittest.makeSuite(correct_event_firing_tests.CorrectEventFiringTests),
         ))

if __name__ == "__main__":
    usage = "usage: %prog [options] arg"
    parser = OptionParser(usage)

    parser.add_option("-r", "--report",
                      action="store_true", dest="report")
    (options, args) = parser.parse_args()

    if options.report:
        outfile = open("./Python_Test_Report.html", "w")
        runner = HTMLTestRunner.HTMLTestRunner(
                    stream=outfile,
                    title='Python Test Report',
                    description='Test result of Pyton Unit tests for QtWebDriver'
                    )
        result = runner.run(suite())
    else:
        result = unittest.TextTestRunner(verbosity=2).run(suite())

    driver.close()
    sys.exit(not result.wasSuccessful())

