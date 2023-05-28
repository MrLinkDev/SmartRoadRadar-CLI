SmartRoad radar Communication
=============================

Данная библиотека позволяет реализовать управление радаром SmartRoad по определённому протоколу.
В качестве примера реализации, в репозиторие имеется объект SmartRoadRadarCli. Также, к объектам,
которые будут использоваться, написана документация, с которой можно ознакомиться перейдя по ссылкам ниже.

**Объекты, добавляемые библиотекой:**
* Serial
* SmartRoadRadar

Настройка проекта
-----------------
Сборка проекта осуществлялась со следующими параметрами в CMakeLists:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.txt}
cmake_minimum_required(VERSION 3.25)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXE_LINKER_FLAGS "-static")
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

1. Добавьте в ваш проект следующие файлы:
   * serial.hpp
   * smart_road_radar.hpp
   * smart_road_radar_utils.hpp
   * utils.hpp
2. Подключите добавленные файлы к проекту в CMakeLists.txt, например, следующим образом:
   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.txt}
   cmake_minimum_required(VERSION 3.25)
   project(smart_road)

   set(CMAKE_CXX_STANDARD 17)
   set(CMAKE_EXE_LINKER_FLAGS "-static")
   
   add_executable(
       smart_road_radar
       src/main.cpp
       src/serial.hpp
       src/utils.hpp
       src/smart_road_radar.hpp
       src/smart_road_radar_utils.hpp
       src/smart_road_radar_cli.hpp)
   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
3. Соберите Ваш проект.