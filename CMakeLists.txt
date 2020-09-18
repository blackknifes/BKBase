CMAKE_MINIMUM_REQUIRED(VERSION 3.0.0)

project(BKBase)

add_definitions("-DBK_EXPORT_IMPLEMENTS")

add_executable(BKBase main.cpp bk_stream.c bk_stream.h bk_export.h bk_platform.h)