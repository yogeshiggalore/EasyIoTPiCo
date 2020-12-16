/*******************************************************************************
* File Name: ModbusASCIIMaster.cpp
*
* Version: 1.0
*
* Description:
* This is source file for modbus ascii master. All the functions related to
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

#include <Arduino.h>
#include <ModbusASCIIMaster.h>
#include <ModbusUtils.h>
#include <ESPUtils.h>

// Constructors ////////////////////////////////////////////////////////////////
ModbusASCIIMaster::ModbusASCIIMaster(){}

void ModbusASCIIMaster::Get_Config_Parameters_From_Memory(void){

}

void ModbusASCIIMaster::Start(void){

    Get_Config_Parameters_From_Memory();

    Serial.begin(ModUtils.Modbus_Get_Baudrate(ui8ModbusBaudrate),
                    ModUtils.Get_UART_Config(ui8ModbusDatabits,ui8ModbusParity,ui8ModbusStopbit));
    
    //pinMode(MODBUS_ENABLE_PIN,OUTPUT);
    //pinMode(MODBUS_LED_PIN,OUTPUT);
    //digitalWrite(MODBUS_ENABLE_PIN,LOW);
    //digitalWrite(MODBUS_LED_PIN,LOW); 
    Prepare_Packet();
}

void ModbusASCIIMaster::Prepare_Packet(void){

    if(uint8ModbusIntervalFlag){
        uint8ModbusIntervalFlag = 0;
        ModUtils.aui8ModbusTxBuffer[0] = 0x01;
        ModUtils.aui8ModbusTxBuffer[1] = 0x03;
        ModUtils.aui8ModbusTxBuffer[2] = 0x00;
        ModUtils.aui8ModbusTxBuffer[3] = 0x00;
        ModUtils.aui8ModbusTxBuffer[4] = 0x00;
        ModUtils.aui8ModbusTxBuffer[5] = 0x0A;
        ModUtils.aui8ModbusTxBuffer[6] = 0xC5;
        ModUtils.aui8ModbusTxBuffer[7] = 0xCD;

        ui16ModbusPrepareResult = MODBUS_DATA_PREPARED;
        ui16ModbusPrepareResult = ui16ModbusPrepareResult << 8;
        ui16ModbusPrepareResult = ui16ModbusPrepareResult | 0x08;

        ui8ModbusExpectedSlaveid = ModUtils.aui8ModbusTxBuffer[0];
        ui16ModbusExpectedBytes = ModUtils.Modbus_ExpectedBytes_RTU(ModUtils.aui8ModbusTxBuffer);
    }
}

void ModbusASCIIMaster::Read(void){
    uint16_t ui16Response;
    uint8_t ui8Response;
    uint8_t ui8LoopCounter;

    ui16Response = Is_Slave_DataReceived();

    ui8Response = ui16Response >> 8;

    if(ui8Response){
        ui8Response = ui16Response;
        ui16ModbusReadResult = MODBUS_DATA_RECEIVED;
        ui16ModbusReadResult = ui16ModbusReadResult << 8;
        ui16ModbusReadResult = ui16ModbusReadResult | ui8Response;
        ui16ModbusRxCounter = 0;
        //Serial1.print("Modbus Error code ");
        //Serial1.print(ui8ModbusErrorCode,HEX);
        Serial1.print("Modbus Rx: ");
        for(ui8LoopCounter=0;ui8LoopCounter<ui8Response;ui8LoopCounter++){
            Serial1.print(ModUtils.aui8ModbusRxBuffer[ui8LoopCounter],HEX);
            Serial1.print(" ");
        }
        Serial1.println(" ");
    }
}

void ModbusASCIIMaster::Process(void){
    uint16_t ui16Response;
    uint8_t ui8Response;

    ui8Response = ui16ModbusReadResult >> 8;

    if(ui8Response == MODBUS_DATA_RECEIVED){
        if(ui8ModbusErrorCode == MODBUS_ASCII_ERR_NONE){
            
        }
    }
}

void ModbusASCIIMaster::Write(void){
    uint8_t ui8FlagValue=0;
    uint8_t ui8LoopCounter=0;

    ui8FlagValue = ui16ModbusPrepareResult >> 8;
    if(ui8FlagValue == MODBUS_DATA_PREPARED){
        ui8FlagValue = ui16ModbusPrepareResult;
        if(ui8FlagValue){
            //digitalWrite(MODBUS_ENABLE_PIN,HIGH);
            //digitalWrite(MODBUS_LED_PIN,HIGH);
            Serial1.print("Modbus Tx: ");
            for(ui8LoopCounter=0;ui8LoopCounter<ui8FlagValue;ui8LoopCounter++){
                Serial.write(ModUtils.aui8ModbusTxBuffer[ui8LoopCounter]);
                while(Serial.availableForWrite() < 127);
                Serial1.print(ModUtils.aui8ModbusTxBuffer[ui8LoopCounter],HEX);
                Serial1.print(" ");
            }
            Serial1.println(" ");
            delay(1);
            //digitalWrite(MODBUS_LED_PIN,LOW);
            //digitalWrite(MODBUS_ENABLE_PIN,LOW);
        }
        
        ui16ModbusPrepareResult = 0;
        ui16ModbusTimeoutCounter = 0;
        uint8ModbusTimeoutFlag = 0;
        ui8ModbusTaskCompleteFlag = 0;
    }
}

uint16_t ModbusASCIIMaster::Is_Slave_DataReceived(void){
    uint8_t ui8BufferSize;
    uint8_t ui8LoopCounter=0;
    uint16_t ui16Result=0;
    uint8_t ui8ModbusResponse=0;
    uint16_t ui16Response=0;
    uint8_t ui8CRCLow=0;
    uint8_t ui8CRCHigh=0;

    ui8BufferSize = Serial.available();    
    for(ui8LoopCounter=0;ui8LoopCounter<ui8BufferSize;ui8LoopCounter++){
        ModUtils.aui8ModbusRxBuffer[ui16ModbusRxCounter] = Serial.read();
        ui16ModbusRxCounter++;
    }
    
    if(ModUtils.aui8ModbusRxBuffer[0] != MODBUS_ASCII_START_BYTE){
        ui16ModbusRxCounter = 0;
    }else{
        if(ModUtils.aui8ModbusRxBuffer[ui16ModbusRxCounter] == MODBUS_ASCII_CR_BYTE){
            if(ModUtils.aui8ModbusRxBuffer[ui16ModbusRxCounter-1] == MODBUS_ASCII_LF_BYTE){
                
            }
        }
    }

    ui16Result = ui8ModbusResponse;
    ui16Result = ui16Result << 8;
    ui16Result = ui16Result | ui16ModbusRxCounter;
    return  ui16Result;
}

void IRAM_ATTR ModbusASCIIMaster::Check_Modbus_Timer(void){
    ui16ModbusIntervalCounter++;
    ui16ModbusTimeoutCounter++;

    if(ui16ModbusIntervalCounter > MODBUS_READ_INTERVAL){
        ui16ModbusIntervalCounter = 0;
        uint8ModbusIntervalFlag=1;
    }

    if(ui16ModbusTimeoutCounter > MODBUS_TIMEOUT){
        ui16ModbusTimeoutCounter = 0;
        uint8ModbusTimeoutFlag=1;
    }
}

void ModbusASCIIMaster::Update(void){
    Prepare_Packet();
    Read();
    Process();
    Write();
}

uint8_t ModbusASCIIMaster::Modbus_RunCommand(uint8_t aui8Buffer[],uint8_t ui8ByteToSent,uint8_t BytesExpected,uint8_t* RxByteCount, uint16_t TimeOutIntervalInMs, uint16_t *ResponseTimeout){
	uint16_t msloopCount;
    uint16_t ui16CRC;
    uint8_t ui8LoopCounter;
    uint8_t ui8Low;
    uint8_t ui8High;

	memset(ModUtils.aui8ModbusRxBuffer, 0, MODBUS_RX_BUFFER_SIZE);
        
	ui16CRC = ModUtils.Modbus_CalculateCRC16(aui8Buffer, ui8ByteToSent);
	aui8Buffer[ui8ByteToSent] = ui16CRC >> 8;
	aui8Buffer[ui8ByteToSent+1] = ui16CRC;
	
    //digitalWrite(MODBUS_ENABLE_PIN, HIGH);
    //digitalWrite(MODBUS_LED_PIN, HIGH);
    
    Serial.print("Modbus Tx: ");
    for(ui8LoopCounter=0;ui8LoopCounter<(ui8ByteToSent+2);ui8LoopCounter++){
        Serial.write(aui8Buffer[ui8LoopCounter]);
        Serial1.print(aui8Buffer[ui8LoopCounter],HEX);
        Serial1.print(" ");
        while(Serial.availableForWrite() < 127);
    }
    Serial1.println(" ");

    memset(ModUtils.aui8ModbusRxBuffer, 0, MODBUS_RX_BUFFER_SIZE);
    
    //digitalWrite(MODBUS_ENABLE_PIN, LOW);
    //digitalWrite(MODBUS_LED_PIN, LOW);

	for(msloopCount = 0 ; msloopCount < (TimeOutIntervalInMs + (BytesExpected * 1)) ; msloopCount++){
		delay(1); //1 ms delay
		
		//All expeted bytes are received, exit loop
		if(Serial.available()){
            //digitalWrite(MODBUS_LED_PIN, HIGH);
        }

        if(Serial.available() >= BytesExpected){
			delay(1);
            break;
		}
	}
	
    //digitalWrite(MODBUS_LED_PIN, LOW);
    
	*ResponseTimeout = msloopCount;
    
	// Read the Rx buffer size
	*RxByteCount = Serial.available();
	
	// If *RxByteCount is less than 4, then we did not receive a complete response packet
	if(*RxByteCount < 4){
        for(ui8LoopCounter=0;ui8LoopCounter<4;ui8LoopCounter++){
            Serial.read();
        }

        return MODBUS_ASCII_ERR_TIMEOUT;
	}
    
	//Transfer the full content to a Global Buffer, even if error exists.... this is for debug
	Serial1.print("Modbus Rx: ");
    for(ui8LoopCounter=0;ui8LoopCounter<*RxByteCount;ui8LoopCounter++){
        ModUtils.aui8ModbusRxBuffer[ui8LoopCounter] = Serial.read();
        Serial1.print(ModUtils.aui8ModbusRxBuffer[ui8LoopCounter],HEX);
    }
    Serial1.println(" ");
    
	// Check if the received SlaveAddress is equal to transmitted slave address
	if(ModUtils.aui8ModbusRxBuffer[0] != aui8Buffer[0]){
		return MODBUS_ASCII_ERR_TIMEOUT;
	}

	// Calculate CRC for the received data
	ui16CRC = ModUtils.Modbus_CalculateCRC16(ModUtils.aui8ModbusRxBuffer, *RxByteCount-2);
    ui8High = ui16CRC>>8;
    ui8Low = ui16CRC;

	// Check if calculated CRC matches received CRC
	if ((ui8High != ModUtils.aui8ModbusRxBuffer[*RxByteCount-2]) || (ui8Low!= ModUtils.aui8ModbusRxBuffer[*RxByteCount-1])){
		return MODBUS_ASCII_ERR_CRC;
	}

    if ( ModUtils.aui8ModbusRxBuffer[1] != aui8Buffer[1] ){
    	if ( ModUtils.aui8ModbusRxBuffer[1] == (0x80 | aui8Buffer[1]) ){
            if (ModUtils.aui8ModbusRxBuffer[2] == 0x01){
                return MODBUS_ASCII_ERR_INVALID_FUNC;
            }
            else if (ModUtils.aui8ModbusRxBuffer[2] == 0x02){
                return MODBUS_ASCII_ERR_INVALID_REG_ADDRESS;
            }
            else{
                return MODBUS_ASCII_ERR_INVALID_MODBUS_COMMAND;
            }      
        }
        else{
            return MODBUS_ASCII_ERR_INVALID_MODBUS_COMMAND;
        }
    }
		
	return MODBUS_ASCII_ERR_NONE;
}

uint8_t ModbusASCIIMaster::Modbus_RunCommand_ASCII(uint8_t aui8Buffer[],uint8_t ui8ByteToSent,uint8_t BytesExpected,uint8_t* RxByteCount, uint16_t TimeOutIntervalInMs, uint16_t *ResponseTimeout){
	uint16_t msloopCount;
    char cLRC;
    uint16_t ui16CRC;
    uint8_t ui8LoopCounter;
    uint8_t ui8Low;
    uint8_t ui8High;

	memset(ModUtils.aui8ModbusRxBuffer, 0, MODBUS_RX_BUFFER_SIZE);
        
	cLRC = ModUtils.Modbus_ASCII_LRC(aui8Buffer, ui8ByteToSent);
    Serial1.print("LRC:");
    Serial1.print(cLRC);
    
	aui8Buffer[ui8ByteToSent] = ui16CRC >> 8;
	aui8Buffer[ui8ByteToSent+1] = ui16CRC;
	
    //digitalWrite(MODBUS_ENABLE_PIN, HIGH);
    //digitalWrite(MODBUS_LED_PIN, HIGH);
    
    Serial1.print("Modbus Tx: ");
    for(ui8LoopCounter=0;ui8LoopCounter<(ui8ByteToSent+2);ui8LoopCounter++){
        Serial.write(aui8Buffer[ui8LoopCounter]);
        Serial1.print(aui8Buffer[ui8LoopCounter],HEX);
        Serial1.print(" ");
        while(Serial.availableForWrite() < 127);
    }
    Serial1.println(" ");

    memset(ModUtils.aui8ModbusRxBuffer, 0, MODBUS_RX_BUFFER_SIZE);
    
    //digitalWrite(MODBUS_ENABLE_PIN, LOW);
    //digitalWrite(MODBUS_LED_PIN, LOW);

	for(msloopCount = 0 ; msloopCount < (TimeOutIntervalInMs + (BytesExpected * 1)) ; msloopCount++){
		delay(1); //1 ms delay
		
		//All expeted bytes are received, exit loop
		if(Serial.available()){
            //digitalWrite(MODBUS_LED_PIN, HIGH);
        }

        if(Serial.available() >= BytesExpected){
			delay(1);
            break;
		}
	}
	
    //digitalWrite(MODBUS_LED_PIN, LOW);
    
	*ResponseTimeout = msloopCount;
    
	// Read the Rx buffer size
	*RxByteCount = Serial.available();
	
	// If *RxByteCount is less than 4, then we did not receive a complete response packet
	if(*RxByteCount < 4){
        for(ui8LoopCounter=0;ui8LoopCounter<4;ui8LoopCounter++){
            Serial.read();
        }

        return MODBUS_ASCII_ERR_TIMEOUT;
	}
    
	//Transfer the full content to a Global Buffer, even if error exists.... this is for debug
	Serial1.print("Modbus Rx: ");
    for(ui8LoopCounter=0;ui8LoopCounter<*RxByteCount;ui8LoopCounter++){
        ModUtils.aui8ModbusRxBuffer[ui8LoopCounter] = Serial.read();
        Serial1.print(ModUtils.aui8ModbusRxBuffer[ui8LoopCounter],HEX);
    }
    Serial1.println(" ");
    
	// Check if the received SlaveAddress is equal to transmitted slave address
	if(ModUtils.aui8ModbusRxBuffer[0] != aui8Buffer[0]){
		return MODBUS_ASCII_ERR_TIMEOUT;
	}

	// Calculate CRC for the received data
	ui16CRC = ModUtils.Modbus_CalculateCRC16(ModUtils.aui8ModbusRxBuffer, *RxByteCount-2);
    ui8High = ui16CRC>>8;
    ui8Low = ui16CRC;

	// Check if calculated CRC matches received CRC
	if ((ui8High != ModUtils.aui8ModbusRxBuffer[*RxByteCount-2]) || (ui8Low!= ModUtils.aui8ModbusRxBuffer[*RxByteCount-1])){
		return MODBUS_ASCII_ERR_CRC;
	}

    if ( ModUtils.aui8ModbusRxBuffer[1] != aui8Buffer[1] ){
    	if ( ModUtils.aui8ModbusRxBuffer[1] == (0x80 | aui8Buffer[1]) ){
            if (ModUtils.aui8ModbusRxBuffer[2] == 0x01){
                return MODBUS_ASCII_ERR_INVALID_FUNC;
            }
            else if (ModUtils.aui8ModbusRxBuffer[2] == 0x02){
                return MODBUS_ASCII_ERR_INVALID_REG_ADDRESS;
            }
            else{
                return MODBUS_ASCII_ERR_INVALID_MODBUS_COMMAND;
            }      
        }
        else{
            return MODBUS_ASCII_ERR_INVALID_MODBUS_COMMAND;
        }
    }
		
	return MODBUS_ASCII_ERR_NONE;
}

// Preinstantiate Objects //////////////////////////////////////////////////////
#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_TWOWIRE)
ModbusASCIIMaster Modbus;
#endif