cmake_minimum_required(VERSION 3.5)

project(
  LAN_sender
  VERSION 0.1
  LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(QT_DIR)
  set(CMAKE_PREFIX_PATH ${QT_DIR})
endif()

set(CMAKE_BUILD_TYPE Debug)

find_package(
  Qt5
  COMPONENTS Widgets Network Core
  REQUIRED)

file(GLOB_RECURSE src src/*.cpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

add_executable(lanshare ${src})
target_link_libraries(lanshare PRIVATE Qt5::Core Qt5::Widgets Qt5::Network)

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    # Do something for Linux
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  target_link_libraries(lanshare PRIVATE iphlpapi wsock32)
endif()
