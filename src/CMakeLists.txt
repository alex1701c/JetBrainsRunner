add_definitions(-DTRANSLATION_DOMAIN=\"plasma_runner_org.kde.jetbrainsrunner\")

add_subdirectory(jetbrains-api)

add_library(krunner_jetbrainsrunner MODULE jetbrainsrunner.cpp)
target_link_libraries(krunner_jetbrainsrunner KF5::Runner KF5::I18n Qt5::Network KF5::KIOGui KF5::KIOCore)

set(kcm_krunner_jetbrainsrunner_SRCS
        config/jetbrainsrunner_config.cpp
        config/jetbrainsrunner_config_mapping_item.h)

ki18n_wrap_ui(kcm_krunner_jetbrainsrunner_SRCS config/jetbrainsrunner_config.ui)
ki18n_wrap_ui(kcm_krunner_jetbrainsrunner_SRCS config/jetbrainsrunner_config_mapping_item.ui)
add_library(kcm_krunner_jetbrainsrunner MODULE ${kcm_krunner_jetbrainsrunner_SRCS})
target_link_libraries(kcm_krunner_jetbrainsrunner
        Qt5::Core
        Qt5::Gui
        Qt5::Network
        KF5::CoreAddons
        KF5::ConfigCore
        KF5::I18n
        KF5::ConfigWidgets
        KF5::Runner
        )

target_link_libraries(krunner_jetbrainsrunner jetbrains_api_static)
target_link_libraries(kcm_krunner_jetbrainsrunner jetbrains_api_static)

configure_file(plasma-runner-jetbrainsrunner.desktop.in plasma-runner-jetbrainsrunner.desktop)

if(KF5Runner_VERSION VERSION_LESS 5.72.0)
    install(FILES ${CMAKE_BINARY_DIR}/src/plasma-runner-jetbrainsrunner.desktop DESTINATION ${KDE_INSTALL_KSERVICES5DIR})
else()
    kcoreaddons_desktop_to_json(krunner_jetbrainsrunner ${CMAKE_BINARY_DIR}/src/plasma-runner-jetbrainsrunner.desktop)
endif()
install(TARGETS kcm_krunner_jetbrainsrunner DESTINATION ${KDE_INSTALL_QTPLUGINDIR})
install(TARGETS krunner_jetbrainsrunner DESTINATION ${KDE_INSTALL_QTPLUGINDIR}/kf5/krunner/)
install(FILES config/plasma-runner-jetbrainsrunner_config.desktop DESTINATION ${KDE_INSTALL_KSERVICES5DIR})

set(JETBRAINS_ICON_INSTALL_DIR "/usr/share/pixmaps/" CACHE STRING "Default icon directory")
install(FILES jetbrainsrunner.png DESTINATION ${JETBRAINS_ICON_INSTALL_DIR})
#Icon downloaded from https://icon-icons.com/icon/jetbrains-toolbox/93803#64

