/*******************************************************************************
* File Name: ModbusUtils.h
*
* Version: 1.0
*
* Description:
* This is header file for modbus utils. All the functions related to
* Modbus utils are implemented in this file
*
* Owner:
* Yogesh M Iggalore
*
* Code Tested With:
* 1. platformio and esp8266
*
********************************************************************************
* Copyright (2020-21) , METI
********************************************************************************/
#ifndef ModbusUtils_h
#define ModbusUtils_h

#include <Arduino.h>

#define MODBUS_PARITY_NONE  0
#define MODBUS_PARITY_EVEN  1
#define MODBUS_PARITY_ODD   2
#define MODBUS_PARITY_MASK  3

#define MODBUS_STOP_BIT_0  0
#define MODBUS_STOP_BIT_1  1
#define MODBUS_STOP_BIT_2  2

#define MODBUS_DATA_BITS_5 5
#define MODBUS_DATA_BITS_6 6
#define MODBUS_DATA_BITS_7 7
#define MODBUS_DATA_BITS_8 8

#define MODBUS_SPEEDS_SUPPORTED_MAX 8
#define MODBUS_PARITY_SUPPORT_MAX   4

#define MODBUS_TCP_PORT 502

//
// MODBUS MBAP offsets
//
#define MODBUS_TCP_TID              0
#define MODBUS_TCP_PID              2
#define MODBUS_TCP_LEN              4
#define MODBUS_TCP_UID              6
#define MODBUS_TCP_FUNC             7
#define MODBUS_TCP_REGISTER_START   8
#define MODBUS_TCP_REGISTER_NUMBER  10

#define MODBUS_ASCII_START_BYTE   0x3A
#define MODBUS_ASCII_CR_BYTE      0x0D
#define MODBUS_ASCII_LF_BYTE      0x0A

// ModBus Function Codes (Read)
#define MODBUS_FUNC_READ_COIL_REG		            0x01
#define MODBUS_FUNC_READ_INPUT_STATUS_REG		    0x02
#define MODBUS_FUNC_READ_HOLDING_REG		        0x03
#define MODBUS_FUNC_READ_INPUT_REG			        0x04

#define MODBUS_FUNC_WRITE_SINGLE_COIL			    0x05
#define MODBUS_FUNC_WRITE_SINGLE_REG			    0x06
#define MODBUS_FUNC_WRITE_MULTI_COIL			    0x0F
#define MODBUS_FUNC_WRITE_MULTI_REG			        0x10

    
#define MODBUS_COMMAND_STANDARD_LENGTH              0x08
// ModBus Function Codes (Write)    
#define MODBUS_FUNC_PRESET_SINGLE_REG		        0x06
#define MODBUS_FUNC_PRESET_MULTIPLE_REG	            0x10

#define MODBUS_RTU_ERR_NONE						    0x00
#define MODBUS_RTU_ERR_CRC							0xFF
#define MODBUS_RTU_ERR_TIMEOUT						0xFE
#define MODBUS_RTU_ERR_INVALID_REG_ADDRESS	        0xFD
#define MODBUS_RTU_ERR_INVALID_RESPONSE_LEN		    0xFC
#define MODBUS_RTU_ERR_INVALID_MODBUS_COMMAND		0xFB
#define MODBUS_RTU_ERR_INVALID_FUNC		            0xF9
#define MODBUS_RTU_ERR_INVALID_DATA_VALUE		    0xF8
#define MODBUS_RTU_ERR_GATEWAY_FAILED		        0xF7
#define MODBUS_RTU_METI_NO_NEW_DATA		            0xF6
#define MODBUS_RTU_ERR_UNKNOWN		                0xF1

#define MODBUS_ILLEGAL_FUN_CODE     0x01
#define MODBUS_ILLEGAL_ADDRESS_CODE 0x02
#define MODBUS_ILLEGAL_VALUE_CODE   0x03
#define MODBUS_SLAVE_FAILURE        0x04
#define MODBUS_ACKNOWLEDGE          0x05
#define MODBUS_SLAVE_BUSY           0x06
#define MODBUS_NAK_ACKNOWLEDGE      0x07
#define MODBUS_MEMORY_ERROR         0x08 
#define MODBUS_GATEWAY_ERROR        0x0A
#define MODBUS_GATEWAY_FAILURE      0x0B

//Standard locations
#define MODBUS_RTU_LOCATION_DEV_NUMBER			    0
#define MODBUS_RTU_LOCATION_FUNC_CODE			    1	
#define MODBUS_RTU_LOCATION_FUNC_ERROR			    2	

//Query - Locations
#define MODBUS_RTU_LOCATION_REG_START_LO		    2
#define MODBUS_RTU_LOCATION_REG_START_HI		    3
#define MODBUS_RTU_LOCATION_REG_COUNT_LO		    4
#define MODBUS_RTU_LOCATION_REG_COUNT_HI		    5
#define MODBUS_RTU_LOCATION_CRC_LO		            6
#define MODBUS_RTU_LOCATION_CRC_HI		            7

#define MODBUS_RTU_LOCATION_BYTES_COUNT		        2
    
#define MODBUS_RTU_LOCATION_BYTES_TO_FOLLOW		    6
#define MODBUS_RTU_WRITE_RETRIES		            0
#define MODBUS_RTU_WRITE_EXPECTED_BYTES		        8
    
#define MODBUS_TYPE_RTU     0
#define MODBUS_TYPE_ASCII   1
#define MODBUS_TYPE_TCPIP   2

#define MODBUS_MODE_NONE    0
#define MODBUS_MODE_SLAVE   1
#define MODBUS_MODE_MASTER  2

#define MODBUS_DEFAULT_BYTE_SIZE   0x08
#define MODBUS_MIN_BYTES           0x05

#define MODBUS_ASCII_MANDATORY_RESPONSE_BYTES 4
#define MODBUS_RTU_MANDATORY_RESPONSE_BYTES       5   //1-address, 1-function, 1-no.of recd bytes, 2-crc
#define MODBUS_TCPIP_MANDATORY_RESPONSE_BYTES       4

#define METI_MODBUS_PORT_INDEX_RTU_0  0
#define METI_MODBUS_PORT_INDEX_RTU_1  1
#define METI_MODBUS_PORT_INDEX_ASCII  2
#define METI_MODBUS_PORT_INDEX_TCPIP  3

#define MODBUS_MAX_DEVICE_ID_ALLOWED   245

#define MODBUS_RX_BUFFER_SIZE   300
#define MODBUS_TX_BUFFER_SIZE   64

class ModbusUtils{
public:
    
    uint8_t aui8ModbusRxBuffer[MODBUS_RX_BUFFER_SIZE];
    uint8_t aui8ModbusTxBuffer[MODBUS_TX_BUFFER_SIZE];

    uint32_t Modbus_Baud_Display[MODBUS_SPEEDS_SUPPORTED_MAX]={115200,57600,38400,19200,9600,4800,2400,1200};

    //Time per byte in milliseconds 
    //Index-Baudrate : 0 - 115200, 1 - 57600, 2 - 38400 , 3 - 19200 , 4 - 9600, 5 - 4800 , 6 - 2400, 7-1200
    uint8_t ModbusTimePerByte[MODBUS_SPEEDS_SUPPORTED_MAX] = {1,1,1,1,2,2,4,8};

    // 0 - None parity, 1 - Even Parity , 2 - Odd Parity , 3 - Mark Space
    uint8_t Modbus_Parity[MODBUS_PARITY_SUPPORT_MAX] = {0, 1, 2, 3};

    ModbusUtils();
    void Start(void);

    uint32_t Modbus_Get_Baudrate(uint8_t ui8Index);
    uint8_t  Modbus_Get_Parity(uint8_t ui8Index);
    uint8_t  Modbus_Get_TimePerByte(uint8_t ui8Index);

    uint32_t Get_UART_Config(uint8_t ui8Databits, uint8_t ui8Parity, uint8_t ui8Stopbit);

    uint8_t Modbus_Convert_RTU_To_ASCII(uint8_t *MsgSourceAddr, uint8_t *MsgDestAddr,uint8_t MsgLength);
    uint8_t Modbus_Convert_ASCII_To_RTU(uint8_t *MsgSourceAddr, uint8_t *MsgDestAddr,uint8_t MsgLength);
    char Modbus_ASCII_LRC(uint8_t *MsgSourceAddr, uint8_t MsgLength);
    uint16_t Modbus_CalculateCRC16(uint8_t *MsgStartAddr, uint8_t MsgLength);
    uint8_t Modbus_Verify_FunctionCode(uint8_t ui8ReceivedByte);
    uint8_t Modbus_ExpectedBytes_RTU(uint8_t aui8Buffer[]);
    uint8_t Modbus_ExpectedBytes_TCPIP(uint8_t aui8Buffer[]);
    //uint8_t modbus_get_expected_bytes( uint8_t function_code, uint16_t register_count);

    uint8_t Modbus_Get_Error_Code(uint8_t ui8Byte);
};

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_TWOWIRE)
extern ModbusUtils ModUtils;
#endif
#endif