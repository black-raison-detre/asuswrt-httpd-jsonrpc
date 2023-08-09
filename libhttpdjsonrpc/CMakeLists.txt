cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)
project(libhttpdjsonrpc VERSION 0.1.0 LANGUAGES C)

find_package(PkgConfig REQUIRED)

pkg_check_modules(JSONC REQUIRED json-c)
pkg_get_variable(CFLAGS json-c cflags)
pkg_get_variable(LDFLAGS json-c ldflags)

set(LIB_HEADER jsonrpc.h)
set(LIB_SRC
	jsonrpc.c nvram.c action_script.c amng_custom.c
	addon_config.c addon_config_json.c file_ops.c common_ops.c
	)
set(CMAKE_C_FLAGS "-DRPC_TEST")
set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} ${CFLAGS})

add_library(${PROJECT_NAME} ${LIB_HEADER} ${LIB_SRC})

target_link_libraries(${PROJECT_NAME} ${LDFLAGS})
