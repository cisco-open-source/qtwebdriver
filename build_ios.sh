
python generate_wdversion.py

export GYP_DEFINES="OS=ios"
#export GYP_GENERATOR_FLAGS="xcode_project_version=3.2"

gyp --depth . -G output_dir=. -D platform=desktop -D mode=release --generator-output=out/ wd.gyp
