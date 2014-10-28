# Synopsis
QtWebDriver(QtWD) is library based on Google ChromeDriver and intended for automatic testing. At the momemnt, you can use it to test many kinds of Qt application - QWidget, QwebView and QQuick1(Qt4) or QQuick2(Qt5) based applications. If you hadn't used selenium for automatic testing, you may also find this links helpfull:
http://code.google.com/p/selenium/w/list
http://docs.seleniumhq.org/
# Code Example
See out test application in src/Test/main.cc

# Build and run
To build WD you need:
* Install Qt (4 or 5)
* Create wd.gypi file, our current wd.gypi looks like: ```
{
  'variables': {
    'QT5': '0', # change to '1' for enabling Qt5
    'QT_BIN_PATH': '/home/username/Qt5.2.1/5.2.1/gcc/bin/',
    'QT_INC_PATH': '/home/username/Qt5.2.1/5.2.1/gcc/include/',
    'QT_LIB_PATH': '/home/username/Qt5.2.1/5.2.1/gcc/lib/',
  },
}

```
# API Reference
Depending on the size of the project, if it is small and simple enough the reference docs can be added to the README. For medium size to larger projects it is important to at least provide a link to where the API reference docs live.
# Tests
Describe and show how to run the tests with code examples.
# Contributors
Let people know how they can dive into the project, include important links to things like issue trackers, irc, twitter accounts if applicable.
# License
Classic LGPL License. This means that you can’t ship this your product with QtWD unlees it’s an LGPL product, but you can free test it in your local enviroment and just doesn’t ship 

