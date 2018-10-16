#!/bin/bash

# install cmocka
cd /tmp
git clone git://git.cryptomilk.org/projects/cmocka.git
cd cmocka
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Debug ..
make
sudo make install

# install pjproject
cd /tmp
git clone -b 2.7.x --depth 1 https://github.com/pjsip/pjproject.git
cd pjproject
./configure --prefix=/usr
make dep
make
sudo make install

# install sipp

cd /tmp
git clone https://github.com/SIPp/sipp.git
cd sipp
git checkout -b 3.5.1 v3.5.1
sh build.sh
sudo make install

sudo ldconfig
