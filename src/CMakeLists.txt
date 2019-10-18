add_definitions(-DTRANSLATION_DOMAIN=\"plasma_runner_org.kde.jetbrainsrunner\")

set(jetbrainsrunner_SRCS jetbrainsrunner.cpp JetbrainsApplication.cpp JetbrainsApplication.h SettingsDirectory.cpp SettingsDirectory.h)

add_library(krunner_jetbrainsrunner MODULE ${jetbrainsrunner_SRCS})
target_link_libraries(krunner_jetbrainsrunner KF5::Runner KF5::I18n Qt5::Network)

set(kcm_krunner_jetbrainsrunner_SRCS
        config/jetbrainsrunner_config.cpp
        )

ki18n_wrap_ui(kcm_krunner_jetbrainsrunner_SRCS config/jetbrainsrunner_config.ui)
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

add_dependencies(krunner_jetbrainsrunner kcm_krunner_jetbrainsrunner)


install(TARGETS krunner_jetbrainsrunner kcm_krunner_jetbrainsrunner DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES plasma-runner-jetbrainsrunner.desktop config/plasma-runner-jetbrainsrunner_config.desktop DESTINATION ${KDE_INSTALL_KSERVICES5DIR})
install(FILES jetbrains.png DESTINATION /usr/share/icons/)
#Icon downloaded from https://icon-icons.com/icon/jetbrains-toolbox/93803#64