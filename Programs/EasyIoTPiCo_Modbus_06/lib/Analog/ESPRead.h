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
* 1. platformio and esp32
*
********************************************************************************
* Copyright (2020-21) , EasyIoT
********************************************************************************/
#ifndef ESPRead_h
#define ESPRead_h

#include <Arduino.h>
#include <DHT.h>

/* define dht22 pins */
#define DHT22_PIN	4

/* define type as DHT22 */
#define DHTTYPE 	DHT22 

#define ESPREAD_TIME_INTERVAL    5000

#define ADC_POT_PIN		36
#define ADC_LM35_PIN	39

#define ADC_RESOLUTION 	12
#define ADC_WIDTH		12
#define ADC_SAMPLES		30
#define ADC_CLOCK_DIV	1
#define ADC_CYCLE		8

#define LED1_PIN    2

class ESPRead{
public:
    /* DHT22 veriables */
    float fHumidity;
    float fTemperature;
    float fHeatIndex;
    uint8_t ui8DHTValidFlag;
    uint16_t ui16PotADCValue;
    uint16_t ui16LM35ADCValue;
    float    fPotVolt;
    float    fLMTemperature;

    uint16_t ui16ESPReadCounter=0;

    ESPRead();

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
