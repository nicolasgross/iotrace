cmake_minimum_required(VERSION 3.13)
project(iotrace)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "-Wall")

file(GLOB SRCFILES
        "src/*.c"
		)

add_executable(iotrace src ${SRCFILES})
#target_link_libraries()

add_executable(diskio_example test test/diskio_example.c)

