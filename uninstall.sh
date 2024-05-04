#!/bin/bash

# Exit if something fails
set -e

cd build
sudo make uninstall

# KRunner needs to be restarted for the changes to be applied
if pgrep -x krunner > /dev/null
then
    kquitapp5 krunner
fi

