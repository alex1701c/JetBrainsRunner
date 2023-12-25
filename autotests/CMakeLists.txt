include(ECMAddTests)
find_package(Qt${QT_MAJOR_VERSION} ${QT_MIN_VERSION} REQUIRED CONFIG COMPONENTS Test)

ecm_add_test(jetbrainsrunnerintegrationtest.cpp LINK_LIBRARIES Qt::Test KF${QT_MAJOR_VERSION}::Runner KF${QT_MAJOR_VERSION}::Service)
krunner_configure_test(jetbrainsrunnerintegrationtest krunner_jetbrainsrunner)

if (COMMAND krunner_configure_test)
  krunner_configure_test(jetbrainsrunnerintegrationtest krunner_jetbrainsrunner)
else()
  configure_krunner_test(jetbrainsrunnerintegrationtest krunner_jetbrainsrunner)
endif()
