#include "serial.h"


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
}

void Serial::write(char *data) {
    DWORD dwSize = strlen(data);
    DWORD dwBytesWritten;

    WriteFile(hSerial, data, dwSize, &dwBytesWritten, NULL);
}

void Serial::writeLn(char *data) {
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
    for (int pos = 0; pos < bufferLength; ++pos) {
        char receivedByte = readByte();

        buffer[pos] = receivedByte;
    }

    buffer[bufferLength] = SYMBOL_NULL;
}

void Serial::append(char *dest, char symbol) {
    int length = strlen(dest);

    dest[length] = symbol;
    dest[length + 1] = SYMBOL_NULL;
}
