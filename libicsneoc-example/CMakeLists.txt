cmake_minimum_required(VERSION 3.2)
project(libicsneoc-example VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 11)

include(GNUInstallDirs)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../third-party/libicsneo/include)

add_executable(libicsneoc-example src/main.c)
