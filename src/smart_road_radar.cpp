#include "smart_road_radar.h"

SmartRoadRadar::SmartRoadRadar(char *address){
    PortConfig config{};

    config.baudRate = 115200;
    config.byteSize = 8;
    config.stopBits = ONESTOPBIT;
    config.parity = NOPARITY;

    dataBus = Serial(address, config);
}

SmartRoadRadar::SmartRoadRadar(char *address, PortConfig config){
    dataBus = Serial(address, config);
}

Frame SmartRoadRadar::readFrame(byte_t expectedCmd) {
    Frame receivedFrame{};

    do {
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
        printf("command = %02X\n", receivedFrame.commandWord);
    } while (receivedFrame.commandWord != expectedCmd);

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

    /**for (int i = 0; i < frameLength; ++i) {
        printf("%02X ", packet[i]);
    }
    printf("\n");*/

    dataBus.write(packet);
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

byte_t SmartRoadRadar::calculateChecksum(Frame frame) {
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

float SmartRoadRadar::dataByteToFloat(byte_t dataByte1, byte_t dataByte2) {
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

void SmartRoadRadar::getFirmwareVersion(char *version) {
    Frame frame = configureFrame(CMD_REQUEST_VERSION);
    writeFrame(frame);

    Frame receivedFrame = readFrame(CMD_READ_VERSION);

    if (receivedFrame.isValid) {
        unsigned int v_major = receivedFrame.data[0];
        unsigned int v_minor = receivedFrame.data[1];
        unsigned int v_patch = receivedFrame.data[2];

        version = new char[32];

        sprintf(version, "V%d.%d.%d", v_major, v_minor, v_patch);
    }
}

byte_t SmartRoadRadar::setParameters(Parameters parameters) {
    unsigned short length = sizeof parameters;
    byte_t data[length];

    data[0] = parameters.minDistance.inByte[0];
    data[1] = parameters.minDistance.inByte[1];
    data[2] = parameters.minDistance.inByte[2];
    data[3] = parameters.minDistance.inByte[3];

    data[4] = parameters.maxDistance.inByte[0];
    data[5] = parameters.maxDistance.inByte[1];
    data[6] = parameters.maxDistance.inByte[2];
    data[7] = parameters.maxDistance.inByte[3];

    data[8] = parameters.minSpeed.inByte[0];
    data[9] = parameters.minSpeed.inByte[1];
    data[10] = parameters.minSpeed.inByte[2];
    data[11] = parameters.minSpeed.inByte[3];

    data[12] = parameters.maxSpeed.inByte[0];
    data[13] = parameters.maxSpeed.inByte[1];
    data[14] = parameters.maxSpeed.inByte[2];
    data[15] = parameters.maxSpeed.inByte[3];

    data[16] = parameters.minAngle.inByte[0];
    data[17] = parameters.minAngle.inByte[1];
    data[18] = parameters.minAngle.inByte[2];
    data[19] = parameters.minAngle.inByte[3];

    data[20] = parameters.maxAngle.inByte[0];
    data[21] = parameters.maxAngle.inByte[1];
    data[22] = parameters.maxAngle.inByte[2];
    data[23] = parameters.maxAngle.inByte[3];

    data[24] = parameters.leftBorder.inByte[0];
    data[25] = parameters.leftBorder.inByte[1];
    data[26] = parameters.leftBorder.inByte[2];
    data[27] = parameters.leftBorder.inByte[3];

    data[28] = parameters.rightBorder.inByte[0];
    data[29] = parameters.rightBorder.inByte[1];
    data[30] = parameters.rightBorder.inByte[2];
    data[31] = parameters.rightBorder.inByte[3];

    Frame frame = configureFrame(CMD_SET_PARAMETERS, data, length);
    writeFrame(frame);
    
    Frame receivedFrame = readFrame(CMD_READ_STATUS);
    
    if (receivedFrame.isValid) {
        return receivedFrame.data[0];
    }
}

Parameters SmartRoadRadar::getParameters() {
    Frame frame = configureFrame(CMD_GET_PARAMETERS);
    writeFrame(frame);
    
    Frame receivedFrame = readFrame(CMD_READ_PARAMETERS);
    Parameters receivedParameters{};

    if (receivedFrame.isValid) {
        receivedParameters.minDistance.inByte[0] = frame.data[4];
        receivedParameters.minDistance.inByte[1] = frame.data[5];
        receivedParameters.minDistance.inByte[2] = frame.data[6];
        receivedParameters.minDistance.inByte[3] = frame.data[7];

        receivedParameters.maxDistance.inByte[0] = frame.data[0];
        receivedParameters.maxDistance.inByte[1] = frame.data[1];
        receivedParameters.maxDistance.inByte[2] = frame.data[2];
        receivedParameters.maxDistance.inByte[3] = frame.data[3];

        receivedParameters.minSpeed.inByte[0] = frame.data[12];
        receivedParameters.minSpeed.inByte[1] = frame.data[13];
        receivedParameters.minSpeed.inByte[2] = frame.data[14];
        receivedParameters.minSpeed.inByte[3] = frame.data[15];

        receivedParameters.maxSpeed.inByte[0] = frame.data[8];
        receivedParameters.maxSpeed.inByte[1] = frame.data[9];
        receivedParameters.maxSpeed.inByte[2] = frame.data[10];
        receivedParameters.maxSpeed.inByte[3] = frame.data[11];

        receivedParameters.minAngle.inByte[0] = frame.data[16];
        receivedParameters.minAngle.inByte[1] = frame.data[17];
        receivedParameters.minAngle.inByte[2] = frame.data[18];
        receivedParameters.minAngle.inByte[3] = frame.data[19];

        receivedParameters.maxAngle.inByte[0] = frame.data[20];
        receivedParameters.maxAngle.inByte[1] = frame.data[21];
        receivedParameters.maxAngle.inByte[2] = frame.data[22];
        receivedParameters.maxAngle.inByte[3] = frame.data[23];

        receivedParameters.leftBorder.inByte[0] = frame.data[24];
        receivedParameters.leftBorder.inByte[1] = frame.data[25];
        receivedParameters.leftBorder.inByte[2] = frame.data[26];
        receivedParameters.leftBorder.inByte[3] = frame.data[27];

        receivedParameters.rightBorder.inByte[0] = frame.data[28];
        receivedParameters.rightBorder.inByte[1] = frame.data[29];
        receivedParameters.rightBorder.inByte[2] = frame.data[30];
        receivedParameters.rightBorder.inByte[3] = frame.data[31];

        return receivedParameters;
    }
}

byte_t SmartRoadRadar::setTargetNumber(uint8_t number) {
    Frame frame = configureFrame(CMD_SET_TARGET_NUM, number);
    writeFrame(frame);

    Frame receivedFrame = readFrame(CMD_READ_STATUS);

    if (receivedFrame.isValid) {
        return receivedFrame.data[0];
    }
}

void SmartRoadRadar::getTargetData(TargetData *targetData) {
    Frame receivedFrame = readFrame(CMD_READ_TARGET_DATA);

    if (receivedFrame.isValid) {
        int targetCount = (receivedFrame.dataLength.inShort - 1) / TARGET_DATA_BYTE_LENGTH;

        for (int pos = 0; pos < targetCount; ++pos) {
            targetData[pos].targetNum = receivedFrame.data[0 + TARGET_DATA_BYTE_LENGTH * pos];
            targetData[pos].distance = dataByteToFloat(
                    receivedFrame.data[1 + TARGET_DATA_BYTE_LENGTH * pos],
                    receivedFrame.data[2 + TARGET_DATA_BYTE_LENGTH * pos]);
            targetData[pos].speed = dataByteToFloat(
                    receivedFrame.data[3 + TARGET_DATA_BYTE_LENGTH * pos],
                    receivedFrame.data[4 + TARGET_DATA_BYTE_LENGTH * pos]);
            targetData[pos].angle = dataByteToFloat(
                    receivedFrame.data[5 + TARGET_DATA_BYTE_LENGTH * pos],
                    receivedFrame.data[6 + TARGET_DATA_BYTE_LENGTH * pos]);
            targetData[pos].snr =dataByteToFloat(
                    receivedFrame.data[7 + TARGET_DATA_BYTE_LENGTH * pos],
                    receivedFrame.data[8 + TARGET_DATA_BYTE_LENGTH * pos]);
        }

        //for (int i = 0; i < targetCount; ++i) {
        //    printf(
        //            "%d | %f m\t%f m/s\t%f degree\n",
        //            targetData[i].targetNum,
        //            targetData[i].distance,
        //            targetData[i].speed,
        //            targetData[i].angle);
        //}
    }
}

byte_t SmartRoadRadar::enableDataTransmit() {
    Frame frame = configureFrame(CMD_ENABLE_TRANSMIT);
    writeFrame(frame);

    Frame receivedFrame = readFrame(CMD_READ_STATUS);

    if (receivedFrame.isValid) {
        return receivedFrame.data[0];
    }
}

byte_t SmartRoadRadar::disableDataTransmit() {
    Frame frame = configureFrame(CMD_DISABLE_TRANSMIT);
    writeFrame(frame);

    Frame receivedFrame = readFrame(CMD_READ_STATUS);

    if (receivedFrame.isValid) {
        return receivedFrame.data[0];
    }
}

byte_t SmartRoadRadar::setDataTransmitFrequency(byte_t freq) {
    Frame frame = configureFrame(CMD_SET_DATA_FREQ, freq);
    writeFrame(frame);

    Frame receivedFrame = readFrame(CMD_READ_STATUS);

    if (receivedFrame.isValid) {
        return receivedFrame.data[0];
    }
}

byte_t SmartRoadRadar::setZeroDataReporting(byte_t state) {
    Frame frame = configureFrame(CMD_SET_ZERO_REPORT, state);
    writeFrame(frame);

    Frame receivedFrame = readFrame(CMD_READ_STATUS);

    if (receivedFrame.isValid) {
        return receivedFrame.data[0];
    }
}

