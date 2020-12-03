/*******************************************************************************
* File Name: ESPMemory.cpp
*
* Version: 1.0
*
* Description:
* This is source file for memory. All the functions related to
* memory are implemented in this file
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

#include <Arduino.h>
#include <ESPUtils.h>
#include <EEPROM.h>
#include <ESPMemory.h>
#include <ModbusSlave.h>

// Constructors ////////////////////////////////////////////////////////////////
ESPMemory::ESPMemory(){}

/* ini memory and update all bytes*/
void ESPMemory::Init(void){
    EEPROM.begin(MEMORY_SIZE);
    if(EEPROM.read(MEMORY_INIT_BYTE) != 1){
        EEPROM.write(MEMORY_COIL_REG_BYTE,0);
        EEPROM.write(MEMORY_HOLD_REG1_HIGH_BYTE,0);
        EEPROM.write(MEMORY_HOLD_REG1_LOW_BYTE,0);
        EEPROM.write(MEMORY_HOLD_REG2_HIGH_BYTE,0);
        EEPROM.write(MEMORY_HOLD_REG2_LOW_BYTE,0);
        EEPROM.write(MEMORY_HOLD_REG3_HIGH_BYTE,0);
        EEPROM.write(MEMORY_HOLD_REG3_LOW_BYTE,0);
        EEPROM.write(MEMORY_HOLD_REG4_HIGH_BYTE,0);
        EEPROM.write(MEMORY_HOLD_REG4_LOW_BYTE,0);
        EEPROM.write(MEMORY_INIT_BYTE,1);
        EEPROM.commit();
    }

	/* update all memory bytes */
	Update_All();
}

/* This function updates all memory bytes */
void ESPMemory::Update_All(void){
    Modbus.ui8CoilStatus = EEPROM.read(MEMORY_COIL_REG_BYTE);
    
    Modbus.aui16HoldingRegister[0] =  EEPROM.read(MEMORY_HOLD_REG1_HIGH_BYTE);
    Modbus.aui16HoldingRegister[0] = Modbus.aui16HoldingRegister[0] << 8;
    Modbus.aui16HoldingRegister[0] = Modbus.aui16HoldingRegister[0] | EEPROM.read(MEMORY_HOLD_REG1_LOW_BYTE);

    Modbus.aui16HoldingRegister[1] =  EEPROM.read(MEMORY_HOLD_REG2_HIGH_BYTE);
    Modbus.aui16HoldingRegister[1] = Modbus.aui16HoldingRegister[1] << 8;
    Modbus.aui16HoldingRegister[1] = Modbus.aui16HoldingRegister[1] | EEPROM.read(MEMORY_HOLD_REG2_LOW_BYTE);

    Modbus.aui16HoldingRegister[2] =  EEPROM.read(MEMORY_HOLD_REG3_HIGH_BYTE);
    Modbus.aui16HoldingRegister[2] = Modbus.aui16HoldingRegister[2] << 8;
    Modbus.aui16HoldingRegister[2] = Modbus.aui16HoldingRegister[2] | EEPROM.read(MEMORY_HOLD_REG3_LOW_BYTE);

    Modbus.aui16HoldingRegister[3] =  EEPROM.read(MEMORY_HOLD_REG4_HIGH_BYTE);
    Modbus.aui16HoldingRegister[3] = Modbus.aui16HoldingRegister[3] << 8;
    Modbus.aui16HoldingRegister[3] = Modbus.aui16HoldingRegister[3] | EEPROM.read(MEMORY_HOLD_REG4_LOW_BYTE);
}

void ESPMemory::Update_Holding_Reg(uint8_t ui8Index, uint16_t ui16Value){
    uint8_t ui8HighByte=0;
    uint8_t ui8LowByte=0;

    ui8HighByte = ui16Value>>8;
    ui8LowByte = ui16Value;

    if(ui8Index == 0){
        EEPROM.write(MEMORY_HOLD_REG1_HIGH_BYTE,ui8HighByte);
        EEPROM.write(MEMORY_HOLD_REG1_LOW_BYTE,ui8LowByte);
        EEPROM.commit();
    }else if(ui8Index == 1){
        EEPROM.write(MEMORY_HOLD_REG2_HIGH_BYTE,ui8HighByte);
        EEPROM.write(MEMORY_HOLD_REG2_LOW_BYTE,ui8LowByte);
        EEPROM.commit();
    }else if(ui8Index == 2){
        EEPROM.write(MEMORY_HOLD_REG3_HIGH_BYTE,ui8HighByte);
        EEPROM.write(MEMORY_HOLD_REG3_LOW_BYTE,ui8LowByte);
        EEPROM.commit();
    }else if(ui8Index == 3){
        EEPROM.write(MEMORY_HOLD_REG4_HIGH_BYTE,ui8HighByte);
        EEPROM.write(MEMORY_HOLD_REG4_LOW_BYTE,ui8LowByte);
        EEPROM.commit();
    }else{

    }
    Update_All();
}

// Preinstantiate Objects //////////////////////////////////////////////////////
#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_TWOWIRE)
ESPMemory EMemory;
#endif