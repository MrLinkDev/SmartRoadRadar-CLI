#include "serial.h"

LPTSTR address = nullptr;
PortConfig config;

Serial serial;

void write_thread();
void read_thread();

int main() {
    address = LPTSTR("COM3");

    serial = Serial(address, config);
}