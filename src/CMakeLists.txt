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

add_library(share ${src})
target_link_libraries(share PUBLIC Qt5::Core Qt5::Widgets Qt5::Network)
target_include_directories(share PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})