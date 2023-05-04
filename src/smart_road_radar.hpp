#ifndef SMART_ROAD_SMART_ROAD_RADAR_HPP
#define SMART_ROAD_SMART_ROAD_RADAR_HPP

#include "smart_road_radar_utils.hpp"

class SmartRoadRadar {

private:
    Serial data_bus{};

    frame read_frame() {
        frame received_frame{};

        while (data_bus.read_u_byte() != HEADER_DATA_FRAME_1);

        if (data_bus.read_u_byte() != HEADER_DATA_FRAME_2) {
            received_frame.is_valid = false;
            return received_frame;
        }

        received_frame.data_length.b[0] = data_bus.read_u_byte();
        received_frame.data_length.b[1] = data_bus.read_u_byte();

        received_frame.word = data_bus.read_u_byte();

        if (received_frame.data_length.s > 1) {
            received_frame.data = new u_byte_t[received_frame.data_length.s - 1];

            data_bus.read_u_bytes(received_frame.data, received_frame.data_length.s - 1);
        }

        received_frame.checksum = data_bus.read_u_byte();

        u_byte_t calc_checksum = calculate_checksum(received_frame);
        if (calc_checksum == received_frame.checksum) {
            received_frame.is_valid = true;
        } else {
            received_frame.is_valid = false;
        }

        return received_frame;
    }

    frame read_expected_frame(u_byte_t expected_word) {
        frame received_frame;

        do {
            received_frame = read_frame();
        } while (received_frame.word != expected_word);

        return received_frame;
    }

    int read_status() {
        frame received_frame = read_expected_frame(CMD_READ_STATUS);

        if (received_frame.is_valid) {
            if (received_frame.data[0] == SUCCESS) {
                return SMART_ROAD_RADAR_OK;
            } else {
                return SMART_ROAD_RADAR_ERROR;
            }
        } else {
            return SMART_ROAD_RADAR_ERROR;
        }
    }

    int write_frame(frame target_frame) {
        int packet_length =
                LENGTH_HEADER +
                LENGTH_DATA_LENGTH +
                LENGTH_COMMAND_WORD +
                (target_frame.data_length.s - 1) +
                LENGTH_CHECKSUM;

        u_byte_t packet[packet_length];

        packet[0] = target_frame.header[0];
        packet[1] = target_frame.header[1];

        packet[2] = target_frame.data_length.b[0];
        packet[3] = target_frame.data_length.b[1];

        packet[4] = target_frame.word;

        if (target_frame.data != nullptr) {
            for (int i = 5; i < packet_length - 1; ++i) {
                packet[i] = target_frame.data[i - 5];
            }
        }

        packet[packet_length - 1] = target_frame.checksum;

        int result = data_bus.write_u_bytes(packet, packet_length);
        return result;
    }

public:
    SmartRoadRadar() = default;

    explicit SmartRoadRadar(LPTSTR address) {
        port_config config{};

        config.baud_rate = 115200;
        config.byte_size = 8;
        config.stop_bits = ONESTOPBIT;
        config.parity = NOPARITY;

        data_bus = Serial(address, config);
    }

    SmartRoadRadar(LPTSTR address, port_config config) {
        data_bus = Serial(address, config);
    }

    int get_firmware_version(u_byte_t *version_buffer) {
        frame target_frame = configure_frame(CMD_REQUEST_VERSION);
        write_frame(target_frame);

        frame received_frame = read_expected_frame(CMD_READ_VERSION);

        if (received_frame.is_valid) {
            version_buffer[0] = received_frame.data[0];
            version_buffer[1] = received_frame.data[1];
            version_buffer[2] = received_frame.data[2];

            return SMART_ROAD_RADAR_OK;
        } else {
            return SMART_ROAD_RADAR_ERROR;
        }
    }

    int set_parameters(parameters target_parameters) {
        u_short_t length = sizeof target_parameters;
        u_byte_t data[length];

        data[0]  = target_parameters.min_dist.b[0];
        data[1]  = target_parameters.min_dist.b[1];
        data[2]  = target_parameters.min_dist.b[2];
        data[3]  = target_parameters.min_dist.b[3];
 
        data[4]  = target_parameters.max_dist.b[0];
        data[5]  = target_parameters.max_dist.b[1];
        data[6]  = target_parameters.max_dist.b[2];
        data[7]  = target_parameters.max_dist.b[3];
 
        data[8]  = target_parameters.min_speed.b[0];
        data[9]  = target_parameters.min_speed.b[1];
        data[10] = target_parameters.min_speed.b[2];
        data[11] = target_parameters.min_speed.b[3];

        data[12] = target_parameters.max_speed.b[0];
        data[13] = target_parameters.max_speed.b[1];
        data[14] = target_parameters.max_speed.b[2];
        data[15] = target_parameters.max_speed.b[3];

        data[16] = target_parameters.min_angle.b[0];
        data[17] = target_parameters.min_angle.b[1];
        data[18] = target_parameters.min_angle.b[2];
        data[19] = target_parameters.min_angle.b[3];

        data[20] = target_parameters.max_angle.b[0];
        data[21] = target_parameters.max_angle.b[1];
        data[22] = target_parameters.max_angle.b[2];
        data[23] = target_parameters.max_angle.b[3];

        data[24] = target_parameters.left_border.b[0];
        data[25] = target_parameters.left_border.b[1];
        data[26] = target_parameters.left_border.b[2];
        data[27] = target_parameters.left_border.b[3];

        data[28] = target_parameters.right_border.b[0];
        data[29] = target_parameters.right_border.b[1];
        data[30] = target_parameters.right_border.b[2];
        data[31] = target_parameters.right_border.b[3];

        frame target_frame = configure_frame(CMD_SET_PARAMETERS, data, length);
        write_frame(target_frame);

        int result = read_status();
        return result;
    }

    int get_parameters(parameters *received_parameters) {
        frame target_frame = configure_frame(CMD_GET_PARAMETERS);
        write_frame(target_frame);
        
        frame received_frame = read_expected_frame(CMD_READ_PARAMETERS);
        
        if (received_frame.is_valid) {
            received_parameters->min_dist.b[0] = received_frame.data[4];
            received_parameters->min_dist.b[1] = received_frame.data[5];
            received_parameters->min_dist.b[2] = received_frame.data[6];
            received_parameters->min_dist.b[3] = received_frame.data[7];

            received_parameters->max_dist.b[0] = received_frame.data[0];
            received_parameters->max_dist.b[1] = received_frame.data[1];
            received_parameters->max_dist.b[2] = received_frame.data[2];
            received_parameters->max_dist.b[3] = received_frame.data[3];

            received_parameters->min_speed.b[0] = received_frame.data[12];
            received_parameters->min_speed.b[1] = received_frame.data[13];
            received_parameters->min_speed.b[2] = received_frame.data[14];
            received_parameters->min_speed.b[3] = received_frame.data[15];

            received_parameters->max_speed.b[0] = received_frame.data[8];
            received_parameters->max_speed.b[1] = received_frame.data[9];
            received_parameters->max_speed.b[2] = received_frame.data[10];
            received_parameters->max_speed.b[3] = received_frame.data[11];

            received_parameters->min_angle.b[0] = received_frame.data[16];
            received_parameters->min_angle.b[1] = received_frame.data[17];
            received_parameters->min_angle.b[2] = received_frame.data[18];
            received_parameters->min_angle.b[3] = received_frame.data[19];

            received_parameters->max_angle.b[0] = received_frame.data[20];
            received_parameters->max_angle.b[1] = received_frame.data[21];
            received_parameters->max_angle.b[2] = received_frame.data[22];
            received_parameters->max_angle.b[3] = received_frame.data[23];

            received_parameters->left_border.b[0] = received_frame.data[24];
            received_parameters->left_border.b[1] = received_frame.data[25];
            received_parameters->left_border.b[2] = received_frame.data[26];
            received_parameters->left_border.b[3] = received_frame.data[27];

            received_parameters->right_border.b[0] = received_frame.data[28];
            received_parameters->right_border.b[1] = received_frame.data[29];
            received_parameters->right_border.b[2] = received_frame.data[30];
            received_parameters->right_border.b[3] = received_frame.data[31];

            return SMART_ROAD_RADAR_OK;
        } else {
            return SMART_ROAD_RADAR_ERROR;
        }
    }
    
    int set_target_number(u_byte_t number) {
        frame target_frame = configure_frame(CMD_SET_TARGET_NUM, number);
        write_frame(target_frame);
        
        int result = read_status();
        return result;
    }
    
    int get_target_data(target_data *data) {
        frame received_frame;
        
        do {
            received_frame = read_expected_frame(CMD_READ_TARGET_DATA);
        } while (received_frame.data_length.s <= 1);
        
        if (received_frame.is_valid) {
            int target_count = (received_frame.data_length.s - 1) / TARGET_DATA_BYTE_LENGTH;

            for (int pos = 0; pos < target_count; ++pos) {
                data[pos].num = received_frame.data[0 + TARGET_DATA_BYTE_LENGTH * pos];

                data[pos].distance = u_byte_to_float(new u_byte_t[2] {
                        received_frame.data[1 + TARGET_DATA_BYTE_LENGTH * pos],
                        received_frame.data[2 + TARGET_DATA_BYTE_LENGTH * pos]
                });

                data[pos].speed = u_byte_to_float(new u_byte_t[2] {
                        received_frame.data[3 + TARGET_DATA_BYTE_LENGTH * pos],
                        received_frame.data[4 + TARGET_DATA_BYTE_LENGTH * pos]
                });

                data[pos].angle = u_byte_to_float(new u_byte_t[2] {
                        received_frame.data[5 + TARGET_DATA_BYTE_LENGTH * pos],
                        received_frame.data[6 + TARGET_DATA_BYTE_LENGTH * pos]
                });

                data[pos].snr =u_byte_to_float(new u_byte_t[2] {
                        received_frame.data[7 + TARGET_DATA_BYTE_LENGTH * pos],
                        received_frame.data[8 + TARGET_DATA_BYTE_LENGTH * pos]
                });
            }

            return SMART_ROAD_RADAR_OK;
        } else {
            return SMART_ROAD_RADAR_ERROR;
        }
    }

    int enable_data_transmit() {
        frame target_frame = configure_frame(CMD_ENABLE_TRANSMIT);
        write_frame(target_frame);

        int result = read_status();
        return result;
    }

    int disable_data_transmit() {
        frame target_frame = configure_frame(CMD_DISABLE_TRANSMIT);
        write_frame(target_frame);

        int result = read_status();
        return result;
    }

    int set_data_transmit_freq(u_byte_t freq) {
        frame target_frame = configure_frame(CMD_SET_DATA_FREQ, freq);
        write_frame(target_frame);

        int result = read_status();
        return result;
    }

    int enable_zero_data_reporting() {
        frame target_frame = configure_frame(CMD_SET_ZERO_REPORT, ZERO_DATA_REPORT);
        write_frame(target_frame);

        int result = read_status();
        return result;
    }

    int disable_zero_data_reporting() {
        frame target_frame = configure_frame(CMD_SET_ZERO_REPORT, ZERO_DATA_NOT_REPORT);
        write_frame(target_frame);

        int result = read_status();
        return result;
    }
};


#endif //SMART_ROAD_SMART_ROAD_RADAR_HPP
