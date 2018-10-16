FROM ubuntu:bionic

ENV DEBIAN_FRONTEND noninteractive

WORKDIR /app

ADD . /app

RUN apt-get update

RUN apt-get -y --no-install-recommends install build-essential automake ncurses-dev cmake libcmocka0 git \
    pkg-config autoconf libterm-ui-perl libasound2-dev libalsaplayer-dev openssl libssl-dev apt-utils xterm

RUN apt-get -y install curl ncurses-dev libsctp-dev libpcap-dev ca-certificates sip-tester

RUN mkdir /build

RUN cd /build && git clone -b 2.7.x --depth 1 https://github.com/pjsip/pjproject.git

RUN cd /build/pjproject && ./configure --prefix=/usr && make dep && make && make install

RUN ldconfig

RUN make clean && make && make test && make install

RUN rm -rf /build