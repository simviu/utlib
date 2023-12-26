#set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake"
#  CACHE STRING "Vcpkg toolchain file")

#---- temp fix for gcc new install
#set(CMAKE_C_COMPILER "/usr/bin/gcc")
#set(CMAKE_CXX_COMPILER "/usr/bin/g++")

message(STATUS "C Compiler:" ${CMAKE_C_COMPILER})
message(STATUS "CXX Compiler:" ${CMAKE_CXX_COMPILER})
message(STATUS "CXX version: ${CMAKE_CXX_COMPILER_VERSION}")

project(utLib)
cmake_minimum_required(VERSION 3.4)


SET(CMAKE_CXX_STANDARD 17)
add_definitions(-fPIC)
#find_package (jsoncpp REQUIRED)
#find_package(Gflags REQUIRED)

message(STATUS "CMAKE_SYSTEM_PREFIX_PATH = ${CMAKE_SYSTEM_PREFIX_PATH}")
#message(STATUS "JsonCpp_LIBRARIES =: ${JsonCpp_LIBRARIES}")

include_directories( 
    ${CMAKE_PREFIX_PATH}/include
#    /usr/include/jsoncpp
    #/opt/dep/local/include
    inc
    )
    
    
#---- utLib 
FILE(GLOB  SRC_UTLIB src/*.cpp)
add_library(utLib ${SRC_UTLIB})
target_link_libraries(utLib   
#    ${JsonCpp_LIBRARIES} 
#    jsoncpp
    pthread
    )
    
set(ENV{utLib_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
  
