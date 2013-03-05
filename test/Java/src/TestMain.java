import org.junit.runner.JUnitCore;
import org.junit.runner.Result;
import org.junit.runner.notification.Failure;
import org.openqa.selenium.WebDriver;
import org.openqa.selenium.chrome.ChromeDriver;
import org.openqa.selenium.remote.DesiredCapabilities;
import org.openqa.selenium.remote.RemoteWebDriver;

import java.net.MalformedURLException;
import java.net.URL;

public class TestMain {
    public static void main(String[] args) throws InterruptedException {
        System.setProperty("selenium.browser", "chrome");

        DesiredCapabilities capability = DesiredCapabilities.chrome();
        URL hostURL;

        try {
            hostURL = new URL("http://localhost:9517/");
        } catch (MalformedURLException e) {
            e.printStackTrace();  //To change body of catch statement use File | Settings | File Templates.
            return;
        }

        WebDriver driver;
        driver = new RemoteWebDriver(hostURL, capability);
        driver.get("http://www.html5test.com");
//        Result result = JUnitCore.runClasses(ChromeDriverTest.class);
//        for (Failure failure : result.getFailures()) {
//            System.out.println(failure.toString());
//        }
    }
}
