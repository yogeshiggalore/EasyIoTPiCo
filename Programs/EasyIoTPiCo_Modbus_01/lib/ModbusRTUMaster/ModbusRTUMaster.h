/*******************************************************************************
* File Name: ModbusRTUMaster.h
*
* Version: 1.0
*
* Description:
* This is header file for modbus rtu master. All the functions related to
* Modbus master are implemented in this file
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
#ifndef ModbusRTUMaster_h
#define ModbusRTUMaster_h

#include <Arduino.h>

//#define MODBUS_ENABLE_PIN  D2
//#define MODBUS_LED_PIN     D6

#define MODBUS_DATA_PREPARED    0x01
#define MODBUS_DATA_WRITTEN     0x02
#define MODBUS_DATA_RECEIVED    0x03
#define MODBUS_DATA_PROCESSED   0x04
#define MODBUS_DATA_ERROR       0xFF

#define MODBUS_READ_INTERVAL    5000
#define MODBUS_TIMEOUT          3000

class ModbusRTUMaster{
public:

    uint8_t ui8ModbusSlaveid = 1;
    uint8_t ui8ModbusBaudrate = 4;
    uint8_t ui8ModbusDatabits= 8;
    uint8_t ui8ModbusParity = 0;
    uint8_t ui8ModbusStopbit = 1;

    uint16_t ui16ModbusPrepareResult;
    uint16_t ui16ModbusReadResult;
    uint16_t ui16ModbusProcessResult;
    uint16_t ui16ModbusWriteResult;

    uint16_t ui16ModbusRxCounter;
    uint16_t ui16ModbusExpectedBytes;
    uint8_t  ui8ModbusExpectedSlaveid;
    uint8_t  ui8ModbusErrorCode;

    uint16_t ui16ModbusTimeoutCounter=0;
    uint16_t ui16ModbusIntervalCounter=0;
    uint8_t uint8ModbusIntervalFlag=0;
    uint8_t uint8ModbusTimeoutFlag=0;
    uint8_t ui8ModbusTaskCompleteFlag=0;

    ModbusRTUMaster();
    void Get_Config_Parameters_From_Memory(void);
    uint16_t Is_Slave_DataReceived(void);
    void IRAM_ATTR Check_Modbus_Timer(void);

    uint8_t Modbus_RunCommand(uint8_t aui8Buffer[],uint8_t ui8ByteToSent,uint8_t BytesExpected,uint8_t* RxByteCount, uint16_t TimeOutIntervalInMs, uint16_t *ResponseTimeout);

    void Prepare_Packet(void);
    void Start(void);
    void Read(void);
    void Process(void);
    void Write(void);
    void Update(void);
};

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_TWOWIRE)
extern ModbusRTUMaster Modbus;
#endif
#endif