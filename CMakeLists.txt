cmake_minimum_required(VERSION 3.5)

project(licensedigger LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(QT_MIN_VERSION "5.15.2")
set(KF5_MIN_VERSION "5.91.0")
set(KDE_COMPILERSETTINGS_LEVEL "5.82")

# ECM
find_package(ECM ${KF5_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})
include(ECMAddTests)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(KDEClangFormat)
include(ECMDeprecationSettings)

find_package(Qt${QT_MAJOR_VERSION} REQUIRED COMPONENTS
    Core
    Test
)

ecm_set_disabled_deprecation_versions(
    QT 5.15.2
    KF 5.102.0
)


add_subdirectory(autotests)

add_executable(
    licensedigger
    main.cpp
    licenseregistry.cpp
    directoryparser.cpp
    skipparser.cpp
    licenses.qrc
    annotations.qrc
)

ecm_mark_nongui_executable(licensedigger)

target_compile_definitions(licensedigger
  PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(licensedigger
  PRIVATE Qt::Core)

install(TARGETS licensedigger ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
