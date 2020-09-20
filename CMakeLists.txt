CMAKE_MINIMUM_REQUIRED(VERSION 3.0.0)

project(BKBase)

if(WIN32)
	add_compile_options("/utf-8")
	add_definitions("-D_CRT_SECURE_NO_WARNINGS")
endif()

add_definitions("-DBK_EXPORT_IMPLEMENTS")

set(SOURCES bk_stream.c bk_buffer.c bk_data.c bk_vector.c bk_list.c)
set(HEADERS bk_stream.h bk_export.h bk_platform.h bk_buffer.h bk_data.h bk_vector.h bk_list.h)

add_executable(BKBase ${SOURCES} ${HEADERS} main.cpp)
