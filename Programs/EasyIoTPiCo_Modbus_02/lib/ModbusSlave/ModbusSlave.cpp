/*******************************************************************************
* File Name: ModbusSlave.cpp
*
* Version: 1.0
*
* Description:
* This is source file for modbus slave. All the functions related to
* Modbus slave are implemented in this file
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
#include <ModbusSlave.h>
#include <ModbusUtils.h>
#include <ESPUtils.h>
#include <EEPROM.h>
#include <ESPMemory.h>
#include <ESPRead.h>

// Constructors ////////////////////////////////////////////////////////////////
ModbusSlave::ModbusSlave(){}

void ModbusSlave::Start(void){

    Serial.begin(ModUtils.Modbus_Get_Baudrate(4),ModUtils.Get_UART_Config(8,0,1));
    
    //pinMode(MODBUS_ENABLE_PIN,OUTPUT);
    //pinMode(MODBUS_LED_PIN,OUTPUT);
    //digitalWrite(MODBUS_ENABLE_PIN,LOW);
    //digitalWrite(MODBUS_LED_PIN,LOW); 
}

void ModbusSlave::Update(void){
    Read();
    Process();
    Write();
}

/****************************************************************************** 
* Function Name: Read
*******************************************************************************
*
* Summary:
*  This function call internally ModbusSlave_Is_DataReceived function.
*  Based on return values, sets ui16ModbusSlaveReadResult
*
*  
*
* Parameters:  
*  None
*
* Return: 
*  None
*  
*******************************************************************************/

void ModbusSlave::Read(void){
    uint8_t  ui8Response;
    uint16_t ui16Result;
    uint8_t ui8LoopCounter=0;

    ui16Result = Is_DataReceived();
    ui8Response = ui16Result >> 8;
    if(ui8Response == 1){
        ui16ModbusSlaveReadResult = MODBUS_DATA_RECEIVED;
        ui16ModbusSlaveReadResult = ui16ModbusSlaveReadResult << 8;
        ui8Response = ui16Result;
        ui16ModbusSlaveReadResult = ui16ModbusSlaveReadResult | ui8Response;
        Serial1.print("Modbus Rx: ");
        sWebServerModbusRxCopy = " ";
        ui8LastFunCodeRequested = ModUtils.aui8ModbusRxBuffer[1];
        for(ui8LoopCounter=0;ui8LoopCounter<ui8Response;ui8LoopCounter++){
            Serial1.print(ModUtils.aui8ModbusRxBuffer[ui8LoopCounter],HEX);
            Serial1.print(" ");
            sWebServerModbusRxCopy = sWebServerModbusRxCopy + String(ModUtils.aui8ModbusRxBuffer[ui8LoopCounter],HEX) + " ";
        }
        Serial1.println(" ");
        ui16ModbusRxCount++;
    }
    else{
        ui16ModbusSlaveReadResult = MODBUS_DATA_NOT_RECVD;
        ui16ModbusSlaveReadResult = ui16ModbusSlaveReadResult << 8;
        ui8Response = 0;
        ui16ModbusSlaveReadResult = ui16ModbusSlaveReadResult | ui8Response;
    }
    
}

/****************************************************************************** 
* Function Name: Process
*******************************************************************************
*
* Summary:
*  This function checks for MODBUS_DATA_RECEIVED and process the command.
*  Based on return values, sets ui16ModbusSlaveProcessResult
*
*  
*
* Parameters:  
*  None
*
* Return: 
*  None
*  
*******************************************************************************/

void ModbusSlave::Process(void){
    uint8_t  ui8Response;
    uint8_t  ui8BufferSize=0;
    uint16_t ui16Response=0;
    uint8_t  ui8CRCLow=0;
    uint8_t  ui8CRCHigh=0;
    
    ui8Response = ui16ModbusSlaveReadResult >> 8;
    if(ui8Response == MODBUS_DATA_RECEIVED){
        ui8BufferSize = ui16ModbusSlaveReadResult; // get buffer size in lower 8 bits
        ui16Response = ModUtils.Modbus_CalculateCRC16(ModUtils.aui8ModbusRxBuffer, ui8BufferSize-2);
        ui8CRCLow = ui16Response;
        ui8CRCHigh = ui16Response >> 8;
        if((ModUtils.aui8ModbusRxBuffer[ui8BufferSize-1] == ui8CRCLow) & ((ModUtils.aui8ModbusRxBuffer[ui8BufferSize-2] == ui8CRCHigh))){
            if(ModUtils.aui8ModbusRxBuffer[0] == ui8ModbusSlaveId){
                if(ModUtils.aui8ModbusRxBuffer[1] == MODBUS_FUNC_READ_COIL_REG){
                    Process_Coil_Status();
                }else if(ModUtils.aui8ModbusRxBuffer[1] == MODBUS_FUNC_READ_INPUT_STATUS_REG){
                    Process_Input_Status();
                }else if(ModUtils.aui8ModbusRxBuffer[1] == MODBUS_FUNC_READ_HOLDING_REG){
                    Process_Holding_Register();
                }else if(ModUtils.aui8ModbusRxBuffer[1] == MODBUS_FUNC_READ_INPUT_REG){
                    Process_Input_Register();
                }else if(ModUtils.aui8ModbusRxBuffer[1] == MODBUS_FUNC_WRITE_SINGLE_COIL){
                    Process_Force_Single_Coil();
                }else if(ModUtils.aui8ModbusRxBuffer[1] == MODBUS_FUNC_WRITE_SINGLE_REG){   
                    Process_Preset_Single_Register();
                }else if(ModUtils.aui8ModbusRxBuffer[1] == MODBUS_FUNC_WRITE_MULTI_COIL){
                    Process_Force_Multiple_Coil();
                }else if(ModUtils.aui8ModbusRxBuffer[1] == MODBUS_FUNC_WRITE_MULTI_REG){
                    Process_Preset_Multiple_Register();
                }else{
                    ui16ModbusSlaveProcessResult = MODBUS_RTU_ERR_INVALID_FUNC;
                    ui16ModbusSlaveProcessResult = ui16ModbusSlaveProcessResult << 8;
                    ui16ModbusSlaveProcessResult = ui16ModbusSlaveProcessResult | 5;//number of bytes to send...
                    Exception_Handling((0x80 | ModUtils.aui8ModbusRxBuffer[1]),0x01); // illegal function code
                    ui8ModbusErrorCode = MODBUS_RTU_ERR_INVALID_FUNC;
                }
            }else{
                ui16ModbusSlaveProcessResult = MODBUS_DATA_READ_ERROR;
                ui16ModbusSlaveProcessResult = ui16ModbusSlaveProcessResult << 8;
                ui16ModbusSlaveProcessResult = ui16ModbusSlaveProcessResult | 0;
            }    
        }else{
            Serial1.println("CRC error");
            ui8ModbusErrorCode = MODBUS_RTU_ERR_CRC;
        }
    }
}

/****************************************************************************** 
* Function Name: ModbusSlave_Process_Holding_Register
*******************************************************************************
*
* Summary:
*  This function process holding register request.
*   Example
*   Request
*
*   This command is requesting the content of analog output holding registers # 40108 to
*   40110 from the slave device with address 17.
*
*   11 03 006B 0003 7687
*
*   11: The Slave Address (11 hex = address17 )
*   03: The Function Code 3 (read Analog Output Holding Registers)
*   006B: The Data Address of the first register requested.( 006B hex = 107 , + 40001 offset = input #40108 )
*    0003: The total number of registers requested. (read 3 registers 40108 to 40110)  
*    7687: The CRC (cyclic redundancy check) for error checking.
*
*    Response
*
*    11 03 06 AE41 5652 4340 49AD
*
*    11: The Slave Address (11 hex = address17 )
*    03: The Function Code 3 (read Analog Output Holding Registers)
*    06: The number of data bytes to follow (3 registers x 2 bytes each = 6 bytes)
*    AE41: The contents of register 40108
*    5652: The contents of register 40109 
*    4340: The contents of register 40110
*    49AD: The CRC (cyclic redundancy check).
*  
*
* Parameters:  
*  None
*
* Return: 
*  None
*  
*******************************************************************************/
void ModbusSlave::Process_Holding_Register(void){
    uint16_t ui16StartRegister=0;
    uint16_t ui16NumberofRegister=0;
    uint8_t ui8ArrayCounter=0;
    uint8_t ui8LoopCounter=0;
    uint16_t ui16Result=0;
    uint8_t ui8Value=0;

    ui16StartRegister = ModUtils.aui8ModbusRxBuffer[2];
    ui16StartRegister = ui16StartRegister  << 8;
    ui16StartRegister = ui16StartRegister | ModUtils.aui8ModbusRxBuffer[3];
    
    ui16NumberofRegister = ModUtils.aui8ModbusRxBuffer[4];
    ui16NumberofRegister = ui16NumberofRegister  << 8;
    ui16NumberofRegister = ui16NumberofRegister |ModUtils.aui8ModbusRxBuffer[5];
    
    if((ui16NumberofRegister > MODBUSSLAVE_HOLDING_REG_SIZE) | (ui16NumberofRegister == 0)){
        Process_ErrorCode(0x83,0x2);
    }else{
        if(ui16StartRegister > (MODBUSSLAVE_HOLDING_REG_SIZE-1)){
            Process_ErrorCode(0x83,0x2);
        }else{
            if(ui16NumberofRegister > (MODBUSSLAVE_HOLDING_REG_SIZE - ui16StartRegister)){
                Process_ErrorCode(0x83,0x02); // illegal data address
            }else{
                ModUtils.aui8ModbusTxBuffer[ui8ArrayCounter++] = ui8ModbusSlaveId;
                ModUtils.aui8ModbusTxBuffer[ui8ArrayCounter++] = MODBUS_FUNC_READ_HOLDING_REG;
                ModUtils.aui8ModbusTxBuffer[ui8ArrayCounter++] = ui16NumberofRegister * 2;
                for(ui8LoopCounter=ui16StartRegister;ui8LoopCounter < (ui16StartRegister +  ui16NumberofRegister);ui8LoopCounter++){
                    ModUtils.aui8ModbusTxBuffer[ui8ArrayCounter++] = aui16HoldingRegister[ui8LoopCounter] >> 8;
                    ModUtils.aui8ModbusTxBuffer[ui8ArrayCounter++] = aui16HoldingRegister[ui8LoopCounter];
                }
                ui16Result = ModUtils.Modbus_CalculateCRC16(ModUtils.aui8ModbusTxBuffer,ui8ArrayCounter);
                ui8Value = ui16Result >> 8;
                ModUtils.aui8ModbusTxBuffer[ui8ArrayCounter++] = ui8Value;
                ModUtils.aui8ModbusTxBuffer[ui8ArrayCounter++] = ui16Result;
            
                ui16ModbusSlaveProcessResult = MODBUS_RTU_ERR_NONE;
                ui16ModbusSlaveProcessResult = ui16ModbusSlaveProcessResult << 8;
                ui16ModbusSlaveProcessResult = ui16ModbusSlaveProcessResult | ui8ArrayCounter;//number of bytes to send...
                ui8ModbusErrorCode = MODBUS_RTU_ERR_NONE;
            }
        }
    }
}

/****************************************************************************** 
* Function Name: ModbusSlave_Process_Input_Register
*******************************************************************************
*
* Summary:
*  This function process input register register request.
*   Example
*   Request
*
*   This command is requesting the content of analog input register # 30009
*   from the slave device with address 17.
*
*   11 04 0008 0001 B298
*
*   11: The Slave Address (11 hex = address17 )
*   04: The Function Code 4 (read Analog Input Registers)
*   0008: The Data Address of the first register requested.
*            ( 0008 hex = 8 , + 30001 offset = input register #30009 )
*   0001: The total number of registers requested. (read 1 register)  
*   B298: The CRC (cyclic redundancy check) for error checking.

*   Response

*   The normal response is an echo of the query, returned after the register contents have been written.

*   11 04 02 000A F8F4

*   11: The Slave Address (11 hex = address17 )
*   04: The Function Code 4 (read Analog Input Registers)
*   02: The number of data bytes to follow (1 registers x 2 bytes each = 2 bytes)
*   000A: The contents of register 30009
*   F8F4: The CRC (cyclic redundancy check).
*  
*
* Parameters:  
*  None
*
* Return: 
*  None
*  
*******************************************************************************/

void ModbusSlave::Process_Input_Register(void){
    uint16_t ui16StartRegister=0;
    uint16_t ui16NumberofRegister=0;
    uint8_t  ui8LoopCounter=0;
    uint8_t  ui8ArrayCounter=0;
    uint16_t ui16Result=0;
    uint8_t  ui8Value=0;
    
    ui16StartRegister = ModUtils.aui8ModbusRxBuffer[2];
    ui16StartRegister = ui16StartRegister  << 8;
    ui16StartRegister = ui16StartRegister | ModUtils.aui8ModbusRxBuffer[3];
    
    ui16NumberofRegister = ModUtils.aui8ModbusRxBuffer[4];
    ui16NumberofRegister = ui16NumberofRegister  << 8;
    ui16NumberofRegister = ui16NumberofRegister | ModUtils.aui8ModbusRxBuffer[5];
    
    if((ui16NumberofRegister > MODBUSSLAVE_INPUT_REG_SIZE) | (ui16NumberofRegister == 0)){
       Process_ErrorCode(0x84,0x2); // illegal data address
    }else{
        if(ui16StartRegister > MODBUSSLAVE_INPUT_REG_END_INDEX){
            Process_ErrorCode(0x84,0x2); // illegal data address
        }else{
            if(ui16NumberofRegister > (MODBUSSLAVE_INPUT_REG_END_INDEX + 1 - ui16StartRegister)){
                Process_ErrorCode(0x84,0x2); // illegal data address
            }
            else{
                if(ui16StartRegister < MODBUSSLAVE_INPUT_REG_START_INDEX){
                    Process_ErrorCode(0x84,0x2); // illegal data address
                }
                else{
                    ModUtils.aui8ModbusTxBuffer[ui8ArrayCounter++] = ui8ModbusSlaveId;
                    ModUtils.aui8ModbusTxBuffer[ui8ArrayCounter++] = MODBUS_FUNC_READ_INPUT_REG;
                    ModUtils.aui8ModbusTxBuffer[ui8ArrayCounter++] = ui16NumberofRegister * 2;
                    
                    for(ui8LoopCounter=(ui16StartRegister - MODBUSSLAVE_INPUT_REG_START_INDEX);ui8LoopCounter < ((ui16StartRegister - MODBUSSLAVE_INPUT_REG_START_INDEX) + ui16NumberofRegister);ui8LoopCounter++){
                        ModUtils.aui8ModbusTxBuffer[ui8ArrayCounter++] = aui16InputRegister[ui8LoopCounter] >> 8;
                        ModUtils.aui8ModbusTxBuffer[ui8ArrayCounter++] = aui16InputRegister[ui8LoopCounter];
                    }
                    ui16Result = ModUtils.Modbus_CalculateCRC16(ModUtils.aui8ModbusTxBuffer,ui8ArrayCounter);
                    ui8Value = ui16Result >> 8;
                    ModUtils.aui8ModbusTxBuffer[ui8ArrayCounter++] = ui8Value;
                    ModUtils.aui8ModbusTxBuffer[ui8ArrayCounter++] = ui16Result;
                    
                    ui16ModbusSlaveProcessResult = MODBUS_RTU_ERR_NONE;
                    ui16ModbusSlaveProcessResult = ui16ModbusSlaveProcessResult << 8;
                    ui16ModbusSlaveProcessResult = ui16ModbusSlaveProcessResult | ui8ArrayCounter;//number of bytes to send...
                    ui8ModbusErrorCode = MODBUS_RTU_ERR_NONE;
                }
            }
        }
        
    }
}

/****************************************************************************** 
* Function Name: ModbusSlave_Process_Coil_Status
*******************************************************************************
*
* Summary:
*  This function process input coils request.
*   Example
*   Request
*
*   This command is requesting the ON/OFF status of discrete coils # 20 to 56
*   from the slave device with address 17.
*
*   11 01 0013 0025 0E84
*
*   11: The Slave Address (11 hex = address17 )
*   01: The Function Code 1 (read Coil Status)
*   0013: The Data Address of the first coil to read.
*             ( 0013 hex = 19 , + 1 offset = coil #20 )
*   0025: The total number of coils requested.  (25 hex = 37,  inputs 20 to 56 )  
*   0E84: The CRC (cyclic redundancy check) for error checking.

*   Response

*   11 01 05 CD6BB20E1B 45E6

*   11: The Slave Address (11 hex = address17 )
*   01: The Function Code 1 (read Coil Status)
*   05: The number of data bytes to follow (37 Coils / 8 bits per byte = 5 bytes)
*   CD: Coils 27 - 20 (1100 1101)
*   6B: Coils 35 - 28 (0110 1011)
*   B2: Coils 43 - 36 (1011 0010)
*   0E: Coils 51 - 44 (0000 1110)
*   1B: 3 space holders & Coils 56 - 52 (0001 1011)
*   45E6: The CRC (cyclic redundancy check).

*   The more significant bits contain the higher coil variables. This shows that coil 
*   36 is off (0) and 43 is on (1). Due to the number of coils requested, the last data field1B contains the status of only 5 coils.  The three most significant bits in this data field are filled in with zeroes.
*  
*
* Parameters:  
*  None
*
* Return: 
*  None
*  
*******************************************************************************/

void ModbusSlave::Process_Coil_Status(void){
    uint16_t ui16StartRegister=0;
    uint16_t ui16NumberofRegister=0;
    uint8_t  ui8ArrayCounter=0;
    uint16_t ui16Result=0;
    uint8_t  ui8Value=0;
    
    ui16StartRegister = ModUtils.aui8ModbusRxBuffer[2];
    ui16StartRegister = ui16StartRegister  << 8;
    ui16StartRegister = ui16StartRegister | ModUtils.aui8ModbusRxBuffer[3];
    
    ui16NumberofRegister = ModUtils.aui8ModbusRxBuffer[4];
    ui16NumberofRegister = ui16NumberofRegister  << 8;
    ui16NumberofRegister = ui16NumberofRegister | ModUtils.aui8ModbusRxBuffer[5];
    
    if((ui16NumberofRegister > MODBUSSLAVE_COIL_STATUS_SIZE) | (ui16NumberofRegister == 0)){
       Process_ErrorCode(0x81,0x2); // illegal data address
    }else{
        if(ui16StartRegister > MODBUSSLAVE_COIL_STATUS_END_INDEX){
            Process_ErrorCode(0x81,0x2); // illegal data address
        }else{
            if(ui16NumberofRegister > (MODBUSSLAVE_COIL_STATUS_END_INDEX + 1 - ui16StartRegister)){
                Process_ErrorCode(0x81,0x2); // illegal data address
            }
            else{
                
                ModUtils.aui8ModbusTxBuffer[ui8ArrayCounter++] = ui8ModbusSlaveId;
                ModUtils.aui8ModbusTxBuffer[ui8ArrayCounter++] = MODBUS_FUNC_READ_COIL_REG;
                ModUtils.aui8ModbusTxBuffer[ui8ArrayCounter++] = 1;
                ModUtils.aui8ModbusTxBuffer[ui8ArrayCounter++] = ui8CoilStatus;

                ui16Result = ModUtils.Modbus_CalculateCRC16(ModUtils.aui8ModbusTxBuffer,ui8ArrayCounter);
                ui8Value = ui16Result >> 8;
                ModUtils.aui8ModbusTxBuffer[ui8ArrayCounter++] = ui8Value;
                ModUtils.aui8ModbusTxBuffer[ui8ArrayCounter++] = ui16Result;
                    
                ui16ModbusSlaveProcessResult = MODBUS_RTU_ERR_NONE;
                ui16ModbusSlaveProcessResult = ui16ModbusSlaveProcessResult << 8;
                ui16ModbusSlaveProcessResult = ui16ModbusSlaveProcessResult | ui8ArrayCounter;//number of bytes to send...
                ui8ModbusErrorCode = MODBUS_RTU_ERR_NONE;
            }
        }
        
    }
}

/****************************************************************************** 
* Function Name: Process_Input_Status
*******************************************************************************
*
* Summary:
*  This function process input status request.
*   Example
*   Request
*
*   This command is requesting the ON/OFF status of discrete inputs # 10197 to 10218
*   from the slave device with address 17.

*   11 02 00C4 0016 BAA9

*   11: The Slave Address (11 hex = address17 )
*   02: The Function Code 2 (read Input Status)
*   00C4: The Data Address of the first input to read.
*             ( 00C4 hex = 196 , + 10001 offset = input #10197 )
*   0016: The total number of coils requested. (16 hex = 22,  inputs 197 to 218 )
*   BAA9: The CRC (cyclic redundancy check) for error checking.

*   Response

*   11 02 03 ACDB35 2018

*   11: The Slave Address (11 hex = address17 )
*   02: The Function Code 2 (read Input Status)
*   03: The number of data bytes to follow (22 Inputs / 8 bits per byte = 3 bytes)
*   AC: Discrete Inputs 10204 -10197 (1010 1100)
*   DB: Discrete Inputs 10212 - 10205 (1101 1011)
*   35: 2 space holders & Discrete Inputs 10218 - 10213 (0011 0101)
*   2018: The CRC (cyclic redundancy check).

*   The more significant bits contain the higher Discrete inputs. This shows that input 10197 is 
*   off (0) and 10204 is on (1). Due to the number of inputs requested, the last data field 35 contains the status of only 6 inputs.  The two most significant bits in this data field are filled in with zeroes.
*
* Parameters:  
*  None
*
* Return: 
*  None
*  
*******************************************************************************/
void ModbusSlave::Process_Input_Status(void){
    uint16_t ui16StartRegister=0;
    uint16_t ui16NumberofRegister=0;
    uint8_t  ui8ArrayCounter=0;
    uint16_t ui16Result=0;
    uint8_t  ui8Value=0;
    
    ui16StartRegister = ModUtils.aui8ModbusRxBuffer[2];
    ui16StartRegister = ui16StartRegister  << 8;
    ui16StartRegister = ui16StartRegister | ModUtils.aui8ModbusRxBuffer[3];
    
    ui16NumberofRegister = ModUtils.aui8ModbusRxBuffer[4];
    ui16NumberofRegister = ui16NumberofRegister  << 8;
    ui16NumberofRegister = ui16NumberofRegister | ModUtils.aui8ModbusRxBuffer[5];
    
    if((ui16NumberofRegister > MODBUSSLAVE_INPUT_STATUS_SIZE) | (ui16NumberofRegister == 0)){
       Process_ErrorCode(0x82,0x2); // illegal data address
    }else{
        if(ui16StartRegister > MODBUSSLAVE_INPUT_STATUS_END_INDEX){
            Process_ErrorCode(0x82,0x2); // illegal data address
        }else{
            if(ui16NumberofRegister > (MODBUSSLAVE_INPUT_STATUS_END_INDEX + 1 - ui16StartRegister)){
                Process_ErrorCode(0x82,0x2); // illegal data address
            }
            else{
                
                ModUtils.aui8ModbusTxBuffer[ui8ArrayCounter++] = ui8ModbusSlaveId;
                ModUtils.aui8ModbusTxBuffer[ui8ArrayCounter++] = MODBUS_FUNC_READ_INPUT_STATUS_REG;
                ModUtils.aui8ModbusTxBuffer[ui8ArrayCounter++] = 1;
                ModUtils.aui8ModbusTxBuffer[ui8ArrayCounter++] = ui8InputStatus;

                ui16Result = ModUtils.Modbus_CalculateCRC16(ModUtils.aui8ModbusTxBuffer,ui8ArrayCounter);
                ui8Value = ui16Result >> 8;
                ModUtils.aui8ModbusTxBuffer[ui8ArrayCounter++] = ui8Value;
                ModUtils.aui8ModbusTxBuffer[ui8ArrayCounter++] = ui16Result;

                ui16ModbusSlaveProcessResult = MODBUS_RTU_ERR_NONE;
                ui16ModbusSlaveProcessResult = ui16ModbusSlaveProcessResult << 8;
                ui16ModbusSlaveProcessResult = ui16ModbusSlaveProcessResult | ui8ArrayCounter;//number of bytes to send...
                ui8ModbusErrorCode = MODBUS_RTU_ERR_NONE;
            }
        }
        
    }
}

/****************************************************************************** 
* Function Name: Process_Force_Single_Coil
*******************************************************************************
*
* Summary:
*  This function process preset the coil request.
*   Example
*   Request
*
*   Request

*   This command is writing the contents of discrete coil # 173 to ON
*   in the slave device with address 17.

*   11 05 00AC FF00 4E8B

*   11: The Slave Address (11 hex = address17 )
*   05: The Function Code 5 (Force Single Coil)
*   00AC: The Data Address of the coil. (coil# 173 - 1 = 172 = AC hex).
*             ( 00AC hex = 172 , + 1 offset = coil #173 )
*   FF00: The status to write ( FF00 = ON,  0000 = OFF )  
*   4E8B: The CRC (cyclic redundancy check) for error checking.
*
*   Response
*
*   The normal response is an echo of the query, returned after the coil has been written.
*
*   11 05 00AC FF00 4E8B
*
*   11: The Slave Address (11 hex = address17 )
*   05: The Function Code 5 (Force Single Coil)
*   00AC: The Data Address of the coil. (coil# 173 - 1 = 172 = AC hex)
*   FF00: The status written ( FF00 = ON,  0000 = OFF )  
*   4E8B: The CRC (cyclic redundancy check) for error checking.
*
*
* Parameters:  
*  None
*
* Return: 
*  None
*  
*******************************************************************************/
void ModbusSlave::Process_Force_Single_Coil(void){
    uint16_t ui16RegisterIndex=0;
    uint16_t ui16RegisterValue=0;
    uint8_t ui8LoopCounter=0;

    ui16RegisterIndex = ModUtils.aui8ModbusRxBuffer[2];
    ui16RegisterIndex = ui16RegisterIndex  << 8;
    ui16RegisterIndex = ui16RegisterIndex | ModUtils.aui8ModbusRxBuffer[3];
    
    ui16RegisterValue = ModUtils.aui8ModbusRxBuffer[4];
    ui16RegisterValue = ui16RegisterValue  << 8;
    ui16RegisterValue = ui16RegisterValue | ModUtils.aui8ModbusRxBuffer[5];

    if(ui16RegisterIndex > MODBUSSLAVE_COIL_STATUS_END_INDEX){
        Process_ErrorCode(0x85,0x02);
    }else{
        if(ui16RegisterValue == 0xFF00){
            if(ui16RegisterIndex == 0){
                ui8CoilStatus = ui8CoilStatus | 0x01;
            }else if(ui16RegisterIndex == 1){
                ui8CoilStatus = ui8CoilStatus | 0x02;
            }else if(ui16RegisterIndex == 2){  
                ui8CoilStatus = ui8CoilStatus | 0x04;
            }else{
                ui8CoilStatus = ui8CoilStatus | 0x08;
            }
            for(ui8LoopCounter=0;ui8LoopCounter<8;ui8LoopCounter++){
                ModUtils.aui8ModbusTxBuffer[ui8LoopCounter] = ModUtils.aui8ModbusRxBuffer[ui8LoopCounter];
            }
                    
            ui16ModbusSlaveProcessResult = MODBUS_RTU_ERR_NONE;
            ui16ModbusSlaveProcessResult = ui16ModbusSlaveProcessResult << 8;
            ui16ModbusSlaveProcessResult = ui16ModbusSlaveProcessResult | 8;//number of bytes to send...
            EEPROM.write(MEMORY_COIL_REG_BYTE,ui8CoilStatus);
            EEPROM.commit();
            EMemory.Update_All();
            if(ui8CoilStatus & 0x01){
                digitalWrite(LED1_PIN,HIGH);
            }else{
                digitalWrite(LED1_PIN,LOW);
            }
        }else if(ui16RegisterValue == 0x0000){
            if(ui16RegisterIndex == 0){
                ui8CoilStatus = ui8CoilStatus & 0xFE;
            }else if(ui16RegisterIndex == 1){
                ui8CoilStatus = ui8CoilStatus & 0xFD;
            }else if(ui16RegisterIndex == 2){  
                ui8CoilStatus = ui8CoilStatus & 0xFB;
            }else{
                ui8CoilStatus = ui8CoilStatus & 0xF8;
            }
            for(ui8LoopCounter=0;ui8LoopCounter<8;ui8LoopCounter++){
                ModUtils.aui8ModbusTxBuffer[ui8LoopCounter] = ModUtils.aui8ModbusRxBuffer[ui8LoopCounter];
            }
                    
            ui16ModbusSlaveProcessResult = MODBUS_RTU_ERR_NONE;
            ui16ModbusSlaveProcessResult = ui16ModbusSlaveProcessResult << 8;
            ui16ModbusSlaveProcessResult = ui16ModbusSlaveProcessResult | 8;//number of bytes to send...
            ui8ModbusErrorCode = MODBUS_RTU_ERR_NONE;
            EEPROM.write(MEMORY_COIL_REG_BYTE,ui8CoilStatus);
            EEPROM.commit();
            EMemory.Update_All();
            if(ui8CoilStatus & 0x01){
                digitalWrite(LED1_PIN,HIGH);
            }else{
                digitalWrite(LED1_PIN,LOW);
            }
        }else{
            Process_ErrorCode(0x85,0x03);
        }
    }
}

/****************************************************************************** 
* Function Name: Process_Force_Multiple_Coil
*******************************************************************************
*
* Summary:
*  This function process multiple write coils request.
*   Example
*	
*   Request

*   This command is writing the contents of a series of 10 discrete coils from #20 to #29
*   to the slave device with address 17.
*
*   11 0F 0013 000A 02 CD01 BF0B
*
*   11: The Slave Address (11 hex = address17 )
*   0F: The Function Code 15 (Force Multiple Coils, 0F hex  = 15 )
*   0013: The Data Address of the first coil.
*             ( 0013 hex = 19 , + 1 offset = coil #20 )
*   000A: The number of coils to written ( 0A hex  = 10 )
*   02: The number of data bytes to follow (10 Coils / 8 bits per byte = 2 bytes)
*   CD: Coils 27 - 20 (1100 1101)  
*   01: 6 space holders & Coils 29 - 28 (0000 0001)  
*   BF0B: The CRC (cyclic redundancy check) for error checking.
*
*   The more significant bits contain the higher coil variables. This shows that coil 20 is on (1) and 21 is off (0). Due to the number of coils requested, the last data field01 contains the status of only 2 coils.  The unused bits in the last data byte are filled in with zeroes.
*
*   Response
*
*   11 0F 0013 000A 2699
*
*   11: The Slave Address (11 hex = address17 )
*   0F: The Function Code (Force Multiple Coils, 0F hex  = 15 )
*   0013: The Data Address of the first coil. 
*             ( 0013 hex = 19 , + 1 offset = coil #20 )
*   000A: The number of coils to written ( 0A hex  = 10 )
*   2699: The CRC (cyclic redundancy check) for error checking.
*
*
* Parameters:  
*  None
*
* Return: 
*  None
*  
*******************************************************************************/
void ModbusSlave::Process_Force_Multiple_Coil(void){
    uint16_t ui16RegisterIndex=0;
    uint16_t ui16NumberOfBits=0;
    uint8_t  ui8Value=0;
    uint8_t  ui8Mask=1;
    uint8_t  ui8LoopCounter=0;
    uint16_t ui16Result=0;

    ui16RegisterIndex = ModUtils.aui8ModbusRxBuffer[2];
    ui16RegisterIndex = ui16RegisterIndex  << 8;
    ui16RegisterIndex = ui16RegisterIndex | ModUtils.aui8ModbusRxBuffer[3];
    
    ui16NumberOfBits = ModUtils.aui8ModbusRxBuffer[4];
    ui16NumberOfBits = ui16NumberOfBits  << 8;
    ui16NumberOfBits = ui16NumberOfBits | ModUtils.aui8ModbusRxBuffer[5];

    ui8Value = ModUtils.aui8ModbusRxBuffer[7];

    if(ui16RegisterIndex > MODBUSSLAVE_COIL_STATUS_END_INDEX){
        Process_ErrorCode((0x80|0x0F),0x02);
    }else{
        if(ui16NumberOfBits > MODBUSSLAVE_COIL_STATUS_SIZE){
            Process_ErrorCode((0x80|0x0F),0x03);
        }else{
            for(ui8LoopCounter=0;ui8LoopCounter<ui16RegisterIndex;ui8LoopCounter++){
                ui8Mask = ui8Mask << 1;
                ui8Value = ui8Value << 1;
            }

            for(ui8LoopCounter=ui16RegisterIndex;ui8LoopCounter<(ui16NumberOfBits + ui16RegisterIndex);ui8LoopCounter++){
                if(ui8Value & ui8Mask){
                    ui8CoilStatus = ui8CoilStatus | ui8Mask;    
                }else{
                    ui8CoilStatus = ui8CoilStatus & ~ui8Mask;    
                }
                ui8Mask = ui8Mask << 1;
            }

            for(ui8LoopCounter=0;ui8LoopCounter<6;ui8LoopCounter++){
                ModUtils.aui8ModbusTxBuffer[ui8LoopCounter] = ModUtils.aui8ModbusRxBuffer[ui8LoopCounter];
            }
            ui16Result = ModUtils.Modbus_CalculateCRC16(ModUtils.aui8ModbusTxBuffer,6);
            ModUtils.aui8ModbusTxBuffer[6] = ui16Result;
            ui8Value = ui16Result >> 8;
            ModUtils.aui8ModbusTxBuffer[7] = ui8Value;
                    
            ui16ModbusSlaveProcessResult = MODBUS_RTU_ERR_NONE;
            ui16ModbusSlaveProcessResult = ui16ModbusSlaveProcessResult << 8;
            ui16ModbusSlaveProcessResult = ui16ModbusSlaveProcessResult | 8;//number of bytes to send...
            ui8ModbusErrorCode = MODBUS_RTU_ERR_NONE;
            EEPROM.write(MEMORY_COIL_REG_BYTE,ui8CoilStatus);
            EEPROM.commit();
            EMemory.Update_All();
            if(ui8CoilStatus & 0x01){
                digitalWrite(LED1_PIN,HIGH);
            }else{
                digitalWrite(LED1_PIN,LOW);
            }
        }
    }
}

/****************************************************************************** 
* Function Name: ModbusSlave_Process_Preset_Single_Register
*******************************************************************************
*
* Summary:
*  This function process preset the single register request.
*   Example
*   Request
*
*   This command is writing the contents of analog output holding register # 40002
*   to the slave device with address 17.
*
*   11 06 0001 0003 9A9B
*
*   11: The Slave Address (11 hex = address17 )
*   06: The Function Code 6 (Preset Single Register)
*   0001: The Data Address of the register.
*             ( 0001 hex = 1 , + 40001 offset = register #40002 )
*   0003: The value to write 
*   9A9B: The CRC (cyclic redundancy check) for error checking.

*   Response

*   The normal response is an echo of the query, returned after the register contents have been written.

*   11 06 0001 0003 9A9B

*   11: The Slave Address (11 hex = address17 )
*   06: The Function Code 6 (Preset Single Register)
*   0001: The Data Address of the register. (# 40002 - 40001 = 1 )
*   0003: The value written 
*   9A9B: The CRC (cyclic redundancy check) for error checking.
*  
*
* Parameters:  
*  None
*
* Return: 
*  None
*  
*******************************************************************************/
void ModbusSlave::Process_Preset_Single_Register(void){
    uint16_t ui16RegisterIndex=0;
    uint16_t ui16RegisterValue=0;
    uint8_t ui8LoopCounter=0;

    ui16RegisterIndex = ModUtils.aui8ModbusRxBuffer[2];
    ui16RegisterIndex = ui16RegisterIndex  << 8;
    ui16RegisterIndex = ui16RegisterIndex | ModUtils.aui8ModbusRxBuffer[3];
    
    ui16RegisterValue = ModUtils.aui8ModbusRxBuffer[4];
    ui16RegisterValue = ui16RegisterValue  << 8;
    ui16RegisterValue = ui16RegisterValue |ModUtils.aui8ModbusRxBuffer[5];

    if(ui16RegisterIndex > MODBUSSLAVE_HOLDING_REG_END_INDEX){
        Process_ErrorCode(0x86,0x02);
    }else{
        aui16HoldingRegister[ui16RegisterIndex] = ui16RegisterValue;
        EMemory.Update_Holding_Reg(ui16RegisterIndex,ui16RegisterValue);
        for(ui8LoopCounter=0;ui8LoopCounter<8;ui8LoopCounter++){
            ModUtils.aui8ModbusTxBuffer[ui8LoopCounter] = ModUtils.aui8ModbusRxBuffer[ui8LoopCounter];
        }
                    
        ui16ModbusSlaveProcessResult = MODBUS_RTU_ERR_NONE;
        ui16ModbusSlaveProcessResult = ui16ModbusSlaveProcessResult << 8;
        ui16ModbusSlaveProcessResult = ui16ModbusSlaveProcessResult | 8;//number of bytes to send...
        ui8ModbusErrorCode = MODBUS_RTU_ERR_NONE;
    }
}

/****************************************************************************** 
* Function Name: ModbusSlave_Process_Preset_Multiple_Register
*******************************************************************************
*
* Summary:
*  This function process multiple write register request.
*   Example
*	
*   Preset Multiple Registers (FC=16)
*
*   Request
*
*   This command is writing the contents of two analog output holding registers # 40002 & 40003 to the slave device with address 17.
*
*   11 10 0001 0002 04 000A 0102 C6F0
*
*   11: The Slave Address (11 hex = address17 )
*   10: The Function Code 16 (Preset Multiple Registers, 10 hex - 16 )
*   0001: The Data Address of the first register. 
*            ( 0001 hex = 1 , + 40001 offset = register #40002 )
*   0002: The number of registers to write 
*   04: The number of data bytes to follow (2 registers x 2 bytes each = 4 bytes)
*   000A: The value to write to register 40002 
*   0102: The value to write to register 40003 
*   C6F0: The CRC (cyclic redundancy check) for error checking.
*
*   Response
*
*   11 10 0001 0002 1298
*
*   11: The Slave Address (17 = 11 hex)
*   10: The Function Code 16 (Preset Multiple Registers, 10 hex - 16 )
*   0001: The Data Address of the first register. (# 40002 - 40001 = 1 )
*   0002: The number of registers written. 
*   1298: The CRC (cyclic redundancy check) for error checking.
*
*
* Parameters:  
*  None
*
* Return: 
*  None
*  
*******************************************************************************/
void ModbusSlave::Process_Preset_Multiple_Register(void){
    uint16_t ui16StartRegister=0;
    uint16_t ui16RegisterCount=0;
    uint8_t  ui8LoopCounter=0;
    uint16_t ui16Value=0;
    uint8_t  ui8IndexCounter=0;
    uint16_t ui16Result;
    uint8_t  ui8Value=0;

    ui16StartRegister = ModUtils.aui8ModbusRxBuffer[2];
    ui16StartRegister = ui16StartRegister  << 8;
    ui16StartRegister = ui16StartRegister | ModUtils.aui8ModbusRxBuffer[3];
    
    ui16RegisterCount = ModUtils.aui8ModbusRxBuffer[4];
    ui16RegisterCount = ui16RegisterCount << 8;
    ui16RegisterCount = ui16RegisterCount | ModUtils.aui8ModbusRxBuffer[5];
    
    if(ui16StartRegister > MODBUSSLAVE_HOLDING_REG_END_INDEX){
        Process_ErrorCode((0x80 | 0x10),0x02);
    }else{
        if((ui16RegisterCount == 0) | (ui16RegisterCount > MODBUSSLAVE_HOLDING_REG_SIZE)){
            Process_ErrorCode((0x80 | 0x10),0x02);
        }else{
            if((ui16StartRegister + ui16RegisterCount) > MODBUSSLAVE_HOLDING_REG_SIZE){
                Process_ErrorCode((0x80 | 0x10),0x02);
            }else{
                for(ui8LoopCounter=ui16StartRegister;ui8LoopCounter<(ui16StartRegister + ui16RegisterCount);ui8LoopCounter++){
                    ui16Value = ModUtils.aui8ModbusRxBuffer[7+ui8IndexCounter++];
                    ui16Value = ui16Value << 8;
                    ui16Value = ui16Value | ModUtils.aui8ModbusRxBuffer[7+ui8IndexCounter++];
                    aui16HoldingRegister[ui8LoopCounter] = ui16Value;
                    EMemory.Update_Holding_Reg(ui8LoopCounter,ui16Value);
                }
                for(ui8LoopCounter=0;ui8LoopCounter<6;ui8LoopCounter++){
                ModUtils.aui8ModbusTxBuffer[ui8LoopCounter] = ModUtils.aui8ModbusRxBuffer[ui8LoopCounter];
            }
                ui16Result = ModUtils.Modbus_CalculateCRC16(ModUtils.aui8ModbusTxBuffer,6);
                ModUtils.aui8ModbusTxBuffer[6] = ui16Result;
                ui8Value = ui16Result >> 8;
                ModUtils.aui8ModbusTxBuffer[7] = ui8Value;
                    
                ui16ModbusSlaveProcessResult = MODBUS_RTU_ERR_NONE;
                ui16ModbusSlaveProcessResult = ui16ModbusSlaveProcessResult << 8;
                ui16ModbusSlaveProcessResult = ui16ModbusSlaveProcessResult | 8;//number of bytes to send...
                ui8ModbusErrorCode = MODBUS_RTU_ERR_NONE;
            }
        }
    }
}
/****************************************************************************** 
* Function Name: ModbusSlave_Exception_Handling
*******************************************************************************
*
* Summary:
*  This function prepare aui8ModbusTxBuffer based on exception code.
*  
*
*  
*
* Parameters:  
*  ui8ErrorCode: based on error code contruct the array to be sent...
*
* Return: 
*  None
*  
*******************************************************************************/

void ModbusSlave::Exception_Handling(uint8_t ui8ErrorCode, uint8_t ui8ErrorValue){
    uint16_t ui16Result=0;
    uint8_t  ui8Value=0;
    
    memset(ModUtils.aui8ModbusTxBuffer, 0, MODBUS_TX_BUFFER_SIZE);
    ModUtils.aui8ModbusTxBuffer[0] = ui8ModbusSlaveId;
    ModUtils.aui8ModbusTxBuffer[1] = ui8ErrorCode;
    ModUtils.aui8ModbusTxBuffer[2] = ui8ErrorValue;
    
    ui16Result =ModUtils.Modbus_CalculateCRC16(ModUtils.aui8ModbusTxBuffer,3);
    ModUtils.aui8ModbusTxBuffer[3] = ui16Result;
    ui8Value = ui16Result >> 8;
    ModUtils.aui8ModbusTxBuffer[4] = ui8Value;
}

void ModbusSlave::Process_ErrorCode(uint8_t ui8FunctionError, uint8_t ui8ErrorCode){
    ui16ModbusSlaveProcessResult = MODBUS_RTU_ERR_INVALID_RESPONSE_LEN;
    ui16ModbusSlaveProcessResult = ui16ModbusSlaveProcessResult << 8;
    ui16ModbusSlaveProcessResult = ui16ModbusSlaveProcessResult | 5;//number of bytes to send...
    if(ui8ErrorCode == 0x02){
        ui8ModbusErrorCode = MODBUS_RTU_ERR_INVALID_REG_ADDRESS;
    }else if(ui8ErrorCode == 0x03){
        ui8ModbusErrorCode = MODBUS_RTU_ERR_INVALID_RESPONSE_LEN;
    }
    
    Exception_Handling(ui8FunctionError,ui8ErrorCode);
}

/****************************************************************************** 
* Function Name: Write
*******************************************************************************
*
* Summary:
*  This function write the modbus processed data.
*  number of byte sends depends on variable ui16ModbusSlaveProcessResult's lower byte
*
*  
*
* Parameters:  
*  None
*
* Return: 
*  None
*  
*******************************************************************************/

void ModbusSlave::Write(void){
    uint8_t ui8ByteCount=0;
    uint8_t ui8LoopCounter=0;

    ui8ByteCount = ui16ModbusSlaveProcessResult;
    if(ui8ByteCount > 0){// more than 0 means send response
            // Enable modbusTXE, TXELED
            //digitalWrite(MODBUS_ENABLE_PIN,HIGH);
            //digitalWrite(MODBUS_LED_PIN,HIGH);
            Serial1.print("Modbus Tx: ");
            sWebServerModbusTxCopy = " ";
            for(ui8LoopCounter=0;ui8LoopCounter<ui8ByteCount;ui8LoopCounter++){
                Serial.write(ModUtils.aui8ModbusTxBuffer[ui8LoopCounter]);
                while(Serial.availableForWrite() < 127);
                Serial1.print(ModUtils.aui8ModbusTxBuffer[ui8LoopCounter],HEX);
                Serial1.print(" ");
                sWebServerModbusTxCopy = sWebServerModbusTxCopy + String(ModUtils.aui8ModbusTxBuffer[ui8LoopCounter],HEX) + " ";
            }
            Serial1.println(" ");
            delay(1);
            //digitalWrite(MODBUS_LED_PIN,LOW);
            //digitalWrite(MODBUS_ENABLE_PIN,LOW);
            //clear all set values....
            ui16ModbusSlaveReadResult=0;
            ui16ModbusSlaveProcessResult=0;
            ui16ModbusSlaveWriteResult=0;
            ui16ModbusTxCount++;
        }
}

/****************************************************************************** 
* Function Name: ModbusSlave_Is_DataReceived
*******************************************************************************
*
* Summary:
*  This function read any query request from modbus master.
*  The function checks for any date byte received in modbus_uart rx buffer
*  sets ui16ModbusSlaveReadResult veriable
*  
*
* Parameters:  
*  None
*
* Return: 
*  Returns read result and number of bytes received
*  
*******************************************************************************/

uint16_t ModbusSlave::Is_DataReceived(void){
    uint8_t  ui8Response=0;
    uint8_t  ui8BufferSize=0;
    uint16_t ui16Result=0;
    uint16_t  ui16LoopCounter=0;
    uint8_t  ui8Bytexpected=0;
    
    ui8BufferSize = Serial.available();
    if(ui8BufferSize > 0){
        //digitalWrite(MODBUS_LED_PIN,HIGH);

        for(ui16LoopCounter=0;ui16LoopCounter<ui8BufferSize;ui16LoopCounter++){
            ModUtils.aui8ModbusRxBuffer[ui16ModbusRxCounter] = Serial.read();
            ui16ModbusRxCounter++;
        }
        /* if received 1st byte not equal to slaveid then clear */
        if(ModUtils.aui8ModbusRxBuffer[0] != ui8ModbusSlaveId){
            ui16ModbusRxCounter = 0;
        }else{
            if(ui16ModbusRxCounter > 2){
                /* check for functioncode */
                if(ModUtils.Modbus_Verify_FunctionCode(ModUtils.aui8ModbusRxBuffer[1]) == 0){
                    ui16ModbusRxCounter = 0;
                    //digitalWrite(MODBUS_LED_PIN,LOW);
                }
            }
        }
    }
    
    if( ui16ModbusRxCounter > (MODBUS_DEFAULT_BYTE_SIZE - 1)){
        
        if((ModUtils.aui8ModbusRxBuffer[1] == MODBUS_FUNC_WRITE_MULTI_COIL) | (ModUtils.aui8ModbusRxBuffer[1] == MODBUS_FUNC_WRITE_MULTI_REG)){
            /* +1 because of internal rx*/
            ui8Bytexpected = ModUtils.aui8ModbusRxBuffer[6] + 9 ; // simplymodbus.org
            if(ui16ModbusRxCounter > (ui8Bytexpected-1)){
                //digitalWrite(MODBUS_LED_PIN,LOW);
                ui8Response = 1;
                ui16Result = ui8Response;
                ui16Result = ui16Result << 8;   
                ui16Result = ui16Result | ui16ModbusRxCounter;
                ui16ModbusRxCounter = 0;
            }
        }else{
            //digitalWrite(MODBUS_LED_PIN,LOW);
            ui8Response = 1;
            ui16Result = ui8Response;
            ui16Result = ui16Result << 8;   
            ui16Result = ui16Result | ui16ModbusRxCounter;
            ui16ModbusRxCounter = 0;
        }
    }else{
        ui8Response= 0;
        ui16Result = ui8Response;
        ui16Result = ui16Result << 8;
        ui16Result = ui16Result | ui16ModbusRxCounter;
    }

    return ui16Result;
}

// Preinstantiate Objects //////////////////////////////////////////////////////
#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_TWOWIRE)
ModbusSlave Modbus;
#endif