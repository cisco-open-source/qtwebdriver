
python generate_wdversion.py

#export GYP_DEFINES="OS=ios"
#export GYP_GENERATOR_FLAGS="xcode_project_version=3.2"

#export GYP_DEFINES="SDK_ROOT=/Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator5.0.sdk"

gyp --depth . -G output_dir=. -D platform=desktop -D mode=debug -f xcode -D OS=ios --generator-output=out/ wd.gyp
#gyp --depth . -G output_dir=. -D platform=desktop -D mode=release -D OS=linux --generator-output=out/ wd.gyp
#xcodebuild -project out/wd.xcodeproj -arch i386 -sdk iphonesimulator