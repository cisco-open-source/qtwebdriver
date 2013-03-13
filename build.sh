#!/usr/bin/env bash
gyp --depth . -D library=static_library wd.gyp
make
