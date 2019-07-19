cmake_minimum_required(VERSION 3.2)
project(libicsneojava VERSION 0.1.0)

find_package(SWIG REQUIRED)
find_package(Java REQUIRED)
find_package(JNI REQUIRED)

include(UseJava)
include(UseSWIG)
include_directories(${JNI_INCLUDE_DIRS})

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../third-party/libicsneo ${CMAKE_CURRENT_BINARY_DIR}/third-party/libicsneo)

add_library(libicsneojava SHARED ${CMAKE_CURRENT_SOURCE_DIR}/java_wrap.c)
target_link_libraries(libicsneojava icsneoc)