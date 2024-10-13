cmake_minimum_required(VERSION 3.16)

# Set project variables
project(jetbrainsrunner)
set(CMAKE_PROJECT_VERSION  "2.1.0")
set(CMAKE_CXX_STANDARD 17)
add_definitions(-DCMAKE_PROJECT_VERSION="${CMAKE_PROJECT_VERSION}")
set(CMAKE_PROJECT_HOMEPAGE_URL "https://github.com/alex1701c/JetBrainsRunner")
set(CMAKE_PROJECT_AUTHOR "alex1701c")
set(CMAKE_PROJECT_AUTHOR_EMAIL "alex1701c.dev@gmx.net")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(KDE_COMPILERSETTINGS_LEVEL "5.82.0")
set(KF_MIN_VERSION "5.91.0")

find_package(ECM ${KF_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${CMAKE_MODULE_PATH})

include(FeatureSummary)
include(QtVersionOption)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMDeprecationSettings)
include(KDEClangFormat)

# Find the required Libaries
find_package(Qt${QT_MAJOR_VERSION} ${QT_MIN_VERSION} REQUIRED CONFIG COMPONENTS Widgets Core Xml Network)
find_package(KF${QT_MAJOR_VERSION} ${KF_MIN_VERSION} REQUIRED COMPONENTS I18n Service Runner KIO KCMUtils)
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

if (BUILD_WITH_QT6)
	set(KRUNNER_PACKAGE_VERSION ${KF5Runner_VERSION})
else()
	set(KRUNNER_PACKAGE_VERSION ${KF6Runner_VERSION})
endif()
# Then names of dependencies and paths might differ
if (CPACK_OS STREQUAL "debian")
	set(CPACK_DEBIAN_PACKAGE_DEPENDS "libkf5runner5 (>=${KRUNNER_PACKAGE_VERSION})")
	set(CPACK_DEBIAN_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-Debian.deb")
	set(CPACK_GENERATOR "DEB")
elseif(CPACK_OS STREQUAL "fedora")
	set(CPACK_RPM_PACKAGE_REQUIRES "kf5-krunner >= ${KRUNNER_PACKAGE_VERSION}")
	set(CPACK_RPM_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-Fedora.rpm")
	set(CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION "/usr/lib64/qt5;/usr/lib64/qt5/plugins;/usr/lib64/qt5/plugins/kf5;/usr/lib64/qt5/plugins/kf5/krunner;/usr/share/kservices5;/usr/share/icons;/usr/share/pixmaps")
	set(CPACK_GENERATOR "RPM")
elseif(CPACK_OS STREQUAL "opensuse")
	set(CPACK_RPM_PACKAGE_REQUIRES "libKF5Runner5 >= ${KRUNNER_PACKAGE_VERSION}")
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

