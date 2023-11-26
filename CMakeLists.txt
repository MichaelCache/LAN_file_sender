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

if(WIN32)
  add_executable(lantransfer WIN32 main.cpp)
else()
  add_executable(lantransfer main.cpp)
endif()
target_link_libraries(lantransfer PRIVATE share Qt5::Core Qt5::Widgets
                                       Qt5::Network)
