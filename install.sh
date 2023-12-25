#!/bin/bash

# Exit if something fails
set -e

if [ -d "$PWD/JetBrainsRunner" ]; then
    # Update existing install
    cd JetBrainsRunner/
    git pull --recurse-submodules -f
    echo "Files have been updated."
elif [[ $(basename "$PWD") !=  "JetBrainsRunner"* ]]; then
    git clone https://github.com/alex1701c/JetBrainsRunner --recurse-submodules
    cd JetBrainsRunner/
fi

mkdir -p build
cd build
cmake -DCMAKE_INSTALL_PREFIX=/usr -DKDE_INSTALL_USE_QT_SYS_PATHS=ON -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=OFF ..
make -j$(nproc)

sudo make install

# KRunner needs to be restarted for the changes to be applied
if pgrep -x krunner > /dev/null
then
    kquitapp5 krunner
fi

echo "Installation finished !";
