Minumum Requirements
---------------------
* Qt iOS X install: 
Please note that QT library is needed for developing only. If you want just run application on iOs simulator you may skip this step.
Latest WebDriver test application is based on Qt 5.2.0. You may install it by image from http://download.qt-project.org/official_releases/qt/5.2/5.2.0/qt-mac-opensource-5.2.0-ios-x86_64-offline.dmg.


Setup:
------
In command prompt run iOS simulator with WebDriver application (wd.app) e.g.
ios-sim launch ./wd.app 

By default device family is iPhone. In order to run iPad simulator just run ios-sim with --family argument e.g.
ios-sim launch ./wd.app --family ipad 

In order to add argument to WebDriver just use --args. For example, VideoTests require video file placed in WD repository: 
ios-sim launch ./wd.app --args --test_data_folder=/Users/build/git/wd/test_data
