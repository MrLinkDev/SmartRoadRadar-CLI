#include "smart_road_radar/smart_road_radar_cli.cpp"

SmartRoadRadarCli cli;

void stop(int s) {
    cli.breakReadLoop();
}

int main(int argc, char* argv[]) {
    signal(SIGINT, stop);

    LPTSTR address = LPTSTR(argv[1]);

    cli = SmartRoadRadarCli(address);
    cli.mainLoop();
}