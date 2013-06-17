@ECHO OFF

python generate_wdversion.py

gyp --depth . -G output_dir=. -D platform=desktop -D mode=release --generator-output=out/ wd.gyp
