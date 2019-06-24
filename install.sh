#!/bin/bash

# TODO Enable if the project is on Github
if [[ $(basename "$PWD") != "JetbrainsRunner" ]];then
    #git clone https://github.com/%{USERNAME}/JetbrainsRunner
    #cd JetbrainsRunner/
    echo "Please go to the project folder"
    exit
fi

mkdir -p build
cd build
cmake -DQT_PLUGIN_INSTALL_DIR=`kf5-config --qt-plugins` ..
make -j2
sudo make install

kquitapp5 krunner 2> /dev/null
kstart5 --windowclass krunner krunner > /dev/null 2>&1 &

echo "Installation finished !";
