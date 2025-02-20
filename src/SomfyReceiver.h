#ifndef SOMFY_RECEIVER_H
#define SOMFY_RECEIVER_H

#include <stdint.h>
#include "SomfyFrame.h"
#include "ManchesterDecoder.h"

#ifndef SOMFY_TOLERANCE_MIN
#define SOMFY_TOLERANCE_MIN 0.7
#endif

#ifndef SOMFY_TOLERANCE_MAX
#define SOMFY_TOLERANCE_MAX 1.3
#endif

#define SOMFY_HALF_SYMBOL 640
#define SOMFY_SYMBOL SOMFY_HALF_SYMBOL * 2

#define SOMFY_HALF_SYMBOL_MIN_DURATION SOMFY_HALF_SYMBOL * SOMFY_TOLERANCE_MIN
#define SOMFY_HALF_SYMBOL_MAX_DURATION SOMFY_HALF_SYMBOL * SOMFY_TOLERANCE_MAX
#define SOMFY_SYMBOL_MIN_DURATION SOMFY_SYMBOL * SOMFY_TOLERANCE_MIN
#define SOMFY_SYMBOL_MAX_DURATION SOMFY_SYMBOL * SOMFY_TOLERANCE_MAX
#define SOMFY_HW_SYNC_MIN_DURATION SOMFY_SYMBOL * 2 * SOMFY_TOLERANCE_MIN
#define SOMFY_HW_SYNC_MAX_DURATION SOMFY_SYMBOL * 2 * SOMFY_TOLERANCE_MAX
#define SOMFY_SW_SYNC_MIN_DURATION 4850 * SOMFY_TOLERANCE_MIN
#define SOMFY_SW_SYNC_MAX_DURATION 4850 * SOMFY_TOLERANCE_MAX

enum class somfy_status_t : uint8_t {
    Sync = 0,
    Start = 1,
    Payload = 2,
    Done = 3
};

enum class somfy_pulse_type_t : uint8_t {
    Invalid = 0,
    Short = 1,
    Long = 2
};

class SomfyReceiver
{
    public:
        SomfyReceiver(SomfyFrame& frame);
        void pulse(uint32_t pulse);
        void reset();
        somfy_status_t getStatus(){
            return status;
        };

    private:
        void pulseHwSync(uint32_t pulse);
        bool pulseSwSync(uint32_t pulse);
        void pulseStart(uint32_t pulse);
        void pulsePayload(uint32_t pulse);
        somfy_pulse_type_t getPulseType(uint32_t pulse);
        somfy_status_t status;
        uint8_t hw_sync_nb;
        SomfyFrame& frame;
        ManchesterDecoder decoder;
};

#endif