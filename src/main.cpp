#include "smart_road_radar/smart_road_radar.h"

LPTSTR address = nullptr;
SmartRoadRadar radar;
TargetData *targetData;

void send(int param){
    radar.disableDataTransmit();
    radar.getFirmwareVersion();
}

int main() {
    signal(SIGINT, send);
    address = LPTSTR("COM3");

    radar = SmartRoadRadar(address);

    radar.start();
    radar.stop();
}