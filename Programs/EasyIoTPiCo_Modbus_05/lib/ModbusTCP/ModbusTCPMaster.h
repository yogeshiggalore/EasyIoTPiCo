/*******************************************************************************
*File Name: ModbusTCPMaster.h
*
* Version: 1.0
*
* Description:
* In this header file for ModbusTCPMaster functions
*
*
* Owner:
* Yogesh M Iggalore
*
********************************************************************************
* Copyright (2020-21) , METI M2M India Pvt Ltd
*******************************************************************************/
#ifndef ModbuTCPMaster_h
#define ModbuTCPMaster_h

#include <Arduino.h>
#include <WiFiClient.h>
#include <WiFiServer.h>

#define MODBUS_TCP_PORT 502

class ModbusTCPMaster{
public:
    ModbusTCPMaster();
    uint8_t ip[4] = { 192,168,0,177};
    uint8_t slaveIp[4] = {192,168,0,100};
    uint16_t slavePort = 502;

    uint8_t  aui8ModbusTCPRxBuffer[300];
    uint8_t  aui8ModbusTCPTxBuffer[64];

    uint16_t ui16ModbusTCPPrepareResult;
    uint16_t ui16ModbusTCPReadResult;
    uint16_t ui16ModbusTCPProcessResult;
    uint16_t ui16ModbusTCPWriteResult;

    uint16_t ui16ExpectedBytes;
    uint16_t ui16ModbusRxByteCounter=0;
    
    uint16_t ui16TransactionId=0;
    long lSucessCount=0;
    long lFailCount=0;
    long lTxCount;

    void Start(void);
    void Prepare(void);
    void Write(void);
    void Read(void);
    void Handle(void);
    uint8_t Exceute(IPAddress IPAddr, uint16_t ui16Port, uint16_t ui16Timeout, uint8_t aui8TxBuffer[], uint8_t ui8TxByteCount,uint16_t* ui16RxByteCount,uint16_t *ResponseTimeout);
private:
    WiFiServer MBServer;
    WiFiClient client;
};

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_TWOWIRE)
extern ModbusTCPMaster ModTCPM;
#endif
#endif