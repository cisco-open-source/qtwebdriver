import os
import sys
import shutil
import string
import xml.etree.ElementTree as ET



########################################
def copyQtSceleton(qt_dir, dest_dir, target_id):
  qt_sceleton_dir = os.path.join(qt_dir, "src/android/java/")
  shutil.copytree(qt_sceleton_dir, dest_dir)

  #change java files for support current target version
  java_to_edit = os.path.join(dest_dir, "src/org/qtproject/qt5/android/bindings/QtActivity.java")
  with open(java_to_edit, 'r') as file:
    data = file.readlines()

    comment_block = False
    need_to_comment = False

    for i in range(len(data)):

      trimmed = data[i].strip()

      if "@ANDROID-" in trimmed:
        version = int(string.split(trimmed, '-')[1])
        targetVersion = int (string.split(target_id, '-')[1])
        comment_block = not comment_block
        if version > targetVersion:
          need_to_comment = True
        else:
          need_to_comment = False

      if not comment_block:
        continue

      if need_to_comment:
        data[i] = "// COMMENTED "+data[i]

    # and write everything back
    with open(java_to_edit, 'w') as file:
      file.writelines(data)



########################################
def copyPredefined(predef_dir, dest_dir):

  android_mainifest = os.path.join(predef_dir, "AndroidManifest.xml")

  libs      = os.path.join(predef_dir ,"libs.xml")
  libs_dest = os.path.join(dest_dir, "res/values/")

  strings       = os.path.join(predef_dir, "strings.xml")
  strings_dest  = os.path.join(dest_dir, "res/values/")

  keystore = os.path.join(predef_dir, "hybrid_test.keystore")
  ant_prop = os.path.join(predef_dir, "ant.properties")

  shutil.copy(android_mainifest, dest_dir)
  shutil.copy(libs, libs_dest)
  shutil.copy(strings, strings_dest)
  shutil.copy(keystore, dest_dir)
  shutil.copy(ant_prop, dest_dir)



########################################
def copyNecessaryLib(qt_dir, libs_dir, libs_xml):
  qt_libs_path = os.path.join(qt_dir, "lib")
  os.makedirs(libs_dir)

  tree = ET.parse(libs_xml)
  root = tree.getroot()

  qt_libs = root.findall("./array[@name=\'qt_libs\']/item")

  # shutil.copytree(dest_dir +'/libs/', predef_dir+'/libs/arm')

  for lib in qt_libs:
    if lib.text == "gnustl_shared" :
      shutil.copy(os.path.join(android_ndk_root, 'sources/cxx-stl/gnu-libstdc++/4.7/libs', arch, 'libgnustl_shared.so'), libs_dir)
      continue
    lib_src_path = os.path.join(qt_libs_path, "lib"+lib.text+".so")
    shutil.copy(lib_src_path, libs_dir)

  bundled_libs = root.findall("./array[@name=\'bundled_in_lib\']/item")

  for lib in bundled_libs:
    if lib.text == "libgnustl_shared.so" :
      continue
    bundled_libs_path = os.path.join(qt_dir, string.split(lib.text, ':')[1]);
    shutil.copy(bundled_libs_path, os.path.join(libs_dir, string.split(lib.text, ':')[0]))

  shutil.copy(os.path.join(qt_dir, 'jar/QtAndroid-bundled.jar'), os.path.join(dest_dir, 'libs/QtAndroid-bundled.jar'))



########################################
def buildApk(mode, android_sdk_root, target_id, dest_dir, build_out_dir):
  build_update = android_sdk_root+"/tools/android update project -n AndroidWD -t \""+target_id+"\" -p "+dest_dir
  os.system(build_update)
  if mode == 'release_dbg':
    os.system("ant debug -buildfile "+dest_dir+"/build.xml")
    shutil.copy(os.path.join(dest_dir, 'bin/AndroidWD-debug-unaligned.apk'), os.path.join(build_out_dir, 'AndroidWD_test.apk'))
  elif mode == 'release':
    os.system("ant release -buildfile "+dest_dir+"/build.xml")
    shutil.copy(os.path.join(dest_dir,'bin/AndroidWD-release.apk'), os.path.join(build_out_dir, 'AndroidWD_test.apk'))


########################################
if __name__ == "__main__":

  qt_dir            = os.environ['QT_ROOT']
  android_sdk_root  = os.environ['ANDROID_SDK_ROOT']
  android_ndk_root  = os.environ['ANDROID_NDK_ROOT']
  arch              = os.environ["ANDROID_LIB_ARCH"]
  target_id         = os.environ["ANDROID_TARGET"]

  build_out_dir = sys.argv[1]
  mode          = sys.argv[2]
  dest_dir      = os.path.join(build_out_dir,'android')
  predef_dir    = "./platform/android"
  libWD         = 'libWebDriver_noWebkit_android.so'
  libs_xml      = os.path.join(dest_dir, 'res/values/libs.xml')
  libs_dir      = os.path.join(dest_dir ,'libs',arch)
  libWD_path    = os.path.join(build_out_dir,libWD)

#clear directory
  shutil.rmtree(dest_dir, ignore_errors=True)

  copyQtSceleton(qt_dir, dest_dir, target_id)

  copyPredefined(predef_dir, dest_dir)

  copyNecessaryLib(qt_dir, libs_dir, libs_xml)

  shutil.copy(libWD_path, libs_dir)

  buildApk(mode, android_sdk_root, target_id, dest_dir, build_out_dir)