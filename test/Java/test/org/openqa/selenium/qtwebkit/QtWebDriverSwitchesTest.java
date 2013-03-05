package org.openqa.selenium.qtwebkit;

import org.junit.*;

import java.util.Arrays;

import org.openqa.selenium.Dimension;
import org.openqa.selenium.Point;
import org.openqa.selenium.WebDriver;
import org.openqa.selenium.chrome.ChromeDriver;
import org.openqa.selenium.chrome.ChromeOptions;
import org.openqa.selenium.remote.DesiredCapabilities;
import org.openqa.selenium.remote.RemoteWebDriver;
import org.openqa.selenium.testing.JUnit4TestBase;
import org.openqa.selenium.testing.NeedsLocalEnvironment;

import java.net.MalformedURLException;
import java.net.URL;
import java.util.concurrent.Callable;

import static org.junit.Assert.assertEquals;
import static org.openqa.selenium.TestWaiter.waitFor;

public class QtWebDriverSwitchesTest extends JUnit4TestBase {

    @Before
    public void createDriver() throws Exception {
    }

    @After
    public void theerDown() throws Exception {
        driver.quit();
    }

    @NeedsLocalEnvironment
    @Test
    public void canStartWebDriverWithPos() {
        URL hostURL;
        Point point = new Point(80, 60);
        String arg = "--window-position=" + point.getX() + "," + point.getY();
        DesiredCapabilities capabilities = DesiredCapabilities.chrome();
        capabilities.setCapability("chrome.switches", Arrays.asList(arg));

        try {
            hostURL = new URL("http://localhost:9517");
        } catch (MalformedURLException e) {
            e.printStackTrace();  //To change body of catch statement use File | Settings | File Templates.
            return;
        }
        driver = new RemoteWebDriver(
                hostURL, capabilities);

        driver.get(pages.colorPage);

        waitFor(xEqual(driver, point));
        waitFor(yEqual(driver, point));

        sleep(2);



    }

    @NeedsLocalEnvironment
    @Test
    public void canStartWebDriverWithSize() {
        URL hostURL;
        Dimension size = new Dimension(600, 600);
        String arg = "--window-size=" + size.getWidth() + ", " + size.getHeight();
        DesiredCapabilities capabilities = DesiredCapabilities.chrome();
        capabilities.setCapability("chrome.switches", Arrays.asList(arg));

        try {
            hostURL = new URL("http://localhost:9517");
        } catch (MalformedURLException e) {
            e.printStackTrace();  //To change body of catch statement use File | Settings | File Templates.
            return;
        }
        driver = new RemoteWebDriver(
                hostURL, capabilities);

        driver.get(pages.colorPage);

        waitFor(windowHeightToEqual(driver, size));
        waitFor(windowWidthToEqual(driver, size));

        sleep(2);

    }

    @NeedsLocalEnvironment
    @Test
    public void canStartWebDriverMaximized() {
        URL hostURL;
        DesiredCapabilities capabilities = DesiredCapabilities.chrome();
        capabilities.setCapability("chrome.switches", Arrays.asList("--start-maximized"));

        try {
            hostURL = new URL("http://localhost:9517");
        } catch (MalformedURLException e) {
            e.printStackTrace();  //To change body of catch statement use File | Settings | File Templates.
            return;
        }
        driver = new RemoteWebDriver(
                hostURL, capabilities);

        driver.get(pages.colorPage);

        Dimension maximizedSize = driver.manage().window().getSize();

        driver.manage().window().setSize(new Dimension(10, 10));
        driver.manage().window().maximize();

        waitFor(windowHeightToEqual(driver, maximizedSize));
        waitFor(windowWidthToEqual(driver, maximizedSize));

        sleep(2);

    }

    private Callable<Boolean> windowHeightToEqual(final WebDriver driver, final Dimension size) {
        return new Callable<Boolean>() {
            public Boolean call() throws Exception {
                Dimension newSize = driver.manage().window().getSize();
                if(newSize.height == size.height) {
                    return true;
                }

                return null;
            }
        };
    }

    private Callable<Boolean> windowWidthToEqual(final WebDriver driver, final Dimension size) {
        return new Callable<Boolean>() {
            public Boolean call() throws Exception {
                Dimension newSize = driver.manage().window().getSize();
                if(newSize.width == size.width) {
                    return true;
                }
                return null;
            }
        };
    }

    private Callable<Boolean> xEqual(final WebDriver driver, final Point targetPosition) {
        return new Callable<Boolean>() {
            public Boolean call() throws Exception {
                Point newPosition = driver.manage().window().getPosition();
                if(newPosition.x == targetPosition.x) {
                    return true;
                }

                return null;
            }
        };
    }
    private Callable<Boolean> yEqual(final WebDriver driver, final Point targetPosition) {
        return new Callable<Boolean>() {
            public Boolean call() throws Exception {
                Point newPosition = driver.manage().window().getPosition();
                if(newPosition.y == targetPosition.y) {
                    return true;
                }

                return null;
            }
        };
    }

    private void sleep(int sec) {
        try {
            Thread.sleep(sec*1000);
        } catch (InterruptedException ie) {
            //Handle exception
        }
    }
}
