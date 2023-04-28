#include "smart_road_radar.h"

SmartRoadRadar::SmartRoadRadar(char *address) {
    PortConfig config{};

    config.baudRate = 115200;
    config.byteSize = 8;
    config.stopBits = ONESTOPBIT;
    config.parity = NOPARITY;

    dataBus = Serial(address, config);
}

SmartRoadRadar::SmartRoadRadar(char *address, PortConfig config) {
    dataBus = Serial(address, config);
}

Frame SmartRoadRadar::readFrame() {
    Frame receivedFrame{};

    // Reading Frame headers
    while (dataBus.readUnsignedByte() != HEADER_DATA_FRAME_1);

    if (dataBus.readUnsignedByte() != HEADER_DATA_FRAME_2) {
        receivedFrame.isValid = false;

        return receivedFrame;
    }

    // Reading data length
    receivedFrame.dataLength.inByte[0] = dataBus.readUnsignedByte();
    receivedFrame.dataLength.inByte[1] = dataBus.readUnsignedByte();

    // Reading command word
    receivedFrame.commandWord = dataBus.readUnsignedByte();

    // Reading data
    if (receivedFrame.dataLength.inShort > 1) {
        receivedFrame.data = new byte_t[receivedFrame.dataLength.inShort - 1];
        dataBus.readUnsignedBytes(receivedFrame.data, receivedFrame.dataLength.inShort - 1);
    }

    // Reading checksum
    receivedFrame.checksum = dataBus.readUnsignedByte();

    byte_t calculatedChecksum = calculateChecksum(receivedFrame);

    if (calculatedChecksum == receivedFrame.checksum) {
        receivedFrame.isValid = true;
    } else {
        receivedFrame.isValid = false;
    }

    return receivedFrame;
}

void SmartRoadRadar::writeFrame(Frame frame) {
    isPaused = true;

    int frameLength =
            LENGTH_HEADER +
            LENGTH_DATA_LENGTH +
            LENGTH_COMMAND_WORD +
            (frame.dataLength.inShort - 1) +
            LENGTH_CHECKSUM;

    byte_t packet[frameLength];

    packet[0] = frame.header[0];
    packet[1] = frame.header[1];

    packet[2] = frame.dataLength.inByte[0];
    packet[3] = frame.dataLength.inByte[1];

    packet[4] = frame.commandWord;

    if (frame.data != nullptr) {
        for (int i = 5; i < frameLength - 1; ++i) {
            packet[i] = frame.data[i - 5];
        }
    }

    packet[frameLength - 1] = frame.checksum;

    dataBus.write(packet);

    isPaused = false;
}

Frame SmartRoadRadar::configureFrame(byte_t cmd, byte_t *data, unsigned short length) {
    Frame frame{};

    frame.dataLength.inShort = length + 1;
    frame.commandWord = cmd;

    frame.data = new unsigned char[length];
    frame.data = data;

    frame.checksum = calculateChecksum(frame);

    return frame;
}

Frame SmartRoadRadar::configureFrame(byte_t cmd, byte_t data) {
    Frame frame{};

    frame.dataLength.inShort = 2;
    frame.commandWord = cmd;

    frame.data = new byte_t[1];
    frame.data[0] = data;

    frame.checksum = calculateChecksum(frame);

    return frame;
}

Frame SmartRoadRadar::configureFrame(byte_t cmd) {
    Frame frame{};

    frame.dataLength.inShort = 1;
    frame.commandWord = cmd;

    frame.checksum = calculateChecksum(frame);

    return frame;
}

void SmartRoadRadar::getFirmwareVersion() {
    Frame frame = configureFrame(CMD_REQUEST_VERSION);
    writeFrame(frame);

    cout << "sent" << endl;
}

void SmartRoadRadar::enableDataTransmit() {
    Frame frame = configureFrame(CMD_ENABLE_TRANSMIT);
    writeFrame(frame);
}

void SmartRoadRadar::disableDataTransmit() {
    Frame frame = configureFrame(CMD_DISABLE_TRANSMIT);
    writeFrame(frame);
}

void SmartRoadRadar::start() {
    isReadingInputStream = true;
    inputStreamThread = thread(&SmartRoadRadar::readInputStream, this);
}

void SmartRoadRadar::stop() {
    inputStreamThread.join();
}

void SmartRoadRadar::readInputStream() {
    Frame frame;

    while (isReadingInputStream) {
        while (isPaused) {
            std::this_thread::sleep_for(chrono::milliseconds(250));
        }
        frame = readFrame();

        if (frame.dataLength.inShort == 1 && frame.commandWord == CMD_READ_TARGET_DATA) {
            printf("Zero data\n");
        }

        if (frame.commandWord == CMD_READ_VERSION) {
            if (frame.isValid) {
                unsigned int v_major = frame.data[0];
                unsigned int v_minor = frame.data[1];
                unsigned int v_patch = frame.data[2];

                printf("V%d.%d.%d", v_major, v_minor, v_patch);
            }
        }

        if (frame.commandWord == CMD_READ_STATUS) {
            if (frame.data[0] == SUCCESS) {
                cout << "OK" << endl;
            } else {
                cout << "ERROR" << endl;
            }
        }
    }
}

byte_t calculateChecksum(Frame frame) {
    unsigned char checksum = 0x00;

    checksum += frame.dataLength.inByte[0];
    checksum += frame.dataLength.inByte[1];

    checksum += frame.commandWord;

    if (frame.dataLength.inShort > 1) {
        for (unsigned short i = 0; i < frame.dataLength.inShort; ++i) {
            checksum += frame.data[i];
        }
    }

    return checksum;
}

float dataByteToFloat(byte_t dataByte1, byte_t dataByte2) {
    float value = 0.0f;

    union {
        unsigned short inShort{};
        unsigned char inByte[2];
    } rawData;

    rawData.inByte[0] = dataByte1;
    rawData.inByte[1] = dataByte2;

    value = float(rawData.inShort) * SCALE;

    return value;
}
