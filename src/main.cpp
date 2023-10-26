#include "smart_road_radar_cli.hpp"

#define DEMO_ADDRESS "DEMO"

void usage() {
    printf("SmartRoadRadar-CLI\n\n");
    printf("Run with COM-port name and baud rate as arguments.\n");
    printf("Example: smart_road_radar.exe COM1 230400\n");
}

int main(int argc, char* argv[]) {

    if (argc < 3 || argc > 3) {
        usage();
        exit(-1);
    }

    SmartRoadRadarCLI *radar_cli;

    if (strcmp(argv[1], DEMO_ADDRESS) == 0) {
        radar_cli = new SmartRoadRadarCLI();
    } else {
        port_config config{};

        config.baud_rate = atoi(argv[2]);
        config.byte_size = BYTE_SIZE;
        config.stop_bits = ONESTOPBIT;
        config.parity = NOPARITY;

        radar_cli = new SmartRoadRadarCLI((LPTSTR) argv[1], config);
    }

    radar_cli->main_loop();

    exit(0);
}