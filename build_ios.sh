#!/bin/sh

export QT_DIR=/usr/local/opt/qt5_ios
export PATH=$QT_DIR/bin:$PATH

cat > wd.gypi <<EOF
{
  'variables': 
  {
    'QT5': '1',
    'QT_BIN_PATH':       '${QT_DIR}/bin',
    'QT_INC_PATH':       '${QT_DIR}/include',
    'QT_LIB_PATH':       '${QT_DIR}/lib',
    'WD_CONFIG_WEBKIT':  '0',
    'WD_CONFIG_QUICK':   '1',
    'WD_CONFIG_PLAYER':  '1',
    'WD_BUILD_MONGOOSE': '1',
  },
}
EOF

python generate_wdversion.py

gyp --depth . -G output_dir=. -D platform=desktop -D mode=debug -f xcode -D OS=ios --generator-output=out/ wd.gyp

if [ "$1" == "-all" ]
then

#xcodebuild -project out/wd_test.xcodeproj -target test_ios_WD -arch i386 -sdk iphonesimulator #clean build
xcodebuild -project out/base.xcodeproj -target chromium_base -arch i386 -sdk iphonesimulator6.0 clean build
xcodebuild -project out/wd_core.xcodeproj -target WebDriver_core -arch i386 -sdk iphonesimulator6.0 clean build
xcodebuild -project out/wd_ext_qt.xcodeproj -target WebDriver_extension_qt_base -arch i386 -sdk iphonesimulator6.0 clean build
xcodebuild -project out/wd_ext_qt.xcodeproj -target WebDriver_extension_qt_quick -arch i386 -sdk iphonesimulator6.0 clean build


cd ./platform/ios/wd
qmake wd.pro -r -spec macx-ios-clang CONFIG+=x86 CONFIG+=iphonesimulator CONFIG+=release
make

fi