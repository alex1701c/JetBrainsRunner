#!/bin/bash

# Exit if something fails
set -e

# Delete the old install if it exists
INSTALL_DIR=${HOME}/JetBrainsRunner
if [ -d "$INSTALL_DIR" ]; then
    rm -rf "$INSTALL_DIR"
    echo "The old ${INSTALL_DIR} directory is removed."
fi

if [[ $(basename "$PWD") !=  "JetBrainsRunner"* ]];then
    git clone https://github.com/alex1701c/JetBrainsRunner --recurse-submodules
    cd JetBrainsRunner/
fi

mkdir -p build
cd build
cmake -DCMAKE_INSTALL_PREFIX=/usr -DKDE_INSTALL_QTPLUGINDIR=`kf5-config --qt-plugins` -DCMAKE_BUILD_TYPE=Release  ..
make -j$(nproc)

sudo make install

# KRunner needs to be restarted for the changes to be applied
if pgrep -x krunner > /dev/null
then
    kquitapp5 krunner
fi

# If KRunner does not get started using the shortcut we have to autostart it
krunner_version=$(krunner --version | cut -d " " -f2)
major_version=$(echo $krunner_version | cut -d "." -f -1)
minor_version=$(echo $krunner_version | cut -d "." -f2)
if [[ (("$major_version" < "5")) || (("$minor_version" < "17")) ]]
then
    kstart5 krunner --windowclass krunner
fi

echo "Installation finished !";
