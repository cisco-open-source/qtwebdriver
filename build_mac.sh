
python generate_wdversion.py

gyp --depth . -G output_dir=. -D platform=desktop -D mode=release --generator-output=out/ wd.gyp

if [ "$1" == "-all" ]
then

xcodebuild -project ./out/wd.xcodeproj -alltargets #clean build

fi

