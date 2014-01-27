Minumum Requirements
---------------------
* Qt Mac OS X install: 
Download QT sources from 
http://download.qt-project.org/official_releases/qt/5.2/5.2.0/single/qt-everywhere-opensource-src-5.2.0.tar.gz
Untar and build sources for native (64bit) architecture
> configure -opensource -confirm-license -nomake examples -nomake tests -release -arch x86_64 -no-framework
> make
> make install
See more details here
http://qt-project.org/doc/qt-5/macosx-building.html

Setup:
------
In command prompt run WebDriver or WebDriver_noWebkit