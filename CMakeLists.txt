cmake_minimum_required(VERSION 3.5)

project(licensedigger LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ECM
find_package(ECM ${KF5_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})
include(ECMAddTests)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)

find_package(Qt5 REQUIRED COMPONENTS
    Core
    Quick
    Test
)

add_subdirectory(autotests)

add_executable(
    licensedigger
    main.cpp
    licenseregistry.cpp
    directoryparser.cpp
    qml.qrc
    licenses.qrc
    annotations.qrc
)

target_compile_definitions(licensedigger
  PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(licensedigger
  PRIVATE Qt5::Core Qt5::Quick)
