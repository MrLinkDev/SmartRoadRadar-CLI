#include "serial.h"

LPTSTR address = nullptr;
PortConfig config;

Serial serial;

void write_thread();
void read_thread();

int main() {
    //address = LPTSTR("COM3");

    //config.baudRate = 115200;
    //config.byteSize = 8;
    //config.stopBits = ONESTOPBIT;
    //config.parity = NOPARITY;

    //serial = Serial(address, config);
}