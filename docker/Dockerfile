FROM ubuntu:20.04

ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && \
    apt-get install -y \
    build-essential \
    checkinstall \
    cmake \
    gdb \
    git \
    nodejs \
    npm \
    libmbedtls-dev \
    libasound2-dev \
    libavcodec-dev \
    libavdevice-dev \
    libavfilter-dev \
    libavformat-dev \
    libavutil-dev \
    libcurl4-openssl-dev \
    libfdk-aac-dev \
    libfontconfig-dev \
    libfreetype6-dev \
    libgl1-mesa-dev \
    libjack-jackd2-dev \
    libjansson-dev \
    libluajit-5.1-dev \
    libpulse-dev \
    libqt5x11extras5-dev \
    libspeexdsp-dev \
    libswresample-dev \
    libswscale-dev \
    libudev-dev \
    libv4l-dev \
    libvlc-dev \
    libx11-dev \
    libx264-dev \
    libxcb-shm0-dev \
    libxcb-xinerama0-dev \
    libxcomposite-dev \
    libxinerama-dev \
    pkg-config \
    python3-dev \
    qtbase5-dev \
    libqt5svg5-dev \
    swig \
    libxcb-randr0-dev \
    libxcb-xfixes0-dev \
    libx11-xcb-dev \
    libxcb1-dev \
    x11vnc \
    xserver-xorg-video-dummy \
    xserver-xorg-input-void

RUN apt-get install -y patchelf

COPY xorg.conf /etc/xorg.conf
COPY entrypoint.sh /entrypoint.sh

ENV DISPLAY :99
WORKDIR /obs-node

ENTRYPOINT ["sh", "/entrypoint.sh"]