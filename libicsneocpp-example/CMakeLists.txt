cmake_minimum_required(VERSION 3.2)
project(libicsneocpp-example VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 11)

include(GNUInstallDirs)

# Add an include directory like so if desired
#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Enable Warnings
if(MSVC)
	# Force to always compile with W4
	if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
		string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
	else()
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
	endif()
else() #if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-switch -Wno-nested-anon-types -Wno-gnu-anonymous-struct -Wno-unknown-pragmas -Wno-zero-length-array")
endif()

add_subdirectory("third-party/libicsneo")

add_executable(libicsneocpp-example src/main.cpp)
target_link_libraries(libicsneocpp-example icsneocpp)