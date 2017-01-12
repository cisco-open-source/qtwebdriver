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
/**
 * This can be used as a template to insert QtWebDriver in an application code
 * QtWebdriver shoud be started in the same process as the QApplication
 */
#include "Headers.h"

void setQtSettings();
void PrintVersion();
void PrintHelp();

int main(int argc, char *argv[])
{
    // Application specific code
    base::AtExitManager exit;
    QApplication app(argc, argv);
    app.setQuitOnLastWindowClosed(false);
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("utf8"));
#endif

    CommandLine cmd_line(CommandLine::NO_PROGRAM);
#if defined(OS_WIN)
    cmd_line.ParseFromString(::GetCommandLineW());
#elif defined(OS_POSIX)
    cmd_line.InitFromArgv(argc, argv);
#endif

    if (cmd_line.HasSwitch("help")) {
        PrintHelp();
        return 0;
    }
    if (cmd_line.HasSwitch("version")) {
      PrintVersion();
      return 0;
    }
    
    // Start webdriver
    int startError = wd_setup(argc, argv);
    if (startError){
        std::cout << "Error while starting server, errorCode " << startError << std::endl;
        return startError;
    }

    setQtSettings();

    return app.exec();
}

void setQtSettings() {
#if (WD_ENABLE_WEB_VIEW == 1)    
    QWebSettings::globalSettings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::JavascriptEnabled, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::JavascriptCanOpenWindows, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::JavascriptCanCloseWindows, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::LocalStorageEnabled, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::LocalContentCanAccessRemoteUrls, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::LocalContentCanAccessFileUrls, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::OfflineStorageDatabaseEnabled, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::OfflineWebApplicationCacheEnabled, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::PluginsEnabled, true);
    QWebSettings::globalSettings()->enablePersistentStorage("./web/html5");
    QWebSettings::globalSettings()->setLocalStoragePath("./web/html5");
    QWebSettings::globalSettings()->setOfflineStoragePath("./web/html5");
    QWebSettings::globalSettings()->setOfflineWebApplicationCachePath("./web/html5");
#endif    
}

void PrintVersion() {
    std::cout <<webdriver::VersionInfo::CreateVersionString()<< std::endl;
}

void PrintHelp() {
    std::cout << "Usage: WebDriver [--OPTION=VALUE]..."                                             << std::endl

                << "Starts QtWebDriver server"                                                        << std::endl
                << ""                                                                                 << std::endl
                << "OPTION         DEFAULT VALUE      DESCRIPTION"                                    << std::endl
                << "http-threads   4                  The number of threads to use for handling"      << std::endl
                << "                                  HTTP requests"                                  << std::endl
                << "log-path       ./webdriver.log    The path to use for the QtWebDriver server"     << std::endl
                << "                                  log"                                            << std::endl
                << "root           ./web              The path of location to serve files from"       << std::endl
                << "port           9517               The port that QtWebDriver listens on"           << std::endl
                << "silence        false              If true, QtWebDriver will not log anything"     << std::endl
                << "                                  to stdout/stderr"                               << std::endl
                << "verbose        false              If true, QtWebDriver will log lots of stuff"    << std::endl
                << "                                  to stdout/stderr"                               << std::endl
                << "url-base                          The URL path prefix to use for all incoming"    << std::endl
                << "                                  WebDriver REST requests. A prefix and postfix"  << std::endl
                << "                                  '/' will automatically be appended if not"      << std::endl
                << "                                  present"                                        << std::endl
                << "config                            The path to config file (e.g. config.json) in"  << std::endl
                << "                                  JSON format with specified WD parameters as"    << std::endl
                << "                                  described above (port, root, etc.)"             << std::endl
                << "wi-server      false              If true, web inspector will be enabled"         << std::endl
                << "wi-port        9222               Web inspector listening port"                   << std::endl
                << "version                           Print version information to stdout and exit"   << std::endl
                << "vnc-login      127.0.0.1:5900     VNC server login parameters"                    << std::endl
                << "                                  format: login:password@ip:port"                 << std::endl
                << "uinput         false              If option set, user input device"               << std::endl
                << "                                  will be registered in the system"               << std::endl
                << "test_data      ./                 Specifies where to look for test specific data" << std::endl
                << "whitelist                         The path to whitelist file (e.g. whitelist.xml)"<< std::endl
                << "                                  in XML format with specified list of IP with"   << std::endl
                << "                                  allowed/denied commands for each of them."      << std::endl
                << "webserver-cfg                     The path to mongoose config file"               << std::endl
                << "                                  (e.g. /path/to/config.json) in JSON format with"<< std::endl
                << "                                  specified mongoose start option"                << std::endl
                << "                                  (extra-mime-types, listening_ports, etc.)"      << std::endl
                << "                                  Option from webserver config file will have"    << std::endl
                << "                                  more priority than commandline param"           << std::endl
                << "                                  that specify the same option."                  << std::endl;
}
