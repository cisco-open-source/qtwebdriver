# Dockerfile to build a Webdriver for QT5
# Check port mapping when running the container
# e.g: docker run -p port:port image

FROM  alexzaporozhets/ubuntu-qt5.4.2
MAINTAINER Hugues Ekra <hekra@cisco.com>
LABEL vendor="Cisco Systems"
LABEL license="LGPLv2.1"
LABEL version="1.3.3"

# Qt root dir, adjusted according to the base Qt5 image
ENV QTDIR "/opt/Qt5.4.2/5.4/gcc_64"
ENV QT_PLUGIN_PATH $QTDIR/plugins

# Build
RUN echo 'debconf debconf/frontend select Noninteractive' | debconf-set-selections \
    && sudo apt-get update \
    && sudo apt-get install -y g++ gyp xvfb x11-apps \
    && git clone https://github.com/cisco-open-source/qtwebdriver \
    && cd qtwebdriver \
    && cp ./qt5_sample_config.gypi ./wd.gypi \
    && sed -i "s@\/home\/hekra01\/qt@$QTDIR@g" wd.gypi \
    && ./build.sh

# hardcoded path, wait for fix https://github.com/docker/docker/issues/29110
# ENV WD_PATH "$dirname(find "$PWD" -name WebDriver -executable)"
ENV WD_PATH /opt/qtwebdriver/out/dist/desktop/release/bin
ENV PATH $PATH:$WD_PATH

# Run
ENTRYPOINT ["/bin/sh", "-c", "xvfb-run -a WebDriver --verbose"]
#CMD ["--verbose"]

# http://fabiorehm.com/blog/2014/09/11/running-gui-apps-with-docker/
