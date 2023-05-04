#include "smart_road_radar_cli.hpp"



int main(int argc, char* argv[]) {

    SmartRoadRadarCLI radar_cli((LPTSTR) argv[1]);

    radar_cli.main_loop();
}