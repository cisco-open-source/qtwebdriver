# Dockerfile to build a Webdriver for QT5
# Check port mapping when running the container
# e.g:
# $ docker run -ti --rm -p 9531:9517 latest:latest --verbose

FROM  alexzaporozhets/ubuntu-qt5.4.2
#FROM icsinc/qt5.5.0-x64
#FROM ubuntu:trusty
MAINTAINER Hugues Ekra <hekra@cisco.com>
LABEL vendor="Cisco Systems"
LABEL license="LGPLv2.1"
LABEL version="1.3.3"

#ENV QTDIR /opt/qt55
ENV QTDIR "/opt/Qt5.4.2/5.4/gcc_64"
ENV QT_PLUGIN_PATH $QTDIR/plugins
ENV QT_QPA_PLATFORM offscreen
ENV QT_QPA_FONTDIR /opt/Qt5.4.2/5.4/Src/qtbase/lib/fonts
WORKDIR /opt

# Build
RUN echo 'debconf debconf/frontend select Noninteractive' | debconf-set-selections \
    && sudo apt-get update && sudo apt-get install -y \
        git g++ gyp xvfb libicu-dev libegl1-mesa-dev libgles2-mesa-dev \
    && git clone --verbose --progress https://github.com/cisco-open-source/qtwebdriver \
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
