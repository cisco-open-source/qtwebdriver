import os

filename = os.environ['ANDROID_JSON_CONFIG']

jsonfile = open (filename, 'w')
jsonfile.write("{\n")
jsonfile.write("\"qt\": \"" +os.environ['QT_ROOT'] +"\",\n")
jsonfile.write("\"sdk\": \"" +os.environ['ANDROID_SDK_ROOT'] +"\",\n")
jsonfile.write("\"ndk\": \"" +os.environ['ANDROID_NDK_ROOT'] +"\",\n")
jsonfile.write("\"toolchain-prefix\": \"" +os.environ['ANDROID_TOOLCHAIN_PREFIX'] +"\",\n")
jsonfile.write("\"tool-prefix\": \"" +os.environ['ANDROID_TOOL_PREFIX'] +"\",\n")
jsonfile.write("\"toolchain-version\": \"" +os.environ['ANDROID_TOOLCHAIN_VERSION'] +"\",\n")
jsonfile.write("\"ndk-host\": \"" +os.environ['ANDROID_NDK_HOST'] +"\",\n")
jsonfile.write("\"target-architecture\": \"" +os.environ['ANDROID_LIB_ARCH'] +"\",\n")
jsonfile.write("\"application-binary\": \"" +os.environ['BINARY_PATH'] +"\",\n")
jsonfile.write("\"android-package\": \"" +os.environ['ANDROID_PACKAGE'] +"\",\n")
# jsonfile.write("\"android-package-source-directory\": \"" +os.environ['ANDROID_JAVA'] +"\",\n")
jsonfile.write("\"android-app-name\": \"" +os.environ['ANDROID_APP_NAME'] +"\"\n")
jsonfile.write("}")
jsonfile.close()

