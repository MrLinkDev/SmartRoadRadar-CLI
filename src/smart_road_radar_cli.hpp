#ifndef SMART_ROAD_SMART_ROAD_RADAR_CLI_HPP
#define SMART_ROAD_SMART_ROAD_RADAR_CLI_HPP

#include <thread>
#include <conio.h>

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

#define CLI_HELP                    "help"
#define CLI_HELP_SHORT              "?"

#define CLI_EXIT                    "exit"

#define ESCAPE_CHAR 27

class SmartRoadRadarCLI {

protected:
    inline static bool exit_from_target_data;

    static void wait_exc_char() {
        while ((int) getch() != ESCAPE_CHAR);

        SmartRoadRadarCLI::exit_from_target_data = true;
    }

private:
    SmartRoadRadar radar{};

    bool exit_from_main_loop = false;

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
                set_parameters(line);
            else
                usage();
        } else if (cmd == CLI_GET_PARAMS || cmd == CLI_GET_PARAMS_SHORT) {
            if (line->length() == 0)
                get_parameters();
            else
                usage();
        } else if (cmd == CLI_SET_TARGET_NUM || cmd == CLI_SET_TARGET_NUM_SHORT) {
            if (line->length() > 0)
                set_target_num(*line);
            else
                usage();
        } else if (cmd == CLI_GET_TARGET_DATA || cmd == CLI_GET_TARGET_DATA_SHORT) {
            if (line->length() >= 0)
                get_target_data(*line);
            else
                usage();
        } else if (cmd == CLI_ENABLE_TRANSMIT || cmd == CLI_ENABLE_TRANSMIT_SHORT) {
            if (line->length() == 0)
                enable_data_transmit();
            else
                usage();
        } else if (cmd == CLI_DISABLE_TRANSMIT || cmd == CLI_DISABLE_TRANSMIT_SHORT) {
            if (line->length() == 0)
                disable_data_transmit();
            else
                usage();
        } else if (cmd == CLI_SET_DATA_FREQ || cmd == CLI_SET_DATA_FREQ_SHORT) {
            if (line->length() > 0)
                set_data_freq(*line);
            else
                usage();
        } else if (cmd == CLI_ENABLE_ZERO_DATA || cmd == CLI_ENABLE_ZERO_DATA_SHORT) {
            if (line->length() == 0)
                enable_zero_data_transmit();
            else
                usage();
        } else if (cmd == CLI_DISABLE_ZERO_DATA || cmd == CLI_DISABLE_ZERO_DATA_SHORT) {
            if (line->length() == 0)
                disable_zero_data_transmit();
            else
                usage();
        } else if (cmd == CLI_HELP || cmd == CLI_HELP_SHORT) {
            usage();
        } else if (cmd == CLI_EXIT) {
            exit_from_main_loop = true;

            printf("Leaving...");
        } else {
            usage();
        }
    }

    void usage() {
        printf("\nSmartRoad radar Communication CLI\n\n");
        printf("Usage:\n");
        printf("\tversion      ( -v) -- shows firmware version of radar.\n\n");
        printf("\tset-params   (-sp) -- uploading default or custom configuration into radar.\n"
               "\tset-params   (-sp) default\n"
               "\tset-params   (-sp) [min_dist max_dist min_speed max_speed\n"
               "\t                    min_angle max_angle left_border right_border]\n\n");
        printf("\tget-params   (-gp) -- shows current radar configuration.\n\n");
        printf("\ttarget-num   ( -t) -- uploading number of targets.\n");
        printf("\ttarget-num   ( -t) [num]\n\n");
        printf("\tget-targets  (-gt) -- shows information about visible targets. Press esc to exit.\n");
        printf("\tget-targets  (-gt)\n");
        printf("\tget-targets  (-gt) [target_num]\n\n");
        printf("\tenable       ( -e) -- starts data transmit.\n");
        printf("\tdisable      ( -d) -- stops data transmit.\n\n");
        printf("\tfreq         ( -f) -- uploading data frequency.\n");
        printf("\tfreq         ( -f) [data_freq]\n\n");
        printf("\tenable-zero  (-ez) -- enable zero data reporting.\n");
        printf("\tdisable-zero (-dz) -- disable zero data reporting.\n\n");
        printf("\thelp         ( ? ) -- shows this usage.\n");
        printf("\texit               -- program closure.\n\n");
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

    void set_parameters(std::string *args) {
        parameters target_parameters;

        if (*args != "default") {
            target_parameters.min_dist.f     = std::stof(get_first_item(args));
            target_parameters.max_dist.f     = std::stof(get_first_item(args));
            target_parameters.min_speed.f    = std::stof(get_first_item(args));
            target_parameters.max_speed.f    = std::stof(get_first_item(args));
            target_parameters.min_angle.f    = std::stof(get_first_item(args));
            target_parameters.max_angle.f    = std::stof(get_first_item(args));
            target_parameters.left_border.f  = std::stof(get_first_item(args));
            target_parameters.right_border.f = std::stof(get_first_item(args));
        }

        if (radar.set_parameters(target_parameters) == SMART_ROAD_RADAR_OK) {
            printf("Parameters are loaded\n");
        } else {
            printf("Can't load parameters into radar\n");
        }
    }

    void get_parameters() {
        parameters received_parameters;

        if (radar.get_parameters(&received_parameters) == SMART_ROAD_RADAR_OK) {
            printf("Min distance = %f\n", received_parameters.min_dist.f);
            printf("Max distance = %f\n", received_parameters.max_dist.f);
            printf("Min speed    = %f\n", received_parameters.min_speed.f);
            printf("Max speed    = %f\n", received_parameters.max_speed.f);
            printf("Min angle    = %f\n", received_parameters.min_angle.f);
            printf("max angle    = %f\n", received_parameters.max_angle.f);
            printf("Left border  = %f\n", received_parameters.left_border.f);
            printf("Right border = %f\n", received_parameters.right_border.f);
        } else {
            printf("Can't get parameters from radar\n");
        }
    }

    void set_target_num(std::string num) {
        u_byte_t target_num = std::stoi(num);

        if (radar.set_target_number(target_num) == SMART_ROAD_RADAR_OK) {
            printf("Target number inserted\n");
        } else {
            printf("Can't insert target number into radar\n");
        }
    }

    void get_target_data(std::string count) {
        int target_count = count.length() == 0 ? 35 : std::stoi(count);
        target_data data[target_count];

        SmartRoadRadarCLI::exit_from_target_data = false;

        system("cls");

        printf(" # |  dist  |   speed  |  angle  \n");
        printf("---------------------------------\n");

        std::thread esc_handler_thread(SmartRoadRadarCLI::wait_exc_char);

        while (!SmartRoadRadarCLI::exit_from_target_data) {
            if (radar.get_target_data(data) == SMART_ROAD_RADAR_OK) {

                for (int i = 0; i < target_count; ++i) {
                    printf("\r%2d | %2.2f m | %2.2f m/s | %2.2f deg\n",
                           data[i].num,
                           data[i].distance,
                           data[i].speed,
                           data[i].angle);

                    data[i] = target_data{};
                }

                printf("\x1b[%dA", target_count);
            }
        }

        esc_handler_thread.join();

        system("cls");
    }

    void enable_data_transmit() {
        if (radar.enable_data_transmit() == SMART_ROAD_RADAR_OK) {
            printf("Data transmit enabled\n");
        } else {
            printf("Can't enable data transmit\n");
        }
    }

    void disable_data_transmit() {
        if (radar.disable_data_transmit() == SMART_ROAD_RADAR_OK) {
            printf("Data transmit disabled\n");
        } else {
            printf("Can't disable data transmit\n");
        }
    }

    void set_data_freq(std::string freq) {
        u_byte_t data_freq = std::stoi(freq);

        if (radar.set_data_transmit_freq(data_freq) == SMART_ROAD_RADAR_OK) {
            printf("Frequency value uploaded into radar\n");
        } else {
            printf("Can't upload frequency value\n");
        }
    }

    void enable_zero_data_transmit() {
        if (radar.enable_zero_data_reporting() == SMART_ROAD_RADAR_OK) {
            printf("Zero data reporting enabled\n");
        } else {
            printf("Can't enable zero data reporting\n");
        }
    }

    void disable_zero_data_transmit() {
        if (radar.disable_zero_data_reporting() == SMART_ROAD_RADAR_OK) {
            printf("Zero data reporting disabled\n");
        } else {
            printf("Can't disable zero data reporting\n");
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
            printf("smart_road_radar:\\> ");

            line = std::string{};
            std::getline(std::cin, line);

            parse_line(&line);
        }
    }
};


#endif //SMART_ROAD_SMART_ROAD_RADAR_CLI_HPP
