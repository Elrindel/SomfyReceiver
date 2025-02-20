/*
    ELECHOUSE_CC1101.ino

    Use of a CC1101 receiver with the SmartRC-CC1101-Driver-Lib library ( https://github.com/LSatan/SmartRC-CC1101-Driver-Lib ) to receive pulses.
*/

#include <ELECHOUSE_CC1101_SRC_DRV.h>
#include <SomfyReceiver.h>

#define RX_PIN   12 //CC1101 : GDO2
#define TX_PIN   13 //CC1101 : GDO0
#define SCK_PIN  18 //CC1101 : SCK
#define MISO_PIN 19 //CC1101 : MISO
#define MOSI_PIN 23 //CC1101 : MOSI
#define CSN_PIN  5  //CC1101 : CSN

#if defined(ESP8266)
    #define RECEIVE_ATTR ICACHE_RAM_ATTR
#elif defined(ESP32)
    #define RECEIVE_ATTR IRAM_ATTR
#else
    #define RECEIVE_ATTR
#endif

static int interruptPin = 0;
SomfyFrame frame = SomfyFrame();
SomfyReceiver receiver = SomfyReceiver(frame);

/*
    This buffer allows pulse processing to be deported outside the interruption to keep it as short as possible.
    This means no blocking code in your loop, and you can increase the buffer size if your loop is too slow.
*/
#define PULSES_BUFFER_SIZE 10
volatile uint32_t pulsesBuffer[PULSES_BUFFER_SIZE] = {0};
volatile uint8_t pulsesBufferIndex = 0;

void RECEIVE_ATTR handleReceive(){
    if(pulsesBufferIndex >= PULSES_BUFFER_SIZE){
        return;
    }

    static uint32_t last_time = 0;
    const uint32_t time = micros();
    const uint32_t duration = time - last_time;

    if(duration < SOMFY_HALF_SYMBOL_MIN_DURATION){
        return;
    }

    last_time = time;

    //Stores the pulse in the buffer to limit the interrupt execution time
    pulsesBuffer[pulsesBufferIndex++] = duration;
}

void setup(){
    Serial.begin(115200);
    while(!Serial);

    ELECHOUSE_cc1101.setGDO(TX_PIN, RX_PIN);
    ELECHOUSE_cc1101.setSpiPin(SCK_PIN, MISO_PIN, MOSI_PIN, CSN_PIN);

    if(ELECHOUSE_cc1101.getCC1101()){
        Serial.println("Connection OK");
    }else{
        Serial.println("Connection Error");
    }

    ELECHOUSE_cc1101.Init();
    ELECHOUSE_cc1101.setCCMode(0);
    ELECHOUSE_cc1101.setMHZ(433.42);
    ELECHOUSE_cc1101.setRxBW(99.97);
    ELECHOUSE_cc1101.setDeviation(47.60);
    ELECHOUSE_cc1101.setPA(10);
    ELECHOUSE_cc1101.setModulation(2);
    ELECHOUSE_cc1101.setManchester(1);
    ELECHOUSE_cc1101.setPktFormat(3);
    ELECHOUSE_cc1101.setDcFilterOff(0);
    ELECHOUSE_cc1101.setCrc(0);
    ELECHOUSE_cc1101.setCRC_AF(0);
    ELECHOUSE_cc1101.setSyncMode(4);
    ELECHOUSE_cc1101.setAdrChk(0);

    pinMode(RX_PIN, INPUT);
    interruptPin = digitalPinToInterrupt(RX_PIN);
    attachInterrupt(interruptPin, handleReceive, CHANGE);
    ELECHOUSE_cc1101.SetRx();
}

void loop(){
    if(pulsesBufferIndex > 0){
        //Sends pulses recorded during interruption to SomfyReceiver
        for(uint8_t i = 0; i < pulsesBufferIndex; i++){
            receiver.pulse(pulsesBuffer[i]);
        }
        pulsesBufferIndex = 0;
    }

    //Receiver status is "Done", time to read the frame
    if(receiver.getStatus() == somfy_status_t::Done){
        Serial.print("VALID:");
        Serial.print(frame.valid);
        Serial.print(" KEY:");
        Serial.print(frame.key);
        Serial.print(" ADDR:");
        Serial.print(frame.address);
        Serial.print(" CMD:");
        Serial.print((uint8_t)frame.cmd);
        Serial.print(" CODE:");
        Serial.println(frame.code);

        //Reset the receiver to receive next pulses
        //If you want to save the frame for processing after the reset, create a new frame instance before the reset : SomfyFrame savedFrame = SomfyFrame(frame);
        receiver.reset();
    }
}