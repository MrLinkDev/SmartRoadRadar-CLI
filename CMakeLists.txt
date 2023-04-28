cmake_minimum_required(VERSION 3.25)
project(smart_road)

set(CMAKE_CXX_STANDARD 17)

add_executable(
        smart_road
        src/main.cpp
        src/serial.cpp
        src/serial.h
        src/smart_road_radar/smart_road_radar.cpp
        src/smart_road_radar/smart_road_radar.h
        src/smart_road_radar/smart_road_radar_utils.h)
