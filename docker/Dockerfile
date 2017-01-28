# Dockerfile to build a Webdriver for QT5
# Documentation: https://github.com/cisco-open-source/qtwebdriver/wiki/Docker

FROM  alexzaporozhets/ubuntu-qt5.4.2
MAINTAINER Hugues Ekra <hekra@cisco.com>
LABEL vendor="Cisco Systems"
LABEL license="LGPLv2.1"
LABEL version="1.3.3"

#Env
ENV QTDIR "/opt/Qt5.4.2/5.4/gcc_64"
ENV QT_PLUGIN_PATH $QTDIR/plugins
ENV QT_QPA_PLATFORM offscreen
ENV QT_QPA_FONTDIR /opt/Qt5.4.2/5.4/Src/qtbase/lib/fonts
WORKDIR /opt

# Build
RUN echo 'debconf debconf/frontend select Noninteractive' | debconf-set-selections \
    && sudo apt-get update && sudo apt-get install -y \
        g++ gyp xvfb xorg libicu-dev libegl1-mesa-dev libgles2-mesa-dev \
    && wget https://github.com/cisco-open-source/qtwebdriver/archive/WD_1.X_dev.zip \
    && unzip WD_1.X_dev.zip \
    && mv qtwebdriver-WD_1.X_dev qtwebdriver \
    && cd qtwebdriver \
    && cp ./qt5_sample_config.gypi ./wd.gypi \
    && sed -i "s@\/home\/hekra01\/qt@$QTDIR@g" wd.gypi \
    && ./build.sh

# hardcoded path, wait for fix https://github.com/docker/docker/issues/29110
ENV WD_PATH /opt/qtwebdriver/out/dist/desktop/release/bin
ENV PATH $PATH:$WD_PATH

# Run
#ENTRYPOINT ["/bin/sh", "-c", "xvfb-run -a WebDriver"]
ENTRYPOINT ["WebDriver"]
