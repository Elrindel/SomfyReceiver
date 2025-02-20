#include "SomfyFrame.h"

SomfyFrame::SomfyFrame(const SomfyFrame& frame){
    frameSize = frame.frameSize;
    memcpy(bytes, frame.bytes, SOMFY_FRAME_MAX_SIZE_BYTES);
    valid = frame.valid;
    key = frame.key;
    cmd = frame.cmd;
    protocol = frame.protocol;
    checksum = frame.checksum;
    code = frame.code;
    address = frame.address;
}

bool SomfyFrame::setSize(uint8_t hw_sync_nb){
    if(hw_sync_nb <= 7 || hw_sync_nb == 14){
        frameSize = 56;
        return true;
    }
    
    if(hw_sync_nb == 13 || hw_sync_nb == 12 || hw_sync_nb > 17){
        frameSize = 80;
        return true;
    }

    frameSize = 56;
    return false;
}

bool SomfyFrame::decode(){
    uint8_t decoded[10] = {
        bytes[0],
        bytes[1] ^ bytes[0],
        bytes[2] ^ bytes[1],
        bytes[3] ^ bytes[2],
        bytes[4] ^ bytes[3],
        bytes[5] ^ bytes[4],
        bytes[6] ^ bytes[5],
        bytes[7],
        bytes[8],
        bytes[9]
    };

    uint8_t calcChecksum = 0;
    for(uint8_t i = 0; i < 7; i++){
        calcChecksum ^= (i == 1)? (decoded[i] >> 4) : decoded[i] ^ (decoded[i] >> 4);
    }
    calcChecksum &= 0b1111;

    key = decoded[0];
    checksum = decoded[1] & 0b1111;
    code = decoded[3] + (decoded[2] << 8);
    address = (decoded[6] + (decoded[5] << 8) + (decoded[4] << 16));;

    cmd = (somfy_command_t)(decoded[1] >> 4);
    if(cmd == somfy_command_t::RTWProto){
        if(key >= 160){
            protocol = somfy_protocol_t::RTS;
            if(key == 164){
                cmd = somfy_command_t::Toggle;
            }
        }else if(key > 148){
            protocol = somfy_protocol_t::RTV;
            cmd = (somfy_command_t)(key - 148);
        }else if(key > 133){
            protocol = somfy_protocol_t::RTW;
            cmd = (somfy_command_t)(key - 133);
        }
    }else{
        protocol = somfy_protocol_t::RTS;
    }

    valid = (checksum == calcChecksum && address > 0 && address < 16777215);

    if(cmd != somfy_command_t::Sensor && valid){
        valid = (code > 0);
    }
    if(valid && protocol == somfy_protocol_t::RTS && frameSize == 80){
        if((decoded[9] & 0x0F) != calc80Checksum(decoded[7], decoded[8], decoded[9])){
            valid = false;
        }else{
            if(cmd == somfy_command_t::My){
                cmd = (somfy_command_t)((decoded[1] >> 4) | ((decoded[8] & 0x0F) << 4));
            }else if(cmd == somfy_command_t::StepDown){
                cmd = (somfy_command_t)((decoded[1] >> 4) | ((decoded[8] & 0x08) << 4));
            }
        }
    }

    if(valid && key == 0){
        valid = false;
    }

    return valid;
}