
project(leela)
cmake_minimum_required(VERSION 2.8)
aux_source_directory(. SRC_LIST)

set(HEADERS myc.h)

message(${CMAKE_C_FLAGS_DEBUG})
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g")

add_executable(time_client time_client.c ${HEADERS})
add_executable(time_server time_server.c ${HEADERS})
add_executable(echo_client echo_client.c ${HEADERS})
add_executable(echo_server echo_server.c ${HEADERS})

add_executable(test test.c)
#add_executable(${PROJECT_NAME} ${SRC_LIST})

