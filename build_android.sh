#!/usr/bin/env bash


arch=$1

if [ -z $arch ];
then
  arch="arm"
else
  arch="x86"
fi

export ANDROID_NDK_ROOT=/opt/android/android-ndk-r8e
export ANDROID_SDK_ROOT=opt/android/adt-bundle-linux-x86-20130522/sdk

if [ $arch = "x86" ];
then
  export ANDROID_ARCH=arch-x86
  export ANDROID_LIB_ARCH=x86
  export ANDROID_TOOLCHAIN=x86-4.7
  export ANDROID_TOOL_PREFIX=i686-linux-android
elif [ $arch = "arm" ];
then
  export ANDROID_ARCH=arch-arm
  export ANDROID_LIB_ARCH=armeabi-v7a
  export ANDROID_TOOLCHAIN=arm-linux-androideabi-4.7
  export ANDROID_TOOL_PREFIX=arm-linux-androideabi
else
  echo "We don't support platform " $arch
  exit 1
fi

export PREBUILD=$ANDROID_NDK_ROOT/toolchains/$ANDROID_TOOLCHAIN/prebuilt/linux-x86
export BIN=$PREBUILD/bin

export CXX=$BIN/$ANDROID_TOOL_PREFIX-g++
export CC=$BIN/$ANDROID_TOOL_PREFIX-gcc
export LINK=$BIN/$ANDROID_TOOL_PREFIX-g++
export AR=$BIN/$ANDROID_TOOL_PREFIX-ar


export GYP_DEFINES="OS=android"

platform="android"

./build.sh `pwd`/out $platform $mode
