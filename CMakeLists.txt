
project(leela)
cmake_minimum_required(VERSION 2.8)
aux_source_directory(. SRC_LIST)

set(HEADERS myc.h)
include_directories(${INCLUDE_DIRECTORIES} /usr/include/glib-2.0 /usr/lib/x86_64-linux-gnu/glib-2.0/include)
link_directories(${LINK_DIRECTORIES} /lib/x86_64-linux-gnu)
message(${CMAKE_C_FLAGS_DEBUG})

add_executable(time_client time_client.c ${HEADERS})
add_executable(time_server time_server.c ${HEADERS})
add_executable(echo_client echo_client.c ${HEADERS})
add_executable(echo_server echo_server.c ${HEADERS})

add_executable(test test.c)
target_link_libraries(test glib-2.0)
#add_executable(${PROJECT_NAME} ${SRC_LIST})
