cmake_minimum_required(VERSION 3.5)

project(
  LAN_sender
  VERSION 0.1
  LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

add_subdirectory(src)
add_subdirectory(test)

find_package(
  Qt5
  COMPONENTS Widgets Network Core
  REQUIRED)

add_executable(lanshare main.cpp)
target_link_libraries(lanshare PRIVATE share Qt5::Core Qt5::Widgets
                                       Qt5::Network)
