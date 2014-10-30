# Synopsis
QtWebDriver (QtWD) is a library based on Google ChromeDriver and intended for automated testing. At the momemnt, you can use it to test many kinds of Qt application - QWidget, QwebView and QQuick1 (Qt4) or QQuick2 (Qt5) based applications. If you hadn't used Selenium for automated testing, you may also find this links helpfull:
* http://code.google.com/p/selenium/w/list
* http://docs.seleniumhq.org/
# Code Example
See out test application in src/Test/main.cc

# Build and run
To build WD you need:
* Install Qt (4 or 5)
* Create wd.gypi file, our current wd.gypi looks like: 
```
{
  'variables': {
    'QT5': '0', # change to '1' for enabling Qt5
    'QT_BIN_PATH': '/home/username/Qt5.2.1/5.2.1/gcc/bin/',
    'QT_INC_PATH': '/home/username/Qt5.2.1/5.2.1/gcc/include/',
    'QT_LIB_PATH': '/home/username/Qt5.2.1/5.2.1/gcc/lib/',
  },
}

```
* run *build.sh* for basic build(output is in out folder)
* you can find builded libs and test app in out/bin/desktop/release

# Where to start
Look at our test application in *src/Test/main.cc* and in docs http://cisco-open-source.github.io/qtwebdriver/page_wd_server.html

# License
Classic LGPL License. 

For more info see out Doxygen documentation on http://cisco-open-source.github.io/qtwebdriver/
