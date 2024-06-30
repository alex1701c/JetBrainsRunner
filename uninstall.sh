#!/bin/bash

# Exit if something fails
set -e

krunner_version=$(krunner --version | grep -oP "(?<=krunner )\d+")
cd build
sudo make uninstall

# KRunner needs to be restarted for the changes to be applied
if pgrep -x krunner > /dev/null
then
    kquitapp$krunner_version krunner
fi

