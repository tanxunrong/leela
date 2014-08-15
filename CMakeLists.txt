
project(leela)
cmake_minimum_required(VERSION 2.8)
aux_source_directory(. SRC_LIST)
add_definitions( -std=c99 -g )

set(HEADERS myc.h msg_queue.h handle.h context.h module.h)
#add_executable(time_client time_client.c ${HEADERS})
#add_executable(time_server time_server.c ${HEADERS})
#add_executable(echo_client echo_client.c ${HEADERS})
#add_executable(echo_server echo_server.c ${HEADERS})
add_executable(uds uds.c msg_queue.c handle.c context.c module.c ${HEADERS})

add_library(mruby STATIC IMPORTED)
set_target_properties(mruby PROPERTIES IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/mruby/build/host/lib/libmruby.a )
include_directories( ${PROJECT_SOURCE_DIR}/mruby/include )

add_library(toy SHARED service/service_toy.c)

if(UNIX)
    if(APPLE)
        include_directories(${INCLUDE_DIRECTORIES} /usr/local/Cellar/glib/2.36.4/include/glib-2.0 /usr/local/Cellar/glib/2.36.4/lib/glib-2.0/include
/usr/local/opt/gettext/include
/usr/local/Cellar/libevent/2.0.21/include)
        link_directories(${LINK_DIRECTORIES}
/usr/local/Cellar/glib/2.36.4/lib
/usr/local/opt/gettext/lib
#/usr/local/Cellar/curl/7.32.0/include
/usr/local/Cellar/libevent/2.0.21/lib)

        target_link_libraries(echo_server event glib-2.0)
        target_link_libraries(uds glib-2.0)
    else()
        include_directories(${INCLUDE_DIRECTORIES}
#/usr/include/glib-2.0)
/home/tanxr/local/include/glib-2.0)
        link_directories(${LINK_DIRECTORIES} /home/tanxr/local/lib)
#        target_link_libraries(echo_server glib-2.0 pthread)
        target_link_libraries(uds glib-2.0 gmodule-2.0 mruby m toy)
    endif()
endif()



