cmake_minimum_required(VERSION 3.5)

project(licensedigger LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(QT_MIN_VERSION "5.15.0")
set(KF5_MIN_VERSION "5.90.0")
set(KDE_COMPILERSETTINGS_LEVEL "5.82")

# ECM
find_package(ECM ${KF5_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})
include(ECMAddTests)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(KDEClangFormat)

find_package(Qt${QT_MAJOR_VERSION} REQUIRED COMPONENTS
    Core
    Test
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

install(TARGETS licensedigger ${INSTALL_TARGETS_DEFAULT_ARGS})
