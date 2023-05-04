#ifndef SMART_ROAD_SERIAL_HPP
#define SMART_ROAD_SERIAL_HPP

#include <windows.h>
#include <iostream>

#include "utils.hpp"

#define BYTE_SIZE 8

#define SYMBOL_NULL '\0'
#define SYMBOL_CR   '\r'
#define SYMBOL_LF   '\n'

#define SERIAL_OK       0
#define SERIAL_ERROR    1

struct port_config {
    DWORD baud_rate;
    DWORD byte_size;
    DWORD stop_bits;
    DWORD parity;
};

class Serial {

private:
    HANDLE h_serial;

public:
    Serial() = default;

    explicit Serial(LPTSTR address) {
        h_serial = ::CreateFile(
                address,
                GENERIC_READ | GENERIC_WRITE,
                0,
                nullptr,
                OPEN_EXISTING,
                FILE_ATTRIBUTE_NORMAL,
                nullptr);

        if (h_serial == INVALID_HANDLE_VALUE) {
            printf("Can't open port %s\n", address);
            return;
        }

        DCB dcbSerialParameters = {0};

        dcbSerialParameters.DCBlength = sizeof dcbSerialParameters;
        dcbSerialParameters.BaudRate = BAUD_115200;
        dcbSerialParameters.ByteSize = BYTE_SIZE;
        dcbSerialParameters.StopBits = ONESTOPBIT;
        dcbSerialParameters.Parity = NOPARITY;

        SetCommState(h_serial, &dcbSerialParameters);
    }

    Serial(LPTSTR address, port_config config) {
        h_serial = ::CreateFile(
                address,
                GENERIC_READ | GENERIC_WRITE,
                0,
                nullptr,
                OPEN_EXISTING,
                FILE_ATTRIBUTE_NORMAL,
                nullptr);

        if (h_serial == INVALID_HANDLE_VALUE) {
            printf("Can't open port %s\n", address);
            return;
        }

        DCB dcbSerialParameters = {0};

        dcbSerialParameters.DCBlength = sizeof dcbSerialParameters;
        dcbSerialParameters.BaudRate = config.baud_rate;
        dcbSerialParameters.ByteSize = config.byte_size;
        dcbSerialParameters.StopBits = config.stop_bits;
        dcbSerialParameters.Parity = config.parity;

        SetCommState(h_serial, &dcbSerialParameters);
    }

    int write_byte(byte_t data) {
        DWORD dw_size = 1;
        DWORD dw_bytes_written;

        WriteFile(
                h_serial,
                &data,
                dw_size,
                &dw_bytes_written,
                nullptr);

        if (dw_size == dw_bytes_written) {
            return SERIAL_OK;
        } else {
            return SERIAL_ERROR;
        }
    }

    int write_bytes(byte_t *data, size_t length) {
        DWORD dw_bytes_written;

        WriteFile(
                h_serial,
                data,
                length,
                &dw_bytes_written,
                nullptr);

        if (length == dw_bytes_written) {
            return SERIAL_OK;
        } else {
            return SERIAL_ERROR;
        }
    }

    int write_u_byte(u_byte_t data) {
        DWORD dw_size = 1;
        DWORD dw_bytes_written;

        WriteFile(
                h_serial,
                &data,
                dw_size,
                &dw_bytes_written,
                nullptr);

        if (dw_size == dw_bytes_written) {
            return SERIAL_OK;
        } else {
            return SERIAL_ERROR;
        }
    }

    int write_u_bytes(u_byte_t *data, size_t length) {
        DWORD dw_bytes_written;

        WriteFile(
                h_serial,
                data,
                length,
                &dw_bytes_written,
                nullptr);

        if (length == dw_bytes_written) {
            return SERIAL_OK;
        } else {
            return SERIAL_ERROR;
        }
    }

    byte_t read_byte() {
        DWORD size;
        byte_t received_byte;

        ReadFile(
                h_serial,
                &received_byte,
                1,
                &size,
                nullptr);

        if (size > 0) {
            return received_byte;
        }
    }

    void read_bytes(byte_t *buffer, size_t buffer_length) {
        byte_t received_byte;

        for (size_t pos = 0; pos < buffer_length; ++pos) {
            received_byte = read_byte();
            buffer[pos] = received_byte;
        }
    }

    u_byte_t read_u_byte() {
        DWORD size;
        u_byte_t received_byte;

        ReadFile(
                h_serial,
                &received_byte,
                1,
                &size,
                nullptr);

        if (size > 0) {
            return received_byte;
        }
    }

    void read_u_bytes(u_byte_t *buffer, size_t buffer_length) {
        u_byte_t received_byte;

        for (size_t pos = 0; pos < buffer_length; ++pos) {
            received_byte = read_byte();
            buffer[pos] = received_byte;
        }
    }

};


#endif //SMART_ROAD_SERIAL_HPP
