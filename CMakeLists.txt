cmake_minimum_required(VERSION 3.14)
project(cassie)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_TYPE Debug)
add_compile_definitions("DEBUG=$<CONFIG:Debug")

include_directories(include)
file(GLOB HEADERS "include/*.h")
include_directories(${HEADERS})
file(GLOB SOURCES "src/*/*.cpp")
add_executable(server ${SOURCES} src/main.cpp)

#link winsock 
if (WIN32)
    target_link_libraries(server ws2_32)
endif()

#link pthreads for UNIX/LINUX
if (UNIX)
    target_link_libraries(server pthread)
endif()