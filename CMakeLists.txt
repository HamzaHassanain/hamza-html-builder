cmake_minimum_required(VERSION 3.10)
project(html_builder)


# define a variable called debug, that I can acess in a c++ code, to enable debugging
set(DEBUG_MODE ON CACHE BOOL "Enable Debug Mode") 

if(DEBUG_MODE)
    add_definitions(-DDEBUG_MODE)
endif()

# add debuging for c++ gcc

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -fsanitize=address -g -O0")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")


file(GLOB SRC_FILES src/*.cpp)


set(CMAKE_CXX_STANDARD 17)
include_directories(${CMAKE_SOURCE_DIR}/includes)


# add_executable(  builder app.cpp ${SRC_FILES})

add_library(builder STATIC ${SRC_FILES})