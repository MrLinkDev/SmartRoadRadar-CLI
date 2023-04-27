#ifndef SMART_ROAD_SMART_ROAD_RADAR_H
#define SMART_ROAD_SMART_ROAD_RADAR_H

#include "serial.h"

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


typedef unsigned char byte_t;

struct Frame {
    bool isValid = false;

    byte_t header[2] = {HEADER_DATA_FRAME_1, HEADER_DATA_FRAME_2};

    union {
        byte_t inByte[2];
        unsigned short inShort;
    } dataLength{};

    byte_t commandWord{};
    byte_t *data = nullptr;
    byte_t checksum{};
};

struct Parameters {
    union {
        float inFloat = MIN_DISTANCE;
        byte_t inByte[4];
    } minDistance;

    union {
        float inFloat = MAX_DISTANCE;
        byte_t inByte[4];
    } maxDistance;

    union {
        float inFloat = MIN_SPEED;
        byte_t inByte[4];
    } minSpeed;

    union {
        float inFloat = MAX_SPEED;
        byte_t inByte[4];
    } maxSpeed;

    union {
        float inFloat = MIN_ANGLE;
        byte_t inByte[4];
    } minAngle;

    union {
        float inFloat = MAX_ANGLE;
        byte_t inByte[4];
    } maxAngle;

    union {
        float inFloat = LEFT_BORDER;
        byte_t inByte[4];
    } leftBorder;

    union {
        float inFloat = RIGHT_BORDER;
        byte_t inByte[4];
    } rightBorder;
};

struct TargetData {
    byte_t targetNum{};

    float distance;
    float speed;
    float angle;
    float snr;
};

class SmartRoadRadar {

private:
    Serial dataBus;

    Frame readFrame();
    void writeFrame(Frame frame);

    Frame configureFrame(byte_t cmd, byte_t *data, unsigned short length);
    Frame configureFrame(byte_t cmd, byte_t data);
    Frame configureFrame(byte_t cmd);

    byte_t calculateChecksum(Frame frame);
    float dataByteToFloat(byte_t dataByte1, byte_t dataByte2);

public:

    SmartRoadRadar() = default;
    SmartRoadRadar(char *address);

    void getFirmwareVersion(char *version);

    byte_t setParameters(Parameters parameters);
    Parameters getParameters();

    byte_t setTargetNumber(uint8_t number);

    void getTargetData(TargetData *targetData);

    byte_t enableDataTransmit();
    byte_t disableDataTransmit();

    byte_t setDataTransmitFrequency(byte_t freq);

    byte_t setZeroDataReporting(byte_t state);
};


#endif //SMART_ROAD_SMART_ROAD_RADAR_H
