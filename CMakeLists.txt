cmake_minimum_required(VERSION 3.16)

# Set project variables
project(jetbrainsrunner)
set(CMAKE_PROJECT_VERSION  "1.9.4")
set(CMAKE_CXX_STANDARD 17)
add_definitions(-DCMAKE_PROJECT_VERSION="${CMAKE_PROJECT_VERSION}")
set(CMAKE_PROJECT_HOMEPAGE_URL "https://github.com/alex1701c/JetBrainsRunner")
set(CMAKE_PROJECT_AUTHOR "alex1701c")
set(CMAKE_PROJECT_AUTHOR_EMAIL "alex1701c.dev@gmx.net")

set(KF_MIN_VERSION "5.91.0")
find_package(ECM ${KF_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${CMAKE_MODULE_PATH})

# Find the required Libaries
find_package(Qt5 ${QT_MIN_VERSION} REQUIRED CONFIG COMPONENTS Widgets Core Quick QuickWidgets Xml Network)
find_package(KF5 ${KF_MIN_VERSION} REQUIRED COMPONENTS I18n Service Runner TextWidgets ConfigWidgets PlasmaQuick KIO)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(FeatureSummary)
include(ECMDeprecationSettings)

ecm_set_disabled_deprecation_versions(
	KF ${KF_MIN_VERSION}
)


add_subdirectory(src)
if (BUILD_TESTING)
	add_subdirectory(autotests)
endif()

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)

# CPack configuration
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A Krunner Plugin which allows you to open your recent projects")
set(CPACK_PACKAGE_NAME krunner-${CMAKE_PROJECT_NAME})
set(CPACK_PACKAGE_VERSION "${CMAKE_PROJECT_VERSION}")
set(CPACK_DEBIAN_PACKAGE_SECTION "utils")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "${CMAKE_PROJECT_AUTHOR}")
set(CPACK_RPM_PACKAGE_ARCHITECTURE "x86_64")
set(CPACK_GENERATOR "DEB;RPM")

# Then names of dependencies and paths might differ
if (CPACK_DEBIAN) # -DCPACK_DEBIAN=true -DKDE_INSTALL_QTPLUGINDIR=/usr/lib/x86_64-linux-gnu/qt5/plugins
	set(CPACK_DEBIAN_PACKAGE_DEPENDS "libkf5runner5 (>=${KF5Runner_VERSION})")
	set(CPACK_DEBIAN_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-Debian.deb")
	set(CPACK_GENERATOR "DEB")
elseif(CPACK_FEDORA) # -DCPACK_FEDORA=true -DKDE_INSTALL_QTPLUGINDIR=/usr/lib64/qt5/plugins
	set(CPACK_RPM_PACKAGE_REQUIRES "kf5-krunner >= ${KF5Runner_VERSION}")
	set(CPACK_RPM_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-Fedora.rpm")
	set(CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION "/usr/lib64/qt5;/usr/lib64/qt5/plugins;/usr/lib64/qt5/plugins/kf5;/usr/lib64/qt5/plugins/kf5/krunner;/usr/share/kservices5;/usr/share/icons;/usr/share/pixmaps")
	set(CPACK_GENERATOR "RPM")
elseif(CPACK_OPENSUSE) # -DCPACK_OPENSUSE=true -DKDE_INSTALL_QTPLUGINDIR=/usr/lib64/qt5/plugins
	set(CPACK_RPM_PACKAGE_REQUIRES "libKF5Runner5 >= ${KF5Runner_VERSION}")
	set(CPACK_RPM_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-OpenSUSE.rpm")
	set(CPACK_GENERATOR "RPM")
endif()
# We always have to explicitely set the generator when using cpack, otherwise
# the different paths of the distros might get messed up
unset(CPACK_DEBIAN CACHE)
unset(CPACK_FEDORA CACHE)
unset(CPACK_OPENSUSE CACHE)

# These scripts will kill krunner and when it is invoked next time the plugins will be loaded
set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${CMAKE_CURRENT_SOURCE_DIR}/postinst;${CMAKE_CURRENT_SOURCE_DIR}/postrm")
set(CPACK_RPM_POST_INSTALL_SCRIPT_FILE "${CMAKE_CURRENT_SOURCE_DIR}/postinst")
set(CPACK_RPM_POST_UNINSTALL_SCRIPT_FILE "${CMAKE_CURRENT_SOURCE_DIR}/postrm")

INCLUDE(CPack)

