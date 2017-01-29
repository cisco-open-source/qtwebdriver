# Dockerfile to build a Webdriver for QT5
# Documentation: https://github.com/cisco-open-source/qtwebdriver/wiki/Docker

FROM rabits/qt:5.4-desktop
MAINTAINER Hugues Ekra <hekra@cisco.com>
LABEL vendor="Cisco Systems"
LABEL license="LGPLv2.1"
LABEL version="1.3.3"

#Env
ENV QTDIR $QT_DESKTOP
ENV QT_PLUGIN_PATH $QTDIR/plugins
ENV QT_QPA_PLATFORM offscreen
ENV QT_QPA_FONTDIR $QT_PATH/5.4/Src/qtbase/lib/fonts
ENV LD_LIBRARY_PATH $QTDIR/lib:$LD_LIBRARY_PATH
ENV PATH $PATH:$HOME/qtwebdriver/out/dist/desktop/release/bin

# Build
RUN sudo apt-get update && sudo apt-get install -y \
        zip wget g++ gyp xvfb xorg libicu-dev libegl1-mesa-dev libgles2-mesa-dev \
        libxslt1-dev libgstreamer-plugins-base0.10-dev dbus libdbus-1-dev libxml2-dev dbus-x11\
    && wget http://github.com/cisco-open-source/qtwebdriver/archive/WD_1.X_dev.zip \
    && unzip WD_1.X_dev.zip \
    && rm -f WD_1.X_dev.zip \
    && mv qtwebdriver-WD_1.X_dev qtwebdriver \
    && cd qtwebdriver \
    && cp ./qt5_sample_config.gypi ./wd.gypi \
    && sed -i "s@\/home\/hekra01\/qt@$QTDIR@g" wd.gypi \
    && ./build.sh

# Run
ENTRYPOINT ["WebDriver"]
