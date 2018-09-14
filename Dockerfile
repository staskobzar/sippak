FROM ubuntu:bionic

ENV DEBIAN_FRONTEND noninteractive

WORKDIR /app

ADD . /app

RUN apt-get update && apt-get upgrade -y && apt-get install -y xterm

RUN apt-get -y --no-install-recommends install build-essential automake ncurses-dev cmake libcmocka0 git \
    pkg-config autoconf libterm-ui-perl libasound2-dev libalsaplayer-dev openssl libssl-dev

RUN apt-get -y install curl ncurses-dev libsctp-dev libpcap-dev

RUN mkdir /build && cd /build && \
    git clone https://github.com/SIPp/sipp.git && cd sipp && git checkout -b v3.5.2 v3.5.2

RUN cd /build/sipp && ./build.sh && make install

RUN cd /build && git clone -b 2.7.x --depth 1 https://github.com/pjsip/pjproject.git

RUN cd /build/pjproject && ./configure --prefix=/usr && make dep && make && make install

RUN make clean && make && make test && make install

