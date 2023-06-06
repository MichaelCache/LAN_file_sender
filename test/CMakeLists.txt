set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_TYPE Debug)

find_package(
  Qt5
  COMPONENTS Widgets Network Core
  REQUIRED)

file(GLOB_RECURSE src *.cpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(test ${src})
target_link_libraries(test PRIVATE share)
target_include_directories(test PRIVATE share)