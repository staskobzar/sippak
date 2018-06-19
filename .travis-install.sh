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

