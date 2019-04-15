cmake_minimum_required(VERSION 3.13)
project(iotrace)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "-Wall -Wextra")
set(CMAKE_EXPORT_COMPILE_COMMANDS on)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB REQUIRED glib-2.0)
pkg_check_modules(JSON_GLIB REQUIRED json-glib-1.0)
pkg_check_modules(GOBJECT REQUIRED gobject-2.0)

# Copied from: https://github.com/darktable-org/darktable/blob/ae01e8f9f9ec14572930317fdf0cacc614300837/cmake/modules/FindJsonGlib.cmake#L13
find_path(JSONGLIB_INCLUDE_DIRS
	NAMES json-glib/json-glib.h
	HINTS ${JSONGLIB_PKGCONF_INCLUDE_DIRS}
	PATH_SUFFIXES json-glib-1.0
)

file(GLOB SRCFILES
	"src/*.c"
	)

add_executable(iotrace src ${SRCFILES})
target_include_directories(iotrace PRIVATE ${GLIB_INCLUDE_DIRS}
	${JSONGLIB_INCLUDE_DIRS})
target_link_libraries(iotrace glib-2.0 json-glib-1.0 gobject-2.0)

add_executable(diskio test test/diskio.c)

add_executable(dup test test/dup.c)

