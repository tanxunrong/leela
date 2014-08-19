
project(leela)
cmake_minimum_required(VERSION 2.8)
#aux_source_directory(. SRC_LIST)
add_definitions( -Wall -std=c99 -g )

set(HEADERS msg_queue.h handle.h context.h module.h leela.h monitor.h start.h)
add_executable(leela leela.c msg_queue.c handle.c context.c module.c monitor.c start.c ${HEADERS})

add_library(mruby STATIC IMPORTED)
set_target_properties(mruby PROPERTIES IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/mruby/build/host/lib/libmruby.a )

add_library(toy SHARED service/service_toy.c)
add_library(logger SHARED service/service_logger.c)

include_directories(${INCLUDE_DIRECTORIES}
					${PROJECT_SOURCE_DIR}/mruby/include
					#/usr/include/glib-2.0)
					/home/tanxr/local/include/glib-2.0)
link_directories(${LINK_DIRECTORIES} /home/tanxr/local/lib)
target_link_libraries(leela glib-2.0 gmodule-2.0 mruby m toy logger)

