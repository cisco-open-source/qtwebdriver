#!/usr/bin/env bash
output_gen=$1
platform=$2
mode=$3

base_output_gen=`dirname ${output_gen}`

if [ -z $platform ];
then
  platforms="desktop flip"
else
  platforms=$platform
fi
if [ -z $mode ];
then
  modes="release_dbg release"
else
  modes=$mode
fi

for platform in $platforms
do
  for mode in $modes
  do
    gyp --depth . -G output_dir=. -D platform=$platform -D mode=$mode -D ROOT_PATH=${base_output_gen} --generator-output=${output_gen}/$platform/$mode wd.gyp
    [ $? -ne 0 ] && exit 1
    cd ${output_gen}/$platform/$mode
    [ $? -ne 0 ] && echo "**** ERROR: Can't access to ${output_gen}/$platform/$mode" && exit 1
    make
    [ $? -ne 0 ] && exit 1
    mkdir -p ${output_gen}/bin/$platform/$mode/
    [ $? -ne 0 ] && echo "**** ERROR: Can't create ${output_gen}/bin/$platform/$mode" && exit 1
    cp -f ${output_gen}/$platform/$mode/Default/libWebDriver.a ${output_gen}/bin/$platform/$mode/ 2>/dev/null
    cp -f ${output_gen}/$platform/$mode/Default/lib.target/libWebDriver.so ${output_gen}/bin/$platform/$mode/ 2>/dev/null
  done
done
