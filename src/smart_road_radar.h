#ifndef SMART_ROAD_SMART_ROAD_RADAR_H
#define SMART_ROAD_SMART_ROAD_RADAR_H

#include "serial.h"

#define CMD_VERSION_SET     0x10

#define CMD_CONFIG_SET      0x36
#define CMD_CONFIG_GET      0x37

#define CMD_TARGET_NUM_SET  0x12

#define CMD_START_TRANSMIT  0x20
#define CMD_STOP_TRANSMIT   0x21

#define CMD_SET_DATA_FREQ   0x18

#define CMD_ZERO_DATA       0x19

#define HEADER_VERSION      0x11
#define HEADER_STATUS       0x50
#define HEADER_CONFIG       0x38

#define HEADER_TARGET_NUM   0x42

#define HEADER_DATA_FRAME_1 0x55
#define HEADER_DATA_FRAME_2 0xAA

Serial dataBus;


class SmartRoadRadar {

};


#endif //SMART_ROAD_SMART_ROAD_RADAR_H
