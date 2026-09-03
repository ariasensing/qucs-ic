cmake_minimum_required(VERSION 3.16)

project(ictech LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

add_library(ictech STATIC
  ictech_global.h
  ictech.cpp
  ictech.h
  substrate.h substrate.cpp
)

target_include_directories(ictech PRIVATE ${PROJECT_SOURCE_DIR})
target_link_libraries(ictech PRIVATE Qt${QT_VERSION_MAJOR}::Core)

target_compile_definitions(ictech PRIVATE ICTECH_LIBRARY)
