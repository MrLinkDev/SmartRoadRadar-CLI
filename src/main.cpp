#include "smart_road_radar_cli.hpp"

void usage() {
    printf("SmartRoadRadar-CLI\n\n");
    printf("Run with COM-port name as argument.\n");
    printf("Example: smart_road_radar.exe COM1\n");
}

int main(int argc, char* argv[]) {

    if (argc < 2 || argc > 2) {
        usage();
        exit(-1);
    }

    SmartRoadRadarCLI radar_cli((LPTSTR) argv[1]);
    radar_cli.main_loop();

    exit(0);
}