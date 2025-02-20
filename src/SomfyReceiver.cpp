#include "SomfyReceiver.h"

SomfyReceiver::SomfyReceiver(SomfyFrame& frame) : frame(frame), decoder(){
    reset();
}

/*
 * @param pulse : Pulse duration in microseconds
 */
void SomfyReceiver::pulse(uint32_t pulse){
    switch(status){
        case somfy_status_t::Sync:
            pulseHwSync(pulse);
        break;
        case somfy_status_t::Start:
            pulseStart(pulse);
        break;
        case somfy_status_t::Payload:
            pulsePayload(pulse);
        break;
    }
}

void SomfyReceiver::pulseHwSync(uint32_t pulse){
    if(pulse > SOMFY_HW_SYNC_MIN_DURATION && pulse < SOMFY_HW_SYNC_MAX_DURATION){
        hw_sync_nb++;
        return;
    }else if(hw_sync_nb >= 4 && pulseSwSync(pulse)){
        return;
    }

    reset();
}

bool SomfyReceiver::pulseSwSync(uint32_t pulse){
    if(pulse > SOMFY_SW_SYNC_MIN_DURATION && pulse < SOMFY_SW_SYNC_MAX_DURATION){
        status = somfy_status_t::Start;
        return true;
    }

    return false;
}

void SomfyReceiver::pulseStart(uint32_t pulse){
    somfy_pulse_type_t type = getPulseType(pulse);

    if(type == somfy_pulse_type_t::Invalid){
        reset();
        return;
    }

    frame.setSize(hw_sync_nb);
    decoder.init(frame.bytes, frame.frameSize, type == somfy_pulse_type_t::Long);

    status = somfy_status_t::Payload;
}

void SomfyReceiver::pulsePayload(uint32_t pulse){
    somfy_pulse_type_t type = getPulseType(pulse);
    
    if(type == somfy_pulse_type_t::Invalid){
        reset();
        return;
    }

    if(type == somfy_pulse_type_t::Long){
        if(!decoder.addLongPulse()){
            reset();
        }
    }else{
        decoder.addShortPulse();
    }

    if(decoder.isLastBit()){
        decoder.storeBit();
    }

    if(decoder.completed()){
        frame.decode();
        status = somfy_status_t::Done;
    }
}

somfy_pulse_type_t SomfyReceiver::getPulseType(uint32_t pulse){
    if(pulse > SOMFY_SYMBOL_MIN_DURATION && pulse < SOMFY_SYMBOL_MAX_DURATION){
        return somfy_pulse_type_t::Long;
    }else if(pulse > SOMFY_HALF_SYMBOL_MIN_DURATION && pulse < SOMFY_HALF_SYMBOL_MAX_DURATION){
        return somfy_pulse_type_t::Short;
    }

    return somfy_pulse_type_t::Invalid;
}

void SomfyReceiver::reset(){
    status = somfy_status_t::Sync;
    hw_sync_nb = 0;
}