#ifndef MANCHESTER_DECODER_H
#define MANCHESTER_DECODER_H

#include <stdint.h>

class ManchesterDecoder
{
    public:
        void init(uint8_t *bytes, uint16_t bitsSize, bool initBit){
            this->bytes = bytes;
            this->bitsSize = bitsSize;
            nextBit = initBit;
            secondPulse = initBit;
            byteIndex = 0;
            bitIndex = 0;
            bytes[0] = 0;
        }

        void addShortPulse(){
            if(secondPulse){
                storeBit();
            }
            secondPulse = !secondPulse;
        }

        bool addLongPulse(){
            if(!secondPulse){
                return false;
            }

            storeBit();
            nextBit = !nextBit;

            return true;
        }

        bool isLastBit(){
            return bitIndex == bitsSize - 1;
        }

        bool completed(){
            return bitIndex == bitsSize;
        }

        void storeBit(){
            if(bitIndex >= bitsSize){
                return;
            }

            bytes[byteIndex] <<= 1;
            bytes[byteIndex] |= nextBit;
            bitIndex++;
            if(bitIndex % 8 == 0 && bitIndex < bitsSize){
                byteIndex++;
                bytes[byteIndex] = 0;
            }
        }

    private:
        bool secondPulse;
        bool nextBit;
        uint8_t *bytes;
        uint8_t byteIndex;
        uint16_t bitsSize;
        uint16_t bitIndex;
};

#endif