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
  ictech_ktech.cpp
)

target_include_directories(ictech PRIVATE ${PROJECT_SOURCE_DIR})
target_link_libraries(ictech PRIVATE Qt${QT_VERSION_MAJOR}::Core)

target_compile_definitions(ictech PRIVATE ICTECH_LIBRARY)

set(KLAYOUT_INCLUDE_HOME "/usr/local/share/klayout")
target_include_directories(ictech PRIVATE ${KLAYOUT_INCLUDE_HOME}/include/lay)
target_include_directories(ictech PRIVATE ${KLAYOUT_INCLUDE_HOME}/include/layui)
target_include_directories(ictech PRIVATE ${KLAYOUT_INCLUDE_HOME}/include/layview)
target_include_directories(ictech PRIVATE ${KLAYOUT_INCLUDE_HOME}/include/laybasic)
target_include_directories(ictech PRIVATE ${KLAYOUT_INCLUDE_HOME}/include/db)
target_include_directories(ictech PRIVATE ${KLAYOUT_INCLUDE_HOME}/include/tl)
target_include_directories(ictech PRIVATE ${KLAYOUT_INCLUDE_HOME}/include/gsi)

set(KLAYOUT_LIBRARY_HOME "/home/alessioc/projects/qucs_s/klayout/bin-release/")
target_link_directories(ictech PRIVATE ${KLAYOUT_LIBRARY_HOME})
target_link_directories(ictech PRIVATE ${KLAYOUT_LIBRARY_HOME}/db_plugins)
target_link_directories(ictech PRIVATE ${KLAYOUT_LIBRARY_HOME}/lay_plugins)

target_link_libraries(ictech PRIVATE klayout_lay)
target_link_libraries(ictech PRIVATE klayout_laybasic)
target_link_libraries(ictech PRIVATE klayout_layview)
target_link_libraries(ictech PRIVATE klayout_db)
target_link_libraries(ictech PRIVATE klayout_rdb)
target_link_libraries(ictech PRIVATE klayout_tl)
target_link_libraries(ictech PRIVATE klayout_gsi)

target_link_libraries(ictech PRIVATE gds2)
target_link_libraries(ictech PRIVATE lefdef)
target_link_libraries(ictech PRIVATE lstream)
target_link_libraries(ictech PRIVATE oasis)
target_link_libraries(ictech PRIVATE net_tracer)
