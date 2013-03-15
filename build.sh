#!/usr/bin/env bash
platform=$1
mode=$2

if [ -z $platform ];
then
  platforms="desktop flip"
else
  platforms=$platform
fi
if [ -z $mode ];
then
  modes="debug release"
else
  modes=$mode
fi

for platform in $platforms
do
  for mode in $modes
  do
    gyp --depth . -D library=static_library -G output_dir=. -D platform=$platform -D mode=$mode --generator-output=out/$platform/$mode wd.gyp
    cd out/$platform/$mode
    make
    cd ../../..
    mkdir -p bin/$platform/$mode/
    cp -f out/$platform/$mode/Default/libWebDriver.a bin/$platform/$mode/
    cp -f out/$platform/$mode/Default/WebDriverTest bin/$platform/$mode/
  done
done
