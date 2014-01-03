#!/bin/sh

export QT_DIR=/usr/local/opt/qt5_ios
export PATH=$QT_DIR/bin:$PATH

python generate_wdversion.py

gyp --depth . -G output_dir=. -D platform=desktop -D mode=debug -f xcode -D OS=ios --generator-output=out/ wd.gyp

if [ "$1" == "-all" ]
then

xcodebuild -project out/wd_test.xcodeproj -target test_ios_WD -arch i386 -sdk iphonesimulator clean build

cd ./platform/ios/wd
qmake wd.pro -r -spec unsupported/macx-iossimulator-clang CONFIG+=x86
make -release

cd ../wd_test

xcodebuild -project wd_test.xcodeproj -alltargets -arch i386 -sdk iphonesimulator -configuration Debug ONLY_ACTIVE_ARCH=NO VALID_ARCHS="armv7 i386" clean build

fi