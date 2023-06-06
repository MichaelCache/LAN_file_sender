cmake_minimum_required(VERSION 3.5)

project(
  LAN_sender
  VERSION 0.1
  LANGUAGES CXX)

add_subdirectory(src)
add_subdirectory(test)

find_package(
  Qt5
  COMPONENTS Widgets Network Core
  REQUIRED)

add_executable(lanshare main.cpp)
target_link_libraries(lanshare PRIVATE share Qt5::Core Qt5::Widgets Qt5::Network)