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
    gyp --depth . -G output_dir=. -D platform=$platform -D mode=$mode --generator-output=out/$platform/$mode wd.gyp
    [ $? -ne 0 ] && exit 1
    cd out/$platform/$mode
    make
    [ $? -ne 0 ] && exit 1
    cd ../../..
    mkdir -p bin/$platform/$mode/
    cp -f out/$platform/$mode/Default/libWebDriver.a bin/$platform/$mode/
    cp -f out/$platform/$mode/Default/lib.target/libWebDriver.so bin/$platform/$mode/ 2>/dev/null
    if [ -f out/$platform/$mode/Default/WebDriver ]
    then
      cp -f out/$platform/$mode/Default/WebDriver bin/$platform/$mode/
    fi
  done
done
