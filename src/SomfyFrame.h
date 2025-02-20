#ifndef SOMFY_FRAME_H
#define SOMFY_FRAME_H

#include <stdint.h>
#include <cstring>

#define SOMFY_FRAME_MAX_SIZE_BYTES 10

enum class somfy_protocol_t : uint8_t {
    RTS = 0x00,
    RTW = 0x01,
    RTV = 0x02,
    GP_Relay = 0x08,
    GP_Remote = 0x09
};

enum class somfy_command_t : uint8_t {
    Unknown0 = 0x0,
    My = 0x1,
    Up = 0x2,
    MyUp = 0x3,
    Down = 0x4,
    MyDown = 0x5,
    UpDown = 0x6,
    MyUpDown = 0x7,
    Prog = 0x8,
    SunFlag = 0x9,
    Flag = 0xA,
    StepDown = 0xB,
    Toggle = 0xC,
    UnknownD = 0xD,
    Sensor = 0xE,
    RTWProto = 0xF, // RTW Protocol
    // Command extensions for 80 bit frames
    StepUp = 0x8B,
    Favorite = 0xC1,
    Stop = 0xF1
};

class SomfyFrame
{
    public:
        uint8_t frameSize = SOMFY_FRAME_MAX_SIZE_BYTES;
        uint8_t bytes[SOMFY_FRAME_MAX_SIZE_BYTES];

        bool valid = false;
        uint8_t key = 0;
        somfy_command_t cmd = somfy_command_t::Unknown0;
        somfy_protocol_t protocol = somfy_protocol_t::RTS;
        uint8_t checksum = 0;
        uint16_t code = 0;
        uint32_t address = 0;

        SomfyFrame(){};
        SomfyFrame(const SomfyFrame& frame);

        bool setSize(uint8_t hw_sync_nb);
        bool decode();
    
    private:
        uint8_t calc80Checksum(uint8_t b0, uint8_t b1, uint8_t b2){
            uint8_t cs80 = 0;
            cs80 = (((b0 & 0xF0) >> 4) ^ ((b1 & 0xF0) >> 4));
            cs80 ^= ((b2 & 0xF0) >> 4);
            cs80 ^= (b0 & 0x0F);
            cs80 ^= (b1 & 0x0F);
            return cs80;
        }
};

#endif