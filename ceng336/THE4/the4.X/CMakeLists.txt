cmake_minimum_required(VERSION 3.25)
project(THE4.X)

set(CMAKE_C_STANDARD 99)

include_directories(SYSTEM .)
include_directories(/Applications/microchip/xc8/v2.30/pic/include/)

add_executable(THE4.X
        command_task.c
        common.c
        common.h
        int.c
        main.c
        response_task.c
        taskdesc.c)