/*******************************************************************************
* File Name: ESPRead.h
*
* Version: 1.0
*
* Description:
* This is header file for ESP read. All the functions related to
* analog read and dht22 read are implemented in this file
*
* Owner:
* Yogesh M Iggalore
*
* Code Tested With:
* 1. platformio and esp8266
*
********************************************************************************
* Copyright (2020-21) , EasyIoTPiCo
********************************************************************************/
#ifndef ESPRead_h
#define ESPRead_h

#include <Arduino.h>
#include <DHT.h>

/* define dht22 pins */
#define DHT11_PIN	D5

/* define type as DHT22 */
#define DHTTYPE 	DHT11 

#define ESPREAD_TIME_INTERVAL    5000

typedef struct{
    bool bDHTDataValid;
    float fHumidity;
    float fTemperature;
    float fHeatIndex;
    float fLMTemperature;
    uint16_t ui16ADCValue;
    uint16_t ui16ESPReadCounter=0;
}READ_PARAMETER;

class ESPRead{
public:
    ESPRead();
    READ_PARAMETER tdfParameters;
    void Start(void);
    void Update(void);
    uint8_t Read_DHT22(void);
    void Read_ADC_Values(void);
private:
    DHT dht;
};

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_TWOWIRE)
extern ESPRead ERead;
#endif
#endif
