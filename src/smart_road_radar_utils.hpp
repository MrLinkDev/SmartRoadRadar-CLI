#ifndef SMART_ROAD_SMART_ROAD_RADAR_UTILS_HPP
#define SMART_ROAD_SMART_ROAD_RADAR_UTILS_HPP

#include "serial.hpp"

#define SMART_ROAD_RADAR_OK     0
#define SMART_ROAD_RADAR_ERROR  1

#define SUCCESS     0x0A
#define FAILURE     0xFF

#define CMD_REQUEST_VERSION     0x10
#define CMD_READ_VERSION        0x11

#define CMD_SET_PARAMETERS      0x36
#define CMD_GET_PARAMETERS      0x37

#define CMD_READ_PARAMETERS     0x38

#define CMD_SET_TARGET_NUM      0x12

#define CMD_READ_STATUS         0x50

#define CMD_READ_TARGET_DATA    0x42

#define CMD_ENABLE_TRANSMIT     0x20
#define CMD_DISABLE_TRANSMIT    0x21

#define CMD_SET_DATA_FREQ       0x18

#define CMD_SET_ZERO_REPORT     0x19

#define HEADER_DATA_FRAME_1     0x55
#define HEADER_DATA_FRAME_2     0xAA

#define LENGTH_HEADER           2
#define LENGTH_DATA_LENGTH      2
#define LENGTH_COMMAND_WORD     1
#define LENGTH_CHECKSUM         1

#define MIN_DISTANCE    0.0f
#define MAX_DISTANCE    13.0f

#define MIN_SPEED       0.0f
#define MAX_SPEED       5.0f

#define MIN_ANGLE      -60.0f
#define MAX_ANGLE       60.0f

#define LEFT_BORDER    -6.0f
#define RIGHT_BORDER    6.0f

#define SCALE   0.01f

#define TARGET_DATA_BYTE_LENGTH 9

#define DATA_FREQ_1     0x01
#define DATA_FREQ_2     0x02
#define DATA_FREQ_3     0x03
#define DATA_FREQ_4     0x04
#define DATA_FREQ_5     0x05
#define DATA_FREQ_10    0x0A
#define DATA_FREQ_15    0x0F
#define DATA_FREQ_20    0x14

#define ZERO_DATA_REPORT        0x0A
#define ZERO_DATA_NOT_REPORT    0xFF

struct frame {
    bool is_valid = false;

    u_byte_t header[2] = {HEADER_DATA_FRAME_1, HEADER_DATA_FRAME_2};

    union {
        u_byte_t b[2];
        u_short_t s;
    } data_length{};

    u_byte_t word{};
    u_byte_t *data = nullptr;
    u_byte_t checksum{};
};

struct parameters {
    union {
        float f = MIN_DISTANCE;
        u_byte_t b[4];
    } min_dist;

    union {
        float f = MAX_DISTANCE;
        u_byte_t b[4];
    } max_dist;

    union {
        float f = MIN_SPEED;
        u_byte_t b[4];
    } min_speed;

    union {
        float f = MAX_SPEED;
        u_byte_t b[4];
    } max_speed;

    union {
        float f = MIN_ANGLE;
        u_byte_t b[4];
    } min_angle;

    union {
        float f = MAX_ANGLE;
        u_byte_t b[4];
    } max_angle;

    union {
        float f = LEFT_BORDER;
        u_byte_t b[4];
    } left_border;

    union {
        float f = RIGHT_BORDER;
        u_byte_t b[4];
    } right_border;
};

struct target_data {
    u_byte_t num{};

    float distance{};
    float speed{};
    float angle{};
    float snr{};
};

u_byte_t calculate_checksum(frame target_frame) {
    u_byte_t checksum = 0x00;

    checksum += target_frame.data_length.b[0];
    checksum += target_frame.data_length.b[1];

    checksum += target_frame.word;

    if (target_frame.data_length.s > 1) {
        for (unsigned short i = 0; i < target_frame.data_length.s - 1; ++i) {
            checksum += target_frame.data[i];
        }
    }

    return checksum;
}

float u_byte_to_float(const u_byte_t u_bytes[2]) {
    return float((int) u_bytes[0] + (int) u_bytes[1]) * SCALE;
}

frame configure_frame(u_byte_t word) {
    frame configured_frame{};

    configured_frame.data_length.s = (u_short_t) 1;
    configured_frame.word = word;

    configured_frame.checksum = calculate_checksum(configured_frame);

    return configured_frame;
}

frame configure_frame(u_byte_t word, u_byte_t data) {
    frame configured_frame{};

    configured_frame.data_length.s = (u_short_t) 2;
    configured_frame.word = word;

    configured_frame.data = new u_byte_t[1];
    configured_frame.data[0] = data;

    configured_frame.checksum = calculate_checksum(configured_frame);

    return configured_frame;
}

frame configure_frame(u_byte_t word, u_byte_t *data, u_short_t length) {
    frame configured_frame{};

    configured_frame.data_length.s = (u_short_t) (length + 1);
    configured_frame.word = word;

    configured_frame.data = new u_byte_t[length];
    configured_frame.data = data;

    configured_frame.checksum = calculate_checksum(configured_frame);

    return configured_frame;
}

#endif //SMART_ROAD_SMART_ROAD_RADAR_UTILS_HPP
