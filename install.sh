#!/bin/bash

# Exit if something fails
set -e

if [[ $(basename "$PWD") !=  "JetBrainsRunner"* ]];then
    git clone https://github.com/alex1701c/JetBrainsRunner --recurse-submodules
    cd JetBrainsRunner/
fi

mkdir -p build
cd build
cmake -DKDE_INSTALL_QTPLUGINDIR=`kf5-config --qt-plugins` -DCMAKE_BUILD_TYPE=Release  ..
make -j$(nproc)
sudo make install

kquitapp5 krunner 2> /dev/null
kstart5 --windowclass krunner krunner > /dev/null 2>&1 &

echo "Installation finished !";
