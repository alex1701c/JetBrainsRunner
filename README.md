![JetBrains Toolbox Logo](https://cdn.icon-icons.com/icons2/1381/PNG/64/jetbrainstoolbox_93803.png)
<sub> [Icon from icon-icons.com](https://icon-icons.com/icon/jetbrains-toolbox/93803#64) </sub>

# JetBrainsRunner
This plugin allows you to launch your recent JetBrains projects from Krunner.

<p align="center">

  <img src="https://user-images.githubusercontent.com/51381523/153750644-dac591f8-c17f-4b49-9fe4-c63712b95ec3.png" alt="Example">

</p>

- The search suggestions and the application ⇒ config file mapping can be customized.
- File watchers are implemented, these reload the configuration when new projects get added/removed.
- You can pin your projects by dragging the KRunner results to the taskbar or desktop.

<details>
<summary><b>Screenshots</b></summary>
  
#### Available CLion projects  
![Available CLion projects](https://raw.githubusercontent.com/alex1701c/Screenshots/master/JetBrainsRunner/multible_projects.png)

#### Search for project by name  
![Search for project by name](https://raw.githubusercontent.com/alex1701c/Screenshots/master/JetBrainsRunner/launch_by_name.png)

#### Search projects of app  
![Search for project by name](https://raw.githubusercontent.com/alex1701c/Screenshots/master/JetBrainsRunner/search_projects_of_app.png)
  
#### Config Dialog
![Config Dialog](https://raw.githubusercontent.com/alex1701c/Screenshots/master/JetBrainsRunner/config_dialog.png)
  
</details>

If you like this plugin you can check out [the dolphin version](https://github.com/alex1701c/JetBrainsDolphinPlugin) and leave a rating in the [KDE Store](https://www.pling.com/p/1311630/).

## Installation
### 1 Required Dependencies


<details>
<summary><b>Debian/Ubuntu</b></summary>

Plasma5:  
```bash
sudo apt install git cmake extra-cmake-modules build-essential libkf5runner-dev libkf5i18n-dev libkf5kio-dev libkf5service-dev libkf5kcmutils-dev qtdeclarative5-dev libkf5dbusaddons-bin
```
Plasma6:  
```bash
sudo apt install git cmake extra-cmake-modules build-essential libkf6runner-dev libkf6i18n-dev libkf6kio-dev libkf6service-dev libkf6kcmutils-dev kf6-kdbusaddons
```

</details>

<details>
<summary><b>OpenSUSE</b></summary>

Plasma5:  
```bash
sudo zypper install git cmake extra-cmake-modules ki18n-devel krunner-devel kcmutils-devel kio-devel kservice-devel kdbusaddons-tools
```
Plasma6:  
```bash
sudo zypper install git cmake kf6-extra-cmake-modules kf6-ki18n-devel kf6-krunner-devel kf6-kio-devel kf6-kservice-devel kf6-kdbusaddons-tools
```

</details>

<details>
<summary><b>Fedora</b></summary>

Plasma5:  
```bash
sudo dnf install git cmake extra-cmake-modules kf5-ki18n-devel kf5-krunner-devel kf5-kcmutils-devel kf5-kio-devel kf5-kservice-devel
```
Plasma6:  
```bash
sudo dnf install git cmake extra-cmake-modules kf6-ki18n-devel kf6-krunner-devel kf6-kcmutils-devel kf6-kio-devel kf6-kservice-devel 
```

</details>


### 2 KRunner Plugin
#### 2.1 Get Files

<details>
<summary>Option A: Easy oneliner method</summary>

```
curl https://raw.githubusercontent.com/alex1701c/JetBrainsRunner/master/install.sh | bash
```  
  
</details> 

#### Option B: Get it from the AUR for [Plasma5](https://aur.archlinux.org/packages/plasma5-runners-jetbrains-runner-git/) or [Plasma6](https://aur.archlinux.org/packages/plasma6-runners-jetbrains-runner-git)

<details>
<summary>Option C: Manual method</summary>

```
git clone https://github.com/alex1701c/JetBrainsRunner --recurse-submodules  
cd JetBrainsRunner/
mkdir build  
cd build
cmake -DKDE_INSTALL_QTPLUGINDIR=`kf5-config --qt-plugins` ..
make
make install
kquitapp5 krunner;kstart5 krunner
``` 
  
</details> 

#### 2.2 (Optional) Configure Plugin
1. Go to system settings (`systemsettings5`)
2. Search for `krunner`
3. Search for `JetBrains` in KRunner plugin list
4. Configure plugin

#### Config Dialog
#### Checkbox Options
1. Search By Application: Enables search suggestions based on the Application name (like in the [first](#available-clion-projects) and [third](#search-projects-of-app) screenshot).  
2. Search By Project: Enables suggestions based on the Project name, this is shown in the [second](#search-for-project-by-name) screenshot.  
3. Update Notifications: Enables notifications you when a new version of this plugin has been released. It checks for a new version weekly or when the config is opened. By unchecking it you turn the check permanently off.
4. Show use application name as catagory for projects.

#### Result Formatting
You can change the displayed text for the run options using the following rules.
- *%APPNAME*: application name
- *%APP*: shortened application name
- *%PROJECT*: project name
- *%DIR*: full directory path

#### Manually Adding IDE's
If the plugin fails to find the installed applications or the correct config file you can specify them manually, please open an issue on Github so it can be fixed.
Once the files have been selected the icon, name and recent project should get displayed.

### Issues:  
If you have any problems with this plugin please open an issue on Github and provide some information:  
- Which JetBrains IDE isn't working.
- How it is installed (Snap/Toolbox/Downloaded from website).
- If you have/had other editions (Community, Educational or Ultimate) of this application installed.
- The content of the log file, you can generate this by clicking the "Generate Log File" button in the config dialog, **please read it before uploading**.
