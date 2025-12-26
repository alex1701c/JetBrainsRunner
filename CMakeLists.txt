cmake_minimum_required(VERSION 3.16)

project(jetbrainsrunner)
set(CMAKE_PROJECT_VERSION "3.0.6") # Handled by release workflow
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

set(KRUNNER_VERSION "${KF${QT_MAJOR_VERSION}Runner_VERSION}")
set(CPACK_PACKAGE_FILE_NAME ${CMAKE_PROJECT_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_OS}-plasma${QT_MAJOR_VERSION})
# Then names of dependencies and paths might differ
if (CPACK_OS STREQUAL "ubuntu")
	set(CPACK_DEBIAN_PACKAGE_DEPENDS "libkf${QT_MAJOR_VERSION}runner${QT_MAJOR_VERSION} (>=${KRUNNER_VERSION})")
	set(CPACK_GENERATOR "DEB")
elseif(CPACK_OS STREQUAL "opensuse" AND QT_MAJOR_VERSION STREQUAL 5)
	set(CPACK_RPM_PACKAGE_REQUIRES "libKF5Runner5 >= ${KRUNNER_VERSION}")
	set(CPACK_GENERATOR "RPM")
elseif(CPACK_OS STREQUAL "fedora" OR CPACK_OS STREQUAL "opensuse")
	set(CPACK_RPM_PACKAGE_REQUIRES "kf${QT_MAJOR_VERSION}-krunner >= ${KRUNNER_VERSION}")
	set(CPACK_GENERATOR "RPM")
else()
  unset(CPACK_PACKAGE_FILE_NAME)
endif()

# These scripts will kill krunner and when it is invoked next time the plugins will be loaded
set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${CMAKE_CURRENT_SOURCE_DIR}/postinst;${CMAKE_CURRENT_SOURCE_DIR}/postrm")
set(CPACK_RPM_POST_INSTALL_SCRIPT_FILE "${CMAKE_CURRENT_SOURCE_DIR}/postinst")
set(CPACK_RPM_POST_UNINSTALL_SCRIPT_FILE "${CMAKE_CURRENT_SOURCE_DIR}/postrm")

INCLUDE(CPack)

