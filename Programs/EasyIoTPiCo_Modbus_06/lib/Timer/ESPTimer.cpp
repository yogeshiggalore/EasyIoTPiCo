/*******************************************************************************
*File Name: ESPTimer.cpp
*
* Version: 1.0
*
* Description:
* In this source code for handling 1ms time
*
*
* Owner:
* silicosmos
*
********************************************************************************
* Copyright (2020-21) ,silicosmos.in
*******************************************************************************/
#include <Arduino.h>
#include <ESPTimer.h>

uint8_t ui8Timer1msFlag;

// Constructors ////////////////////////////////////////////////////////////////
ESPTimer::ESPTimer(){}

void ESPTimer::Start(void){
    
}


// Preinstantiate Objects //////////////////////////////////////////////////////
#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_TWOWIRE)
ESPTimer ETimer;
#endif