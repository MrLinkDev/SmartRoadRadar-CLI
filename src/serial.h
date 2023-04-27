#ifndef SMART_ROAD_SERIAL_H
#define SMART_ROAD_SERIAL_H

#include <windows.h>
#include <iostream>

#define SYMBOL_NULL '\0'
#define SYMBOL_CR   '\r'
#define SYMBOL_LF   '\n'

#define PACKET_LENGTH 256
#define BYTES_TO_READ 1

using namespace std;

struct PortConfig {
    DWORD baudRate;
    DWORD byteSize;
    DWORD stopBits;
    DWORD parity;
};

class Serial {

private:
    HANDLE hSerial;

    void append(char *dest, char symbol);
    void append(unsigned char *dest, unsigned char symbol);

public:
    Serial() = default;
    Serial(LPTSTR address, PortConfig config);

    void write(char *data);
    void write(unsigned char *data);

    void writeLn(char *data);
    void writeLn(unsigned char *data);

    char readByte();
    void readBytes(char *buffer, int bufferLength);

    unsigned char readUnsignedByte();
    void readUnsignedBytes(unsigned char *buffer, int bufferLength);
};


#endif //SMART_ROAD_SERIAL_H
