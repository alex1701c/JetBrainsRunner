include(ECMAddTests)
find_package(Qt5 ${QT_MIN_VERSION} REQUIRED CONFIG COMPONENTS Test)

ecm_add_test(jetbrainsrunnerintegrationtest.cpp LINK_LIBRARIES Qt::Test KF5::Runner KF5::Service)
configure_krunner_test(jetbrainsrunnerintegrationtest krunner_jetbrainsrunner)
