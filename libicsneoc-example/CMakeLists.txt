cmake_minimum_required(VERSION 3.2)
project(libicsneoc-example VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 11)

include(GNUInstallDirs)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../third-party/libicsneo/include)

if(UNIX)   
    set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS)

    add_executable(libicsneoc-example src/main.c)
    
    target_link_libraries(libicsneoc-example ${CMAKE_DL_LIBS})
elseif(WIN32)
    add_executable(libicsneoc-example src/main.c)
endif()