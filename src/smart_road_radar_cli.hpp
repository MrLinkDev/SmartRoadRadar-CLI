#ifndef SMART_ROAD_SMART_ROAD_RADAR_CLI_HPP
#define SMART_ROAD_SMART_ROAD_RADAR_CLI_HPP

#include "smart_road_radar.hpp"

#define CLI_VERSION                 "version"
#define CLI_VERSION_SHORT           "-v"

#define CLI_SET_PARAMS              "set-params"
#define CLI_SET_PARAMS_SHORT        "-sp"

#define CLI_GET_PARAMS              "get-params"
#define CLI_GET_PARAMS_SHORT        "-gp"

#define CLI_SET_TARGET_NUM          "target-num"
#define CLI_SET_TARGET_NUM_SHORT    "-t"

#define CLI_GET_TARGET_DATA         "get-targets"
#define CLI_GET_TARGET_DATA_SHORT   "-gt"

#define CLI_ENABLE_TRANSMIT         "enable"
#define CLI_ENABLE_TRANSMIT_SHORT   "-e"

#define CLI_DISABLE_TRANSMIT        "disable"
#define CLI_DISABLE_TRANSMIT_SHORT  "-d"

#define CLI_SET_DATA_FREQ           "freq"
#define CLI_SET_DATA_FREQ_SHORT     "-f"

#define CLI_ENABLE_ZERO_DATA        "enable-zero"
#define CLI_ENABLE_ZERO_DATA_SHORT  "-ez"

#define CLI_DISABLE_ZERO_DATA       "disable-zero"
#define CLI_DISABLE_ZERO_DATA_SHORT "-dz"

#define DELIMITER   " "

class SmartRoadRadarCLI {

private:
    SmartRoadRadar radar{};

    bool exit_from_main_loop = false;
    bool exit_from_target_data = false;

    std::string get_first_item(std::string *line) {
        std::string item{};
        std::string delimiter = DELIMITER;


        if (line->find(delimiter) == std::string::npos) {
            item = *line;
            line->erase(0, line->length());
        } else {
            item = line->substr(0, line->find(delimiter));
            line->erase(0, line->find(delimiter) + delimiter.length());
        }

        return item;
    }

    void parse_line(std::string *line) {
        if (line->length() <= 1) {
            usage();
            return;
        }

        std::string cmd = get_first_item(line);

        if (cmd == CLI_VERSION || cmd == CLI_VERSION_SHORT) {
            if (line->length() == 0)
                get_version();
            else
                usage();
        } else if (cmd == CLI_SET_PARAMS || cmd == CLI_SET_PARAMS_SHORT) {
            if (line->length() > 0)
                set_params(line);
            else
                usage();
        } else if (cmd == CLI_GET_PARAMS || cmd == CLI_GET_PARAMS_SHORT) {
            if (line->length() == 0)
                get_params();
            else
                usage();
        } else if (cmd == CLI_GET_TARGET_DATA || cmd == CLI_GET_TARGET_DATA_SHORT) {
            if (line->length() == 0)
                get_targets();
            else
                usage();
        }
    }

    void usage() {
        printf("Usage was here...\n");
        exit_from_main_loop = true;
    }

    void get_version() {
        u_byte_t version_buffer[3];

        if (radar.get_firmware_version(version_buffer) == SMART_ROAD_RADAR_OK) {
            printf("Firmware version: V%d.%d.%d\n",
                    (int) version_buffer[0],
                    (int) version_buffer[1],
                    (int) version_buffer[2]);
        } else {
            printf("Can't get firmware version from radar\n");
        }
    }

    void set_params(std::string *args) {
        parameters params;

        if (*args != "default") {
            params.min_dist.f       = std::stof(get_first_item(args));
            params.max_dist.f       = std::stof(get_first_item(args));
            params.min_speed.f      = std::stof(get_first_item(args));
            params.max_speed.f      = std::stof(get_first_item(args));
            params.min_angle.f      = std::stof(get_first_item(args));
            params.max_angle.f      = std::stof(get_first_item(args));
            params.left_border.f    = std::stof(get_first_item(args));
            params.right_border.f   = std::stof(get_first_item(args));
        }

        if (radar.set_parameters(params) == SMART_ROAD_RADAR_OK) {
            printf("Parameters loaded\n");
        } else {
            printf("Error\n");
        }
    }

    void get_params() {
        parameters params;

        if (radar.get_parameters(&params) == SMART_ROAD_RADAR_OK) {
            printf("Min distance = %f\n", params.min_dist.f);
            printf("Max distance = %f\n", params.max_dist.f);
            printf("Min speed = %f\n", params.min_speed.f);
            printf("Max speed = %f\n", params.max_speed.f);
            printf("Min angle = %f\n", params.min_angle.f);
            printf("max angle = %f\n", params.max_angle.f);
            printf("Left border = %f\n", params.left_border.f);
            printf("Right border = %f\n", params.right_border.f);
        } else {
            printf("Error\n");
        }
    }

    void get_targets() {
        target_data data[35];

        system("cls");

        printf(" # |  dist  |   speed  |  angle  \n");
        printf("---------------------------------\n");

        while (true) {
            if (radar.get_target_data(data) == SMART_ROAD_RADAR_OK) {

                for (int i = 0; i < 10; ++i) {
                    printf("\r%2d | %2.2f m | %2.2f m/s | %2.2f deg\n",
                           data[i].num,
                           data[i].distance,
                           data[i].speed,
                           data[i].angle);

                    data[i] = target_data{};
                }

                printf("\x1b[10A");
            }
        }
    }

public:
    SmartRoadRadarCLI() = default;

    explicit SmartRoadRadarCLI(LPTSTR address) {
        radar = SmartRoadRadar(address);
    }

    SmartRoadRadarCLI(LPTSTR address, port_config config) {
        radar = SmartRoadRadar(address, config);
    }

    void main_loop() {
        std::string line;
        exit_from_main_loop = false;

        while (!exit_from_main_loop) {
            line = std::string{};
            std::getline(std::cin, line);

            parse_line(&line);
        }
    }
};


#endif //SMART_ROAD_SMART_ROAD_RADAR_CLI_HPP
