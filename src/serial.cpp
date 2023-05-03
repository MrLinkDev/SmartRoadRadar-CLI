#include "serial.h"

Serial::Serial(LPTSTR address) {
    hSerial = ::CreateFile(
            address,
            GENERIC_READ | GENERIC_WRITE,
            0,
            0,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            0);

    if (hSerial == INVALID_HANDLE_VALUE) {
        printf("Can't open port %s\n", address);
        return;
    }

    DCB dcbSerialParameters = {0};

    dcbSerialParameters.DCBlength = sizeof dcbSerialParameters;
    dcbSerialParameters.BaudRate = BAUD_115200;
    dcbSerialParameters.ByteSize = 8;
    dcbSerialParameters.StopBits = ONESTOPBIT;
    dcbSerialParameters.Parity = NOPARITY;
}

Serial::Serial(LPTSTR address, PortConfig config) {
    hSerial = ::CreateFile(
            address,
            GENERIC_READ | GENERIC_WRITE,
            0,
            0,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            0);

    if (hSerial == INVALID_HANDLE_VALUE) {
        printf("Can't open port %s\n", address);
        return;
    }

    DCB dcbSerialParameters = {0};

    dcbSerialParameters.DCBlength = sizeof dcbSerialParameters;
    dcbSerialParameters.BaudRate = config.baudRate;
    dcbSerialParameters.ByteSize = config.byteSize;
    dcbSerialParameters.StopBits = config.stopBits;
    dcbSerialParameters.Parity = config.parity;

    SetCommState(hSerial, &dcbSerialParameters);
}

void Serial::write(char *data) {
    DWORD dwSize = sizeof data;
    DWORD dwBytesWritten;

    WriteFile(hSerial, data, dwSize, &dwBytesWritten, nullptr);
}

void Serial::write(unsigned char *data) {
    DWORD dwSize = sizeof data;
    DWORD dwBytesWritten;

    //for (int i = 0; i < dwSize; ++i) {
    //    printf("%02X ", data[i]);
    //}
    //printf("\n");

    WriteFile(hSerial, data, dwSize, &dwBytesWritten, nullptr);
}

void Serial::write(unsigned char *data, size_t length) {
    DWORD dwSize = length;
    DWORD dwBytesWritten;

    //for (int i = 0; i < dwSize; ++i) {
    //    printf("%02X ", data[i]);
    //}
    //printf("\n");

    WriteFile(hSerial, data, dwSize, &dwBytesWritten, nullptr);
}

void Serial::writeLn(char *data) {
    append(data, SYMBOL_LF);
    write(data);
}

void Serial::writeLn(unsigned char *data) {
    append(data, SYMBOL_LF);
    write(data);
}

char Serial::readByte() {
    DWORD iSize;
    char receivedByte;

    ReadFile(hSerial, &receivedByte, BYTES_TO_READ, &iSize, 0);
    if (iSize > 0) {
        return receivedByte;
    }
}

void Serial::readBytes(char *buffer, int bufferLength) {
    char receivedByte{};

    for (int pos = 0; pos < bufferLength; ++pos) {
        receivedByte = readByte();
        buffer[pos] = receivedByte;
    }

    buffer[bufferLength] = SYMBOL_NULL;
}

unsigned char Serial::readUnsignedByte() {
    char signedByte = readByte();
    unsigned char unsignedByte = static_cast<unsigned char>(signedByte);

    //printf("%02X ", unsignedByte);

    return unsignedByte;
}

void Serial::readUnsignedBytes(unsigned char *buffer, int bufferLength) {
    unsigned char receivedByte;

    for (int pos = 0; pos < bufferLength; ++pos) {
        receivedByte = readUnsignedByte();
        buffer[pos] = receivedByte;
    }

    buffer[bufferLength] = SYMBOL_NULL;
}

void Serial::append(char *dest, char symbol) {
    int length = strlen(dest);

    dest[length] = symbol;
    dest[length + 1] = SYMBOL_NULL;
}

void Serial::append(unsigned char *dest, unsigned char symbol) {
    size_t length = sizeof dest;

    dest[length] = symbol;
    dest[length + 1] = SYMBOL_NULL;
}
