![JetBrains Toolbox Logo](https://cdn.icon-icons.com/icons2/1381/PNG/64/jetbrainstoolbox_93803.png)
<sub> [Icon from icon-icons.com](https://icon-icons.com/icon/jetbrains-toolbox/93803#64) </sub>

# JetBrainsRunner
This plugin allows you to launch your recent JetBrains projects from Krunner.

- The search suggestions and the application ⇒ config file mapping can be customized.
- File watchers are implemented, these reload the configuration when new projects get added/removed.
- You can pin your projects by dragging the KRunner results to the taskbar or desktop.

<details>
<summary><b>Screenshots</b></summary>
  
#### Available CLion projects  
![Available CLion projects](https://raw.githubusercontent.com/alex1701c/Screenshots/master/JetBrainsRunner/multible_projects.png)

#### Search for project by name  
![ Search for project by name](https://raw.githubusercontent.com/alex1701c/Screenshots/master/JetBrainsRunner/launch_by_name.png)

#### Search projects of app  
![ Search for project by name](https://raw.githubusercontent.com/alex1701c/Screenshots/master/JetBrainsRunner/search_projects_of_app.png)
  
</details>

If you like this plugin you can check out [the dolphin version](https://github.com/alex1701c/JetBrainsDolphinPlugin) and leave a rating in the [KDE Store](https://www.pling.com/p/1311630/).

## Installation
### Required Dependencies

<details>
<summary>Debian/Ubuntu:</summary>
  
```
sudo apt install cmake extra-cmake-modules build-essential libkf5runner-dev libkf5textwidgets-dev qtdeclarative5-dev gettext libnotify-bin libkf5kcmutils-dev libkf5kio-dev
```
  
</details>

<details>
<summary>openSUSE:</summary>
  
```
sudo zypper install cmake extra-cmake-modules libQt5Widgets5 libQt5Core5 libqt5-qtlocation-devel ki18n-devel ktextwidgets-devel 
kservice-devel krunner-devel gettext-tools kconfigwidgets-devel libnotify-tools kcmutils-devel kio-devel
```
  
</details>

<details>
<summary>Fedora:</summary>
  
```
sudo dnf install cmake extra-cmake-modules kf5-ki18n-devel kf5-kservice-devel kf5-krunner-devel kf5-ktextwidgets-devel gettext libnotify kf5-kcmutils-devel kf5-kio-devel
```
  
</details>

<details>
<summary>Archlinux(Manjaro):</summary>

```
sudo pacman -S cmake extra-cmake-modules libnotify kcmutils kio
```
  
</details>

### KRunner Plugin
#### 1. Get Files

<details>
<summary>Easy oneliner method</summary>

```
curl https://raw.githubusercontent.com/alex1701c/JetBrainsRunner/master/install.sh | bash
```  
  
</details> 

<details>
<summary>Manual method (with admin privileges)</summary>

```
git clone https://github.com/alex1701c/JetBrainsRunner --recurse-submodules  
cd JetBrainsRunner/
mkdir build  
cd build
cmake -DKDE_INSTALL_QTPLUGINDIR=`kf5-config --qt-plugins` ..
make
sudo make install
``` 
  
</details> 

#### 2. Restart KRunner
```
kquitapp5 krunner;kstart5 krunner
```

#### 3. (Optional) Configure Plugin
1. Go to system settings (`systemsettings5`)
2. Search for `krunner`
3. Search for `JetBrains` in KRunner plugin list
4. Configure plugin

#### Config Dialog  
The first checkbox enables search suggestions based on the application name (like in the first and third screenshot).  
The second checkbox enables suggestions based on the project name, this is shown in the second screenshot.  
The last checkbox enables notifications you when a new version of this plugin has been released.
It checks for a new version weekly or when the config is opened. By unchecking it you turn the check permanently off.  
After this you can change the displayed text for the run options using the displayed rules.  
If the plugin has issues finding the installed applications or finding the correct config file you can specify them manually.
Once the files have been selected the icon, name and recent project get displayed.  
If you have to do manual you are welcome to create a pull request and add the application path to the default configuration:
https://github.com/alex1701c/JetBrainsRunner/blob/master/src/JetbrainsApplication.cpp#L137
![Config Dialog](https://raw.githubusercontent.com/alex1701c/Screenshots/master/JetBrainsRunner/config_dialog.png)

### Issues:  
If you have any problems with this plugin please open an issue on Github and provide some information:  
- Which JetBrains application is not working
- How is it installed (snap/toolbox/downloaded from website)
- Do you have/had other editions (community, educational etc.) of this application installed
- The content of the log file, you can generate this by clicking the "Generate Log File" button in the config dialog.
*Please check the content of the file before uploading it.*  
