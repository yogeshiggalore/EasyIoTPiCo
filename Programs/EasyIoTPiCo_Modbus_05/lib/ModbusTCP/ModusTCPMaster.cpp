/*******************************************************************************
* File Name: ModbusTCPMaster.cpp
*
* Version: 1.0
*
* Description:
* This is source file for modbus tcp master functions
*
* Owner:
* Yogesh M Iggalore
*
* Code Tested With:
* 1. platformio and esp32
*
********************************************************************************
* Copyright (2020-21) , METI
********************************************************************************/
#include <Arduino.h>
#include <ModbusTCPMaster.h>
#include <ModbusUtils.h>

// Constructors ////////////////////////////////////////////////////////////////
ModbusTCPMaster::ModbusTCPMaster(void):MBServer(MODBUS_TCP_PORT),client(){}

void ModbusTCPMaster::Start(void){
    MBServer.begin();
    client = MBServer.available();
    Serial.println("Modbus TCPIP Master Started");
}

void ModbusTCPMaster::Prepare(void){
    ui16TransactionId++;

    /* transcation id*/
    aui8ModbusTCPTxBuffer[0] = ui16TransactionId >> 8;
    aui8ModbusTCPTxBuffer[1] = ui16TransactionId;

    /* protocol id*/
    aui8ModbusTCPTxBuffer[2] = 0;
    aui8ModbusTCPTxBuffer[3] = 0;

    /* packet length */
    aui8ModbusTCPTxBuffer[4] = 0;
    aui8ModbusTCPTxBuffer[5] = 6;

    /* packet */
    aui8ModbusTCPTxBuffer[6] = 1;
    aui8ModbusTCPTxBuffer[7] = 3;
    aui8ModbusTCPTxBuffer[8] = 0;
    aui8ModbusTCPTxBuffer[9] = 0;
    aui8ModbusTCPTxBuffer[10] = 0;
    aui8ModbusTCPTxBuffer[11] = 64;

    ui16ModbusTCPPrepareResult = 12;

}

void ModbusTCPMaster::Write(void){
    uint8_t ui8ByteSize=0;
    uint8_t ui8DebugLoopCounter=0;

    ui8ByteSize = ui16ModbusTCPPrepareResult;
    if(ui8ByteSize > 0){
        Serial.printf("T:%d S:%d F:%d \n",lTxCount,lSucessCount,lFailCount);
        lTxCount++;
        client.stop();
        client.connect(slaveIp, slavePort);
        if(client.connected()){
            client.write((const uint8_t *)aui8ModbusTCPTxBuffer,ui8ByteSize);
            Serial.print("TCPTx:");
            while(Serial.availableForWrite() < 127);
            for(ui8DebugLoopCounter=0;ui8DebugLoopCounter<ui8ByteSize;ui8DebugLoopCounter++){
                Serial.print(aui8ModbusTCPTxBuffer[ui8DebugLoopCounter]);
                Serial.print(" ");
            }
            while(Serial.availableForWrite() < 127);
            Serial.println();
        }else{
            Serial.print("Failed to connect server:");
            lFailCount++;
        }

        ui16ModbusRxByteCounter = 0;
        ui16ModbusTCPReadResult = 0;
        ui16ModbusTCPProcessResult = 0;
        ui16ModbusTCPWriteResult = 0;
        ui16ModbusTCPPrepareResult = 0;
    }

}

void ModbusTCPMaster::Read(void){
    uint8_t ui8DebugLoopCounter;
    uint16_t ui16LoopCounter;
    
    if(client.connected()){
        if(client.available()){
            while(client.available()){
                aui8ModbusTCPRxBuffer[ui16ModbusRxByteCounter++] = client.read();
            }
            client.flush();
            lSucessCount++;
            if(ui16ModbusRxByteCounter > 6){
                Serial.print("TCPRx:");
                while(Serial.availableForWrite() < 127);
                for(ui8DebugLoopCounter=0;ui8DebugLoopCounter<ui16ModbusRxByteCounter;ui8DebugLoopCounter++){
                    Serial.print(aui8ModbusTCPRxBuffer[ui8DebugLoopCounter]);
                    Serial.print(" ");
                }
                while(Serial.availableForWrite() < 127);
                Serial.println();
            }
        }
    }else{
        //client = MBServer.available();
    }
}

void ModbusTCPMaster::Handle(void){
    Write();
    Read();
    //Process();
}

uint8_t ModbusTCPMaster::Exceute(IPAddress IPAddr, uint16_t ui16Port, uint16_t ui16Timeout, uint8_t aui8TxBuffer[], uint8_t ui8TxByteCount,uint16_t* ui16RxByteCount,uint16_t *ResponseTimeout){
    uint8_t ui8LoopCounter=0;
    uint8_t ui8Response=0;
    uint8_t ui8DebugLoopCounter;
    uint16_t ui16TimeCounter=0;
    uint8_t  ui8CompletFlag=0;
    uint8_t  ui8ExpectedBytes=0;
    uint8_t  ui8ModbusResponse;
    
    ui8Response = ModUtils.Modbus_Verify_FunctionCode(aui8TxBuffer[1]);
    if(ui8Response != 1){
        return MODBUS_RTU_ERR_TIMEOUT;
    }
    
    ui8ExpectedBytes = ModUtils.Modbus_ExpectedBytes_TCPIP(aui8TxBuffer);
    ui8ExpectedBytes = ui8ExpectedBytes + 6;
    Serial.print("Expeced Bytes:");
    Serial.println(ui8ExpectedBytes);
    ui16TransactionId++;

    /* transcation id*/
    aui8ModbusTCPTxBuffer[0] = ui16TransactionId >> 8;
    aui8ModbusTCPTxBuffer[1] = ui16TransactionId;

    /* protocol id*/
    aui8ModbusTCPTxBuffer[2] = 0;
    aui8ModbusTCPTxBuffer[3] = 0;

    /* packet length */
    aui8ModbusTCPTxBuffer[4] = 0;
    aui8ModbusTCPTxBuffer[5] = ui8TxByteCount;

    /* packet */
    for(ui8LoopCounter = 6; ui8LoopCounter <(ui8TxByteCount + 6);ui8LoopCounter++){
        aui8ModbusTCPTxBuffer[ui8LoopCounter] = aui8TxBuffer[ui8LoopCounter-6];
    }

    ui16ModbusTCPPrepareResult = ui8TxByteCount + 6;

    Serial.printf("T:%d S:%d F:%d \n",lTxCount,lSucessCount,lFailCount);
    lTxCount++;
    client.stop();
    client.connect(IPAddr, ui16Port);
    ui16ModbusRxByteCounter = 0;
    
    for(ui8DebugLoopCounter=0;ui8DebugLoopCounter<ui8ExpectedBytes;ui8DebugLoopCounter++){
        ModUtils.aui8ModbusRxBuffer[ui8DebugLoopCounter] = 0;
    }

    if(client.connected()){
        client.write((const uint8_t *)aui8ModbusTCPTxBuffer,ui16ModbusTCPPrepareResult);
        Serial.print("TCPTx:");
        while(Serial.availableForWrite() < 127);
        for(ui8DebugLoopCounter=0;ui8DebugLoopCounter<ui16ModbusTCPPrepareResult;ui8DebugLoopCounter++){
            Serial.print(aui8ModbusTCPTxBuffer[ui8DebugLoopCounter]);
            Serial.print(" ");
        }
        while(Serial.availableForWrite() < 127);
        Serial.println();
        while(ui8CompletFlag == 0){
            if(client.available()){
                while(client.available()){
                    ModUtils.aui8ModbusRxBuffer[ui16ModbusRxByteCounter++] = client.read();
                }
                client.flush();
                Serial.print("Rx count:");
                Serial.println(ui16ModbusRxByteCounter);
                if(ui16ModbusRxByteCounter >= ui8ExpectedBytes){
                    ui8CompletFlag = 1;
                    lSucessCount++;
                    ui8ModbusResponse = MODBUS_RTU_ERR_NONE;
                if(ui16ModbusRxByteCounter > 6){
                        Serial.print("TCPRx:");
                        while(Serial.availableForWrite() < 127);
                        for(ui8DebugLoopCounter=0;ui8DebugLoopCounter<ui16ModbusRxByteCounter;          ui8DebugLoopCounter++){
                            Serial.print(ModUtils.aui8ModbusRxBuffer[ui8DebugLoopCounter]);
                            Serial.print(" ");
                        }
                        while(Serial.availableForWrite() < 127);
                        Serial.println();
                    }
                }
            }else{
                delay(1);
                ui16TimeCounter++;
                if(ui16TimeCounter > ui16Timeout){
                    ui8CompletFlag = 1;
                }
            }
        }
    }else{
        Serial.print("Failed to connect server:");
        lFailCount++;
        ui8ModbusResponse = MODBUS_RTU_ERR_TIMEOUT;
    }

    *ui16RxByteCount = ui16ModbusRxByteCounter;

    return ui8ModbusResponse;
}

// Preinstantiate Objects //////////////////////////////////////////////////////
#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_TWOWIRE)
ModbusTCPMaster ModTCPM;
#endif