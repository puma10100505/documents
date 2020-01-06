cmake_minimum_required(VERSION 2.8)

project(game_networking)
set(bin_name networking_lab)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -g -O2 ")

option(with_ode "switch for ode including" OFF)

if (${with_ode} STREQUAL "ON")
add_definitions(-DGLCLI)
elseif (${with_ode} STREQUAL "OFF")
endif()

set(solution_root ${CMAKE_CURRENT_SOURCE_DIR}/)
set(project_root ${CMAKE_CURRENT_SOURCE_DIR}/src/)

message(${solution_root})
message(${project_root})

# source code list
file(GLOB source 
    ${project_root}/main.cpp 
    ${project_root}/*.cpp 
)

# message(${source})

include_directories(
    ${solution_root}/include
    ${solution_root}/proto
    ${solution_root}/src
    /usr/local/include
)

link_directories(
    /usr/local/lib
    ${solution_root}/libs
)

find_library(lib_pb protobuf ${solution_root}/libs /usr/local/lib)
find_library(lib_boost_chrono boost_chrono ${solution_root}/libs /usr/local/lib)
find_library(lib_boost_log boost_log ${solution_root}/libs /usr/local/lib)
find_library(lib_boost_log_setup boost_log_setup ${solution_root}/libs /usr/local/lib)

if ($(with_ode) STREQUAL "ON")
find_library(lib_ode ode /usr/local/lib)
find_library(lib_drawstuff drawstuff ${solution_root}/libs)
endif()

add_executable(
    ${bin_name}
    ${source}
)

if (${with_ode} STREQUAL "ON")
target_link_libraries(
    ${bin_name}
    ${lib_pb}
    ${lib_ode}
    ${lib_drawstuff}
    -lstdc++
    -lpthread
    -lboost_thread
    -lboost_chrono
    -lrt
    -lGL 
    -lX11 
    -lGLU
)
elseif (${with_ode} STREQUAL "OFF")
target_link_libraries(
    ${bin_name}
    ${lib_pb}
    -lboost_thread
    -lboost_chrono
    -lstdc++
    -lpthread
    -lrt
)
endif()

install(PROGRAMS ${CMAKE_BINARY_DIR}/${bin_name} DESTINATION ${solution_root}/bin)
