#!/usr/bin/env bash


archs=$1

if [ -z $archs ];
then
  archs="armv7 x86"
  modes="release release_dbg"
fi

export ANDROID_NDK_ROOT=/opt/android/android-ndk-r8e
export ANDROID_SDK_ROOT=/opt/android/adt-bundle-linux-x86-20130522/sdk
export QT_DIR=/opt/Qt5.1/5.1.0

for arch in $archs
do
  if [ $arch = "x86" ];
  then
    export ANDROID_ARCH=arch-x86
    export ANDROID_LIB_ARCH=x86
    export ANDROID_TOOLCHAIN=x86-4.7
    export ANDROID_TOOL_PREFIX=i686-linux-android
    export ANDROID_TARGET=android-10
  elif [ $arch = "armv7" ];
  then
    export ANDROID_ARCH=arch-arm
    export ANDROID_LIB_ARCH=armeabi-v7a
    export ANDROID_TOOLCHAIN=arm-linux-androideabi-4.7
    export ANDROID_TOOL_PREFIX=arm-linux-androideabi
    export ANDROID_TARGET=android-10
  else
    echo "We don't support platform " $arch
    exit 1
  fi

  export QT_ROOT=$QT_DIR/android_$arch

  export PREBUILD=$ANDROID_NDK_ROOT/toolchains/$ANDROID_TOOLCHAIN/prebuilt/linux-x86
  export BIN=$PREBUILD/bin

  export CXX=$BIN/$ANDROID_TOOL_PREFIX-g++
  export CC=$BIN/$ANDROID_TOOL_PREFIX-gcc
  export LINK=$BIN/$ANDROID_TOOL_PREFIX-g++
  export AR=$BIN/$ANDROID_TOOL_PREFIX-ar


  export GYP_DEFINES="OS=android"

if [[ -z $modes ]];
then
  modes="release"
fi

  platform="android_"$arch

  for mode in $modes
  do 
    echo "####################### Build "$arch $mode" #######################"

    ./build.sh `pwd`/out $platform $mode

    RETVAL=$?
    if [ $RETVAL -ne 0 ];
    then
      echo "####################### Build "$arch" failed !!! #######################"
      exit $RETVAL
    fi
    
    dist_dir=`pwd`/out/bin/$platform/$mode
    echo "####################### Create apk "$arch" "$mode" #######################"
    python generate_apk.py $dist_dir $mode
  done

done
