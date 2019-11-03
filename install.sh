#!/bin/bash

# Exit if something fails
set -e

if [[ $(basename "$PWD") !=  "JetBrainsRunner"* ]];then
    git clone https://github.com/alex1701c/JetBrainsRunner
    cd JetBrainsRunner/
fi

mkdir -p build
cd build
cmake -DQT_PLUGIN_INSTALL_DIR=`kf5-config --qt-plugins` -DICON_INSTALL_DIR="/usr/share/icons/"  ..
make -j$(nproc)
sudo make install

kquitapp5 krunner 2> /dev/null
kstart5 --windowclass krunner krunner > /dev/null 2>&1 &

echo "Installation finished !";
