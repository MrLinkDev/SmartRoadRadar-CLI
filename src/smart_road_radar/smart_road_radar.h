#include <thread>

#include "../serial.h"
#include "smart_road_radar_utils.h"

#ifndef SMART_ROAD_SMART_ROAD_RADAR_H
#define SMART_ROAD_SMART_ROAD_RADAR_H

class SmartRoadRadar {

private:
    Serial dataBus{};
    std::thread inputStreamThread;

    bool isReadingInputStream = false;
    bool isPaused = false;

    Frame readFrame();
    void writeFrame(Frame frame);

    Frame configureFrame(byte_t cmd, byte_t *data, unsigned short length);
    Frame configureFrame(byte_t cmd, byte_t data);
    Frame configureFrame(byte_t cmd);

    void readInputStream();

public:
    SmartRoadRadar() = default;
    SmartRoadRadar(char *address);
    SmartRoadRadar(char *address, PortConfig config);

    void getFirmwareVersion();

    void enableDataTransmit();
    void disableDataTransmit();

    void start();
    void stop();
};


#endif //SMART_ROAD_SMART_ROAD_RADAR_H
