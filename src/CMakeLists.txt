add_definitions(-DTRANSLATION_DOMAIN=\"plasma_runner_org.kde.jetbrainsrunner\")

add_subdirectory(jetbrains-api)

add_library(krunner_jetbrainsrunner MODULE jetbrainsrunner.cpp)
target_link_libraries(krunner_jetbrainsrunner
    Qt::Network
    KF${QT_MAJOR_VERSION}::Runner
    KF${QT_MAJOR_VERSION}::I18n
    KF${QT_MAJOR_VERSION}::KIOGui
    KF${QT_MAJOR_VERSION}::KIOCore
)

set(kcm_krunner_jetbrainsrunner_SRCS
        config/jetbrainsrunner_config.cpp
        config/jetbrainsrunner_config_mapping_item.h)

ki18n_wrap_ui(kcm_krunner_jetbrainsrunner_SRCS config/jetbrainsrunner_config.ui)
ki18n_wrap_ui(kcm_krunner_jetbrainsrunner_SRCS config/jetbrainsrunner_config_mapping_item.ui)
add_library(kcm_krunner_jetbrainsrunner MODULE ${kcm_krunner_jetbrainsrunner_SRCS})
target_link_libraries(kcm_krunner_jetbrainsrunner
    Qt::Core
    Qt::Gui
    Qt::Network
    KF${QT_MAJOR_VERSION}::CoreAddons
    KF${QT_MAJOR_VERSION}::ConfigCore
    KF${QT_MAJOR_VERSION}::I18n
    KF${QT_MAJOR_VERSION}::ConfigWidgets
    KF${QT_MAJOR_VERSION}::Runner
)

target_link_libraries(krunner_jetbrainsrunner jetbrains_api_static)
target_link_libraries(kcm_krunner_jetbrainsrunner jetbrains_api_static)

configure_file(jetbrainsrunner.json.in jetbrainsrunner.json)

install(TARGETS kcm_krunner_jetbrainsrunner DESTINATION ${KDE_INSTALL_QTPLUGINDIR})
install(TARGETS krunner_jetbrainsrunner DESTINATION ${KDE_INSTALL_QTPLUGINDIR}/kf5/krunner/)

set(JETBRAINS_ICON_INSTALL_DIR )
install(FILES jetbrainsrunner.png DESTINATION "${KDE_INSTALL_DATADIR}/pixmaps/")
#Icon downloaded from https://icon-icons.com/icon/jetbrains-toolbox/93803#64

