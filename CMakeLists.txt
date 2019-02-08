cmake_minimum_required(VERSION 3.13)
project(iotrace)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "-Wall -Wextra")
set(CMAKE_EXPORT_COMPILE_COMMANDS on)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB REQUIRED glib-2.0)

file(GLOB SRCFILES
	"src/*.c"
	)

add_executable(iotrace src ${SRCFILES})
target_include_directories(iotrace PRIVATE ${GLIB_INCLUDE_DIRS})
target_link_libraries(iotrace glib-2.0)

add_executable(diskio_example test test/diskio_example.c)

