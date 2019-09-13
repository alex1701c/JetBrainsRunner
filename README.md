Required Dependencies
----------------------

Debian/Ubuntu:  
`sudo apt install cmake extra-cmake-modules build-essential libkf5runner-dev libkf5textwidgets-dev qtdeclarative5-dev gettext`

openSUSE  
`sudo zypper install cmake extra-cmake-modules libQt5Widgets5 libQt5Core5 libqt5-qtlocation-devel ki18n-devel ktextwidgets-devel kservice-devel krunner-devel gettext-tools`

Fedora  
`sudo dnf install cmake extra-cmake-modules kf5-ki18n-devel kf5-kservice-devel kf5-krunner-devel kf5-ktextwidgets-devel gettext`

The easiest way to install is:  
`curl https://raw.githubusercontent.com/alex1701c/JetBrainsRunner/master/install.sh | bash`

Or you can do it manually:

```
git clone https://github.com/alex1701c/JetBrainsRunner  
cd JetBrainsRunner/
mkdir build  
cd build
cmake -DQT_PLUGIN_INSTALL_DIR=`kf5-config --qt-plugins` ..
make
sudo make install
```

Restart krunner to load the runner (in a terminal type: kquitapp5 krunner;kstart5 krunner )

After this you should see your runner in the system settings:

systemsettings5 (Head to "Search")

You can also launch KRunner via Alt-F2 and you will find your runner.  
Icon downloaded from https://icon-icons.com/icon/jetbrains-toolbox/93803#64

### Screenshots:
#### Available CLion projects  
![Available CLion projects](https://raw.githubusercontent.com/alex1701c/Screenshots/master/JetBrainsRunner/multible_projects.png)

#### Search for project by name  
![ Search for project by name](https://raw.githubusercontent.com/alex1701c/Screenshots/master/JetBrainsRunner/launch_by_name.png)

#### Search projects of app  
![ Search for project by name](https://raw.githubusercontent.com/alex1701c/Screenshots/master/JetBrainsRunner/search_projects_of_app.png)

### Issues:  
If you have any problems with this plugin please open an issue on Github and provide some information:  
- Which JetBrains application is not working
- How is it installed (snap/toolbox/downloaded from website)
- Do you have/had other editions (community, educational etc.) of this application installed
- The debug output of this plugin. In order to get it you have to uncomment the `add_compile_definitions(LOG_INSTALLED` line in the CMakeLists.txt file.  
Then you have to reinstall the plugin and restart it using `kquitapp5 krunner;kstart5 krunner;qdbus org.kde.krunner /App org.kde.krunner.App.query "test"`.  
If the add_compile_definitions command is not found you can uncomment the definitions in the jetbrainsrunner and JetbrainsApplication header files. 