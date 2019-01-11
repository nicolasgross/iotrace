cmake_minimum_required(VERSION 3.13)
project(iotrace)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "-std=c11 -Wall")

file(GLOB SRCFILES
        "src/*.c"
		)

add_executable(iotrace src src/iotrace.c)
#target_link_libraries()
