package org.openqa.selenium.qtwebkit;

import org.junit.runner.RunWith;
import org.junit.runners.Suite;
import org.openqa.selenium.StandardSeleniumTests;

@RunWith(Suite.class)
@Suite.SuiteClasses({
        StandardSeleniumTests.class,
        QtWebDriverSwitchesTest.class
})

public class QtWebkitDriverTest {
}
