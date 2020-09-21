CMAKE_MINIMUM_REQUIRED(VERSION 3.0.0)

project(BKBase)

if(WIN32)
	add_compile_options("/utf-8")
	add_definitions("-D_CRT_SECURE_NO_WARNINGS")
endif()

add_definitions("-DBK_EXPORT_IMPLEMENTS")

file(GLOB_RECURSE BKBASE_SOURCES "./src/*.c" "./src/*.cpp" "./src/*.cc")
file(GLOB_RECURSE BKBASE_HEADERS "./include/*.h" "./include/*.hpp")

add_library(BKBase SHARED ${BKBASE_SOURCES} ${BKBASE_HEADERS})
target_include_directories(BKBase PUBLIC "${CMAKE_CURRENT_LIST_DIR}/include")
set(BKBASE_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/deps/gtest/googletest/include")

if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/deps/gtest/googletest/src/gtest-all.cc")
	option(ENABLE_GKBASE_TEST "Build unit test" ON)
	if(ENABLE_GKBASE_TEST)
		file(GLOB_RECURSE BKBASE_TEST_SOURCES "./test/*.c" "./test/*.cpp" "./test/*.cc" "./test/*.h" "./test/*.hpp")
		add_executable(BKBase_Test ${BKBASE_TEST_SOURCES} "${CMAKE_CURRENT_LIST_DIR}/deps/gtest/googletest/src/gtest-all.cc")

		target_include_directories(BKBase_Test 
			PRIVATE "${BKBASE_INCLUDE_DIR}"
			PRIVATE "${CMAKE_CURRENT_LIST_DIR}/deps/gtest/googletest")
		target_include_directories(BKBase_Test PUBLIC "${CMAKE_CURRENT_LIST_DIR}/include")
		target_link_libraries(BKBase_Test BKBase)
	endif()
endif()
