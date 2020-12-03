/*******************************************************************************
*File Name: ModbusTCPSlave.h
*
* Version: 1.0
*
* Description:
* In this header file for ModbusTCPSlave functions
*
*
* Owner:
* Yogesh M Iggalore
*
********************************************************************************
* Copyright (2020-21) , METI M2M India Pvt Ltd
*******************************************************************************/
#ifndef ModbuTCPSlave_h
#define ModbuTCPSlave_h

#include <Arduino.h>
#include <WiFiClient.h>
#include <WiFiServer.h>

#define MODBUS_TCP_PORT 502

#define MODBUS_DATA_PREPARED    0x01
#define MODBUS_DATA_WRITTEN     0x02
#define MODBUS_DATA_RECEIVED    0x03
#define MODBUS_DATA_PROCESSED   0x04
#define MODBUS_DATA_NOT_RECVD   0x05
#define MODBUS_DATA_READ_ERROR  0x06

#define MODBUS_DATA_ERROR       0xFF

#define MAX_REGISTER_LIMIT  0x04

#define MODBUSSLAVE_HOLDING_REG_START_INDEX 0x00
#define MODBUSSLAVE_HOLDING_REG_END_INDEX   0x03

#define MODBUSSLAVE_HOLDING_REG_SIZE       0x04

#define MODBUSSLAVE_INPUT_REG_START_INDEX 1000
#define MODBUSSLAVE_INPUT_REG_END_INDEX   1003

#define MODBUSSLAVE_INPUT_REG_SIZE       0x04

#define MODBUSSLAVE_COIL_STATUS_START_INDEX 0
#define MODBUSSLAVE_COIL_STATUS_END_INDEX   3

#define MODBUSSLAVE_COIL_STATUS_SIZE       0x04

#define MODBUSSLAVE_INPUT_STATUS_START_INDEX 0
#define MODBUSSLAVE_INPUT_STATUS_END_INDEX   3

#define MODBUSSLAVE_INPUT_STATUS_SIZE       0x04

#define SLAVEID_VALIDATION  1
#define TCP_SLAVE_ID        1


class ModbusTCPSlave{
public:
    uint16_t ui16ModbusRxCounter=0;
    uint8_t ui8ModbusSlaveId=10;
    uint16_t ui16ModbusSlaveReadResult=0;
    uint16_t ui16ModbusSlaveProcessResult=0;
    uint16_t ui16ModbusSlaveWriteResult=0;

    uint16_t aui16HoldingRegister[MAX_REGISTER_LIMIT];
    uint16_t aui16InputRegister[MAX_REGISTER_LIMIT];
    uint8_t  ui8CoilStatus=0;
    uint8_t  ui8InputStatus=0;
    
    String sWebServerModbusRxCopy;
    String sWebServerModbusTxCopy;
    uint8_t ui8LastFunCodeRequested=0;
    uint8_t ui8ModbusErrorCode=0;

    uint16_t ui16ModbusRxCount=0;
    uint16_t ui16ModbusTxCount=0;

    uint16_t ui16TranscationId;
    uint16_t ui16ProtocolId;
    
    uint16_t ui16ModbusRxByteCounter;

    bool bSlaveIdValidation = SLAVEID_VALIDATION;
    uint8_t  ui8TCPSlaveId = TCP_SLAVE_ID ;


    ModbusTCPSlave();
    void Start(void);
    void Read(void);
    void Process(void);
    void Write(void);
    void Update(void);
    uint16_t Is_DataReceived(void);
    void Process_Holding_Register(void);
    void Process_Input_Register(void);
    void Process_Coil_Status(void);
    void Process_Input_Status(void);
    void Process_Force_Single_Coil(void);
    void Process_Force_Multiple_Coil(void);
    void Process_Preset_Single_Register(void);
    void Process_Preset_Multiple_Register(void);

    void Exception_Handling(uint8_t ui8ErrorCode, uint8_t ui8ErrorValue);
    void Process_ErrorCode(uint8_t ui8FunctionError, uint8_t ui8ErrorCode);
private:
    WiFiServer MBServer;
    WiFiClient client;    
};

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_TWOWIRE)
extern ModbusTCPSlave ModTCPS;
#endif
#endif