/*******************************************************************************
* File Name: ModbusSlave.h
*
* Version: 1.0
*
* Description:
* This is header file for memory. All the functions related to
* Memory are implemented in this file
*
* Owner:
* Yogesh M Iggalore
*
* Code Tested With:
* 1. platformio and esp8266
*
********************************************************************************
* Copyright (2020-21) , eNtroL
********************************************************************************/
#ifndef ESPMemory_h
#define ESPMemory_h

#include <Arduino.h>

#define MEMORY_SIZE       10

#define MEMORY_COIL_REG_BYTE        0
#define MEMORY_HOLD_REG1_HIGH_BYTE  1
#define MEMORY_HOLD_REG1_LOW_BYTE   2
#define MEMORY_HOLD_REG2_HIGH_BYTE  3
#define MEMORY_HOLD_REG2_LOW_BYTE   4
#define MEMORY_HOLD_REG3_HIGH_BYTE  5
#define MEMORY_HOLD_REG3_LOW_BYTE   6
#define MEMORY_HOLD_REG4_HIGH_BYTE  7
#define MEMORY_HOLD_REG4_LOW_BYTE   8

#define MEMORY_INIT_BYTE            9

class ESPMemory{
public:
    ESPMemory();
    void Init(void);
    void Update_All(void);
    void Update_Holding_Reg(uint8_t ui8Index, uint16_t ui16Value);
};

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_TWOWIRE)
extern ESPMemory EMemory;
#endif
#endif