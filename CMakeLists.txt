cmake_minimum_required(VERSION 3.10)
project(cassie)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_TYPE Debug)

include_directories(include)
file(GLOB SOURCES "src/*.cpp")
add_executable(server ${SOURCES})

#link winsock 
if (WIN32)
    target_link_libraries(server ws2_32)
endif()

#link pthreads for UNIX/LINUX
if (UNIX)
    target_link_libraries(server pthread)
endif()