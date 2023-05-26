cmake_minimum_required(VERSION 3.25)
project(THE3.X)

set(CMAKE_C_STANDARD 99)

include_directories(SYSTEM .)
include_directories(/Applications/microchip/xc8/v2.30/pic/include/)

add_executable(THE3.X
        pragmas.h
        lcd.h
        the3.h
        the3.c)