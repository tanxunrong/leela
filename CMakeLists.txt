
project(leela)
cmake_minimum_required(VERSION 2.8)
aux_source_directory(. SRC_LIST)
add_definitions( -std=c99 -g )

set(HEADERS myc.h)
add_executable(time_client time_client.c ${HEADERS})
add_executable(time_server time_server.c ${HEADERS})
add_executable(echo_client echo_client.c ${HEADERS})
add_executable(echo_server echo_server.c ${HEADERS})
add_executable(uds uds.c)

if(UNIX)
    if(APPLE)
        include_directories(${INCLUDE_DIRECTORIES} /usr/local/Cellar/glib/2.36.4/include/glib-2.0 /usr/local/Cellar/glib/2.36.4/lib/glib-2.0/include
/usr/local/opt/gettext/include
/usr/local/Cellar/libevent/2.0.21/include)
        link_directories(${LINK_DIRECTORIES}
/usr/local/Cellar/glib/2.36.4/lib
/usr/local/opt/gettext/lib
/usr/local/Cellar/curl/7.32.0/include
/usr/local/Cellar/libevent/2.0.21/lib)

        target_link_libraries(echo_server event glib-2.0)
    else()
        include_directories(${INCLUDE_DIRECTORIES}
/usr/include
/usr/include/glib-2.0
/usr/lib/x86_64-linux-gnu/glib-2.0/include)
        link_directories(${LINK_DIRECTORIES} /lib/x86_64-linux-gnu)
        target_link_libraries(echo_server event glib-2.0)
    endif()
endif()



