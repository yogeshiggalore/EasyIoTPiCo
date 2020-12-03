/*******************************************************************************
*File Name: ESPTimer.h
*
* Version: 1.0
*
* Description:
* In this header file handle 1ms timer
*
*
* Owner:
* silicosmos
*
********************************************************************************
* Copyright (2020-21) , silicosmos.in
*******************************************************************************/

#ifndef ESPTIMER_h
#define ESPTIMER_h
#include <Arduino.h>
#include <ESP8266WiFi.h>

class ESPTimer{
public:
    ESPTimer();
    void Start(void);
};
#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_TWOWIRE)
extern ESPTimer ETimer;
#endif
#endif