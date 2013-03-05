/*
Copyright 2012 Software Freedom Conservancy
Copyright 2007-2012 WebDriver committers
Copyright 2007-2009 Google Inc.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
 */

package org.openqa.selenium.environment.webserver;

import org.openqa.selenium.net.NetworkUtils;

public class DummyAppServer implements AppServer {

    private static final String DEFAULT_CONTEXT_PATH = "";
    private static final String ALTERNATIVE_HOSTNAME_FOR_TEST_ENV_NAME = "ALTERNATIVE_HOSTNAME";

    private final String hostName = "127.0.0.1";
    private int port = 9517;

    private static final NetworkUtils networkUtils = new NetworkUtils();

    private String getCommonPath(String relativeUrl) {
        if (!relativeUrl.startsWith("/")) {
            relativeUrl = DEFAULT_CONTEXT_PATH + "/" + relativeUrl;
        }
        return relativeUrl;
    }

    @Override
    public String getHostName() {
        return hostName;
    }

    @Override
    public String getAlternateHostName() {
        String alternativeHostnameFromProperty = System.getenv(ALTERNATIVE_HOSTNAME_FOR_TEST_ENV_NAME);
        return alternativeHostnameFromProperty == null ?
                "127.0.0.1" : alternativeHostnameFromProperty;
//                networkUtils.getPrivateLocalAddress() : alternativeHostnameFromProperty;
    }

    public String whereIs(String relativeUrl) {
        relativeUrl = getCommonPath(relativeUrl);
        return "http://" + getHostName() + ":" + port + relativeUrl;
    }

    @Override
    public String whereElseIs(String relativeUrl) {
        relativeUrl = getCommonPath(relativeUrl);
        return "http://" + getAlternateHostName() + ":" + port + relativeUrl;
    }

    @Override
    public String whereIsSecure(String relativeUrl) {
        return null;  //To change body of implemented methods use File | Settings | File Templates.
    }

    @Override
    public String whereIsWithCredentials(String relativeUrl, String user, String password) {
        return null;  //To change body of implemented methods use File | Settings | File Templates.
    }

    @Override
    public void start() {
        //To change body of implemented methods use File | Settings | File Templates.
    }

    @Override
    public void stop() {
        //To change body of implemented methods use File | Settings | File Templates.
    }

    @Override
    public void listenOn(int port) {
        //To change body of implemented methods use File | Settings | File Templates.
    }

    @Override
    public void listenSecurelyOn(int port) {
        //To change body of implemented methods use File | Settings | File Templates.
    }


}
