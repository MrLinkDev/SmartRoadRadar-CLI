cmake_minimum_required(VERSION 3.25)
project(smart_road)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXE_LINKER_FLAGS "-static")

add_executable(
        smart_road_radar
        src/main.cpp
        src/serial.hpp src/utils.hpp src/smart_road_radar.hpp src/smart_road_radar_utils.hpp src/smart_road_radar_cli.hpp)
