#include <vector>
#include <conio.h>
#include <signal.h>

#include "smart_road_radar.h"

class SmartRoadRadarCli {

private:
    SmartRoadRadar radar;
    bool shouldStopReading;

    void usage() {
        system("cls");

        printf(" ------------------------------------------- \n");
        printf("|     Smart road radar communication        |\n");
        printf(" ------------------------------------------- \n");
        printf("version     - returns firmware version of radar\n");
        printf("\n");
        printf("set-params  - writing params into radar.\nExamples:\n");
        printf("\tset-params min_dist max_dist min_speed max_speed min_angle max_angle left_border right_border\n");
        printf("\tset-params default\n");
        printf("\n");
        printf("get-targets - starting loop which getting\n");
        printf("              target data and shows it in\n");
        printf("              console. Ctrl+C breaking loop.\n");
    }

public:
    SmartRoadRadarCli() = default;

    SmartRoadRadarCli(LPTSTR address) {
        radar = SmartRoadRadar(address);
    }

    void breakReadLoop() {
        shouldStopReading = true;
    }

    int mainLoop() {
        string line;
        string delimiter = " ";

        while (true) {
            line = "";

            printf("srr:\\> ");
            getline(cin, line);

            string cmd;

            cmd = line.substr(0, line.find(delimiter));
            line.erase(0, line.find(delimiter) + delimiter.length());

            if (cmd == "version") {
                radar.getFirmwareVersion();
            } else if (cmd == "enable") {
                radar.enableDataTransmit();
            } else if (cmd == "disable") {
                radar.disableDataTransmit();
            } else if (cmd == "set-params") {
                Parameters params;

                if (line != "default") {
                    params.minDistance.inFloat = stof(line.substr(0, line.find(delimiter)));
                    line.erase(0, line.find(delimiter) + delimiter.length());

                    params.maxDistance.inFloat = stof(line.substr(0, line.find(delimiter)));
                    line.erase(0, line.find(delimiter) + delimiter.length());

                    params.minSpeed.inFloat = stof(line.substr(0, line.find(delimiter)));
                    line.erase(0, line.find(delimiter) + delimiter.length());

                    params.maxSpeed.inFloat = stof(line.substr(0, line.find(delimiter)));
                    line.erase(0, line.find(delimiter) + delimiter.length());

                    params.minAngle.inFloat = stof(line.substr(0, line.find(delimiter)));
                    line.erase(0, line.find(delimiter) + delimiter.length());

                    params.maxAngle.inFloat = stof(line.substr(0, line.find(delimiter)));
                    line.erase(0, line.find(delimiter) + delimiter.length());

                    params.leftBorder.inFloat = stof(line.substr(0, line.find(delimiter)));
                    line.erase(0, line.find(delimiter) + delimiter.length());

                    params.rightBorder.inFloat = stof(line.substr(0, line.find(delimiter)));
                    line.erase(0, line.find(delimiter) + delimiter.length());
                }

                radar.setParameters(params);
            } else if (cmd == "get-params") {
                Parameters params = radar.getParameters();

                printf("Min distance:\t%f\n", params.minDistance.inFloat);
                printf("Max distance:\t%f\n", params.maxDistance.inFloat);
                printf("Min speed:\t\t%f\n", params.minSpeed.inFloat);
                printf("Max speed:\t\t%f\n", params.maxSpeed.inFloat);
                printf("Min angle:\t\t%f\n", params.minAngle.inFloat);
                printf("Max angle:\t\t%f\n", params.maxAngle.inFloat);
                printf("Left border:\t%f\n", params.leftBorder.inFloat);
                printf("Right border:\t%f\n", params.rightBorder.inFloat);
            } else if (cmd == "get-targets") {
                TargetData targetData[35];
                shouldStopReading = false;

                int lineCount = 10;

                system("cls");

                printf(" # |  dist  |   speed  |  angle  \n");
                printf("---------------------------------\n");

                while (!shouldStopReading) {
                    radar.getTargetData(targetData);
                    for (int i = 0; i < lineCount; ++i) {
                        printf("\r%2d | %2.2f m | %2.2f m/s | %2.2f deg\n",
                               targetData[i].targetNum,
                               targetData[i].distance,
                               targetData[i].speed,
                               targetData[i].angle);

                        targetData[i] = TargetData{};
                    }

                    printf("\x1b[10A");
                }
                printf("\n");
                system("cls");
            } else if (cmd == "disable-zerodata") {
                radar.setZeroDataReporting(ZERO_DATA_NOT_REPORT);
            } else if (cmd == "enable-zerodata") {
                radar.setZeroDataReporting(ZERO_DATA_REPORT);
            } else if (cmd == "?" || cmd == "help") {
                usage();
            } else {
                usage();
            }
        }
    }

};