cmake_minimum_required(VERSION 3.14)

project(multimeterClient VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Quick REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Quick REQUIRED)

set(PROJECT_SOURCES
        src/domain/ChannelClient.h
        src/domain/ChannelClient.cpp
        src/domain/ChannelDispatcher.h
        src/domain/ChannelDispatcher.cpp
        src/gui/MultimeterClientManager.h
        src/gui/MultimeterClientManager.cpp
        src/gui/ChannelDataView.h
        src/gui/ChannelDataView.cpp
        src/main.cpp
        src/resources/qml.qrc
)

add_executable(multimeterClient
  ${PROJECT_SOURCES}
)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

target_compile_definitions(multimeterClient
  PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)

target_link_libraries(multimeterClient
  PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Quick)
