#include "smart_road_radar.h"

LPTSTR address = nullptr;
SmartRoadRadar radar;
TargetData *targetData;

void write_thread();
void read_thread();

int main() {
    address = LPTSTR("COM8");
    radar = SmartRoadRadar(address);

    char *version;
    radar.getFirmwareVersion(version);

    cout << version << endl;

    if (radar.enableDataTransmit() == SUCCESS) {
        while (true) {
            targetData = new TargetData[35];
            radar.getTargetData(targetData);

            for (int i = 0; i < 3; ++i) {
                printf(
                        "%d | %2.2f m\t%2.2f m/s\t%2.2f degree\n",
                        targetData[i].targetNum,
                        targetData[i].distance,
                        targetData[i].speed,
                        targetData[i].angle);
            }
            printf("----------------------------------");
        }
    }


}