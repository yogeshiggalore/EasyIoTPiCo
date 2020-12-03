/*******************************************************************************
* File Name: ModbusUtils.cpp
*
* Version: 1.0
*
* Description:
* This is source file for modbus utils. All the functions related to
* Modbus utils are implemented in this file
*
* Owner:
* Yogesh M Iggalore
*
* Code Tested With:
* 1. platformio and esp32
*
********************************************************************************
* Copyright (2020-21) , eNtroL 
********************************************************************************/
#include <Arduino.h>
#include <ModbusUtils.h>

//*************************************************************************************************
//*** CRC look-up table
//*************************************************************************************************
const char TabCRCHi[] = {
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
};

const char TabCRCLo[] = {
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04,
    0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 0x08, 0xC8,
    0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC,
    0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3, 0x11, 0xD1, 0xD0, 0x10,
    0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
    0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38,
    0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C,
    0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26, 0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0,
    0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4,
    0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
    0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C,
    0xB4, 0x74, 0x75, 0xB5, 0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0,
    0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54,
    0x9C, 0x5C, 0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98,
    0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80, 0x40
};

union
{
    unsigned char Array[2];
    
    struct
    {
        unsigned char LL    :4;
        unsigned char LH    :4;
        unsigned char HL    :4;
        unsigned char HH    :4;
    }NB4;
    
    struct
    {                               // Nibbles x 16 bits
        unsigned char Lo;           // IntL
        unsigned char Hi;           // IntH
    }Byte;
    
    struct
    {                               // BITs
        unsigned b00 :1;
        unsigned b01 :1;
        unsigned b02 :1;
        unsigned b03 :1;
        unsigned b04 :1;
        unsigned b05 :1;
        unsigned b06 :1;
        unsigned b07 :1;
        unsigned b08 :1;
        unsigned b09 :1;
        unsigned b10 :1;
        unsigned b11 :1;
        unsigned b12 :1;
        unsigned b13 :1;
        unsigned b14 :1;
        unsigned b15 :1;
    }Bit;

    signed int INTs;
    unsigned int INTu;
}CRC;

// Constructors ////////////////////////////////////////////////////////////////
ModbusUtils::ModbusUtils(){}

void ModbusUtils::Start(void){
    // as of now doing nothing
}

/******************************************************************************
* Function Name: Get_UART_Config
*******************************************************************************
*
* Summary:
*  This function return serial config values
*
* Parameters:
*  ui8Databits, ui8Parity, ui8Stopbit
*
* Return:
*  SerialConfig
*
*******************************************************************************/
SerialConfig ModbusUtils::Get_UART_Config(uint8_t ui8Databits, uint8_t ui8Parity, uint8_t ui8Stopbit){

    if((ui8Databits == MODBUS_DATA_BITS_5) & (ui8Parity == MODBUS_PARITY_NONE) & (ui8Stopbit == MODBUS_STOP_BIT_1)){
        return  SERIAL_5N1;
    }else if((ui8Databits == MODBUS_DATA_BITS_6) & (ui8Parity == MODBUS_PARITY_NONE) & (ui8Stopbit == MODBUS_STOP_BIT_1)){
        return  SERIAL_6N1;
    }else if((ui8Databits == MODBUS_DATA_BITS_7) & (ui8Parity == MODBUS_PARITY_NONE) & (ui8Stopbit == MODBUS_STOP_BIT_1)){
        return  SERIAL_7N1;
    }else if((ui8Databits == MODBUS_DATA_BITS_8) & (ui8Parity == MODBUS_PARITY_NONE) & (ui8Stopbit == MODBUS_STOP_BIT_1)){
        return  SERIAL_8N1;
    }else if((ui8Databits == MODBUS_DATA_BITS_5) & (ui8Parity == MODBUS_PARITY_NONE) & (ui8Stopbit == MODBUS_STOP_BIT_2)){
        return  SERIAL_5N2;
    }else if((ui8Databits == MODBUS_DATA_BITS_6) & (ui8Parity == MODBUS_PARITY_NONE) & (ui8Stopbit == MODBUS_STOP_BIT_2)){
        return  SERIAL_6N2;
    }else if((ui8Databits == MODBUS_DATA_BITS_7) & (ui8Parity == MODBUS_PARITY_NONE) & (ui8Stopbit == MODBUS_STOP_BIT_2)){
        return  SERIAL_7N2;
    }else if((ui8Databits == MODBUS_DATA_BITS_8) & (ui8Parity == MODBUS_PARITY_NONE) & (ui8Stopbit == MODBUS_STOP_BIT_2)){
        return  SERIAL_8N2;
    }else if((ui8Databits == MODBUS_DATA_BITS_5) & (ui8Parity == MODBUS_PARITY_EVEN) & (ui8Stopbit == MODBUS_STOP_BIT_1)){
        return  SERIAL_5E1;
    }else if((ui8Databits == MODBUS_DATA_BITS_6) & (ui8Parity == MODBUS_PARITY_EVEN) & (ui8Stopbit == MODBUS_STOP_BIT_1)){
        return  SERIAL_6E1;
    }else if((ui8Databits == MODBUS_DATA_BITS_7) & (ui8Parity == MODBUS_PARITY_EVEN) & (ui8Stopbit == MODBUS_STOP_BIT_1)){
        return  SERIAL_7E1;
    }else if((ui8Databits == MODBUS_DATA_BITS_8) & (ui8Parity == MODBUS_PARITY_EVEN) & (ui8Stopbit == MODBUS_STOP_BIT_1)){
        return  SERIAL_8E1;
    }else if((ui8Databits == MODBUS_DATA_BITS_5) & (ui8Parity == MODBUS_PARITY_EVEN) & (ui8Stopbit == MODBUS_STOP_BIT_2)){
        return  SERIAL_5E2;
    }else if((ui8Databits == MODBUS_DATA_BITS_6) & (ui8Parity == MODBUS_PARITY_EVEN) & (ui8Stopbit == MODBUS_STOP_BIT_2)){
        return  SERIAL_6E2;
    }else if((ui8Databits == MODBUS_DATA_BITS_7) & (ui8Parity == MODBUS_PARITY_EVEN) & (ui8Stopbit == MODBUS_STOP_BIT_2)){
        return  SERIAL_7E2;
    }else if((ui8Databits == MODBUS_DATA_BITS_8) & (ui8Parity == MODBUS_PARITY_EVEN) & (ui8Stopbit == MODBUS_STOP_BIT_2)){
        return  SERIAL_8E2;
    }else if((ui8Databits == MODBUS_DATA_BITS_5) & (ui8Parity == MODBUS_PARITY_ODD) & (ui8Stopbit == MODBUS_STOP_BIT_1)){
        return  SERIAL_5O1;
    }else if((ui8Databits == MODBUS_DATA_BITS_6) & (ui8Parity == MODBUS_PARITY_ODD) & (ui8Stopbit == MODBUS_STOP_BIT_1)){
        return  SERIAL_6O1;
    }else if((ui8Databits == MODBUS_DATA_BITS_7) & (ui8Parity == MODBUS_PARITY_ODD) & (ui8Stopbit == MODBUS_STOP_BIT_1)){
        return  SERIAL_7O1;
    }else if((ui8Databits == MODBUS_DATA_BITS_8) & (ui8Parity == MODBUS_PARITY_ODD) & (ui8Stopbit == MODBUS_STOP_BIT_1)){
        return  SERIAL_8O1;
    }else if((ui8Databits == MODBUS_DATA_BITS_5) & (ui8Parity == MODBUS_PARITY_ODD) & (ui8Stopbit == MODBUS_STOP_BIT_2)){
        return  SERIAL_5O2;
    }else if((ui8Databits == MODBUS_DATA_BITS_6) & (ui8Parity == MODBUS_PARITY_ODD) & (ui8Stopbit == MODBUS_STOP_BIT_2)){
        return  SERIAL_6O2;
    }else if((ui8Databits == MODBUS_DATA_BITS_7) & (ui8Parity == MODBUS_PARITY_ODD) & (ui8Stopbit == MODBUS_STOP_BIT_2)){
        return  SERIAL_7O2;
    }else if((ui8Databits == MODBUS_DATA_BITS_8) & (ui8Parity == MODBUS_PARITY_ODD) & (ui8Stopbit == MODBUS_STOP_BIT_2)){
        return  SERIAL_8O2;
    }else{
        return  SERIAL_8N1;
    }

}

/******************************************************************************
* Function Name: Convert_RTU_To_ASCII
*******************************************************************************
*
* Summary:
*  This function converts the RTU bytes to ASCII char
*
*
* Parameters:
*  MsgSourceAddr, MsgDestAddr, MsgLength
*
* Return:
*  None
*
*******************************************************************************/
uint8_t ModbusUtils::Modbus_Convert_RTU_To_ASCII(uint8_t *MsgSourceAddr, uint8_t *MsgDestAddr,uint8_t MsgLength){
    uint8_t ui8ByteSize = 0;
    uint8_t ui8LoopCounter=0;
    uint8_t ui8Byte=0;
    uint8_t ui8High=0;
    uint8_t ui8Low=0;
    uint8_t ui8LRC=0;

    *MsgDestAddr++ = MODBUS_ASCII_START_BYTE;
    ui8ByteSize++;

    //calculate LRC before convertion
    ui8LRC = Modbus_ASCII_LRC(MsgSourceAddr,MsgLength-2);

    for(ui8LoopCounter=0;ui8LoopCounter<(MsgLength-2);ui8LoopCounter++){
        ui8Byte = *MsgSourceAddr++;

        ui8High = 0;
        if((ui8Byte >> 4) > 9){
            ui8High = 0x07;
        }
        ui8High = ui8High + (ui8Byte >> 4) + 0x30;
        *MsgDestAddr++ = ui8High;
        ui8ByteSize++;

        ui8Low = 0;
        if((ui8Byte & 0x0F) > 9){
            ui8Low = 0x07;
        }
        ui8Low = ui8Low + (ui8Byte & 0x0F) + 0x30;
        *MsgDestAddr++ = ui8Low;
        ui8ByteSize++;
    }

    ui8Byte = ui8LRC;
    ui8High = 0;
    if((ui8Byte >> 4) > 9){
        ui8High = 0x07;
    }
    ui8High = ui8High + (ui8Byte >> 4) + 0x30;
    *MsgDestAddr++ = ui8High;
    ui8ByteSize++;

    ui8Low = 0;
    if((ui8Byte & 0x0F) > 9){
        ui8Low = 0x07;
    }
    ui8Low = ui8Low + (ui8Byte & 0x0F) + 0x30;
    *MsgDestAddr++ = ui8Low;
    ui8ByteSize++;

    *MsgDestAddr++ = MODBUS_ASCII_CR_BYTE;
    ui8ByteSize++;

    *MsgDestAddr++ = MODBUS_ASCII_LF_BYTE;
    ui8ByteSize++;

    return ui8ByteSize;
}


/******************************************************************************
* Function Name: Convert_ASCII_To_RTU
*******************************************************************************
*
* Summary:
*  This function converts the RTU bytes to ASCII char
*
*
* Parameters:
*  MsgSourceAddr, MsgDestAddr, MsgLength
*
* Return:
*  None
*
*******************************************************************************/
uint8_t ModbusUtils::Modbus_Convert_ASCII_To_RTU(uint8_t *MsgSourceAddr, uint8_t *MsgDestAddr,uint8_t MsgLength){
    uint8_t  ui8Byte=0;
    uint8_t  ui8LoopCounter=0;
    uint8_t  ui8ByteSize=0;
    uint8_t  ui8Nibble=0;
    uint16_t ui16CRC=0;

    MsgSourceAddr++;
    MsgLength = (MsgLength-5) / 2;
    for(ui8LoopCounter=0;ui8LoopCounter<MsgLength;ui8LoopCounter++){
        ui8Nibble = *MsgSourceAddr++;
        if(ui8Nibble > '9'){
            ui8Nibble = ui8Nibble - 0x07;
        }
        ui8Nibble = ui8Nibble - '0';

        ui8Byte = ui8Nibble;
        ui8Byte = ui8Byte << 4;

        ui8Nibble = *MsgSourceAddr++;
        if(ui8Nibble > '9'){
            ui8Nibble = ui8Nibble - 0x07;
        }
        ui8Nibble = ui8Nibble - '0';

        ui8Byte = ui8Byte | ui8Nibble;

        *MsgDestAddr++ = ui8Byte;
        ui8ByteSize++;
    }

    ui16CRC = Modbus_CalculateCRC16(MsgDestAddr-ui8ByteSize,ui8ByteSize);
    *MsgDestAddr++ = ui16CRC;
    ui8ByteSize++;
    *MsgDestAddr++ = ui16CRC >> 8;
    ui8ByteSize++;

    return ui8ByteSize;
}

/******************************************************************************
* Function Name: Modbus_ASCII_LRC
*******************************************************************************
*
* Summary:
*  This function calculates LRC
*
* Parameters:
*  MsgSourceAddr, MsgLength
*
* Return:
*  cLRC
*
*******************************************************************************/

char ModbusUtils:: Modbus_ASCII_LRC(uint8_t *MsgSourceAddr, uint8_t MsgLength){
	uint8_t cLRC = 0;

	while (MsgLength != 0){
		cLRC += *MsgSourceAddr++;
		MsgLength--;
	}
    cLRC = 0xFF - cLRC;
    cLRC = cLRC + 1;

	return cLRC;
}

//*****************************************************************************
//*** Modbus CRC calculation as per specification given in MODBUS.ORG
//*** Uses table for polynomial 1010 0000 0000 0001 = 0xA0001
//*** Can be used for string received or string to be transmitted
//*** MsgStartAddr - Starting address of the message
//*** MsgLength    - Length of the message (excluding CRC)
//*** Returns a 16 bit CRC value
//*****************************************************************************
uint16_t ModbusUtils::Modbus_CalculateCRC16(uint8_t *MsgStartAddr, uint8_t MsgLength){
    uint16_t ui16Response=0;
    uint8_t Index;

    uint8_t CRCtmpHi = 0xFF;                      // Initialize CRC register to 0xFFFF
    uint8_t CRCtmpLo = 0xFF;

    while(MsgLength--)                                  // Find CRC for each byte of the string
    {
        Index = CRCtmpLo ^ *MsgStartAddr++;
        CRCtmpLo = CRCtmpHi ^ TabCRCHi[Index];
        CRCtmpHi = TabCRCLo[Index];
    }

    /* Actual
    ui16Response = CRCtmpHi;
    ui16Response = ui16Response << 8;
    ui16Response = ui16Response | CRCtmpLo;
    CRC.Byte.Hi = CRCtmpHi;                             // Update the CRC registers
    CRC.Byte.Lo = CRCtmpLo;
    */
    /* changed */

    ui16Response = CRCtmpLo;
    ui16Response = ui16Response << 8;
    ui16Response = ui16Response | CRCtmpHi;
    CRC.Byte.Hi = CRCtmpHi;                             // Update the CRC registers
    CRC.Byte.Lo = CRCtmpLo;

    return ui16Response;
}

/******************************************************************************
* Function Name: Modbus_Verify_FunctionCode
*******************************************************************************
*
* Summary:
*  This function verify the function code received
*  The function checks for any date byte received in modbus_uart rx buffer
*  sets ui16ModbusSlaveReadResult veriable
*
*
* Parameters:
*  ui8ReceivedByte
*
* Return:
*  Returns 1 if valid function code else 0
*
*******************************************************************************/
uint8_t ModbusUtils::Modbus_Verify_FunctionCode(uint8_t ui8ReceivedByte){
    uint8_t ui8Response=0;
    uint8_t ui8FunCode=0;

    ui8FunCode = ui8ReceivedByte;
    ui8ReceivedByte = ui8ReceivedByte & 0x1F;

    switch(ui8ReceivedByte){
        case MODBUS_FUNC_READ_COIL_REG:
            ui8Response = 1;
            break;
        case MODBUS_FUNC_READ_INPUT_STATUS_REG:
            ui8Response = 1;
            break;
        case MODBUS_FUNC_READ_HOLDING_REG:
            ui8Response = 1;
            break;
        case MODBUS_FUNC_READ_INPUT_REG:
            ui8Response = 1;
            break;
        case MODBUS_FUNC_WRITE_SINGLE_COIL:
            ui8Response = 1;
            break;
        case MODBUS_FUNC_WRITE_SINGLE_REG:
            ui8Response = 1;
            break;
        case MODBUS_FUNC_WRITE_MULTI_COIL:
            ui8Response = 1;
            break;
        case MODBUS_FUNC_WRITE_MULTI_REG:
            ui8Response = 1;
            break;
        default:
            ui8Response = 0;
            break;
    }

    /* proper funcode error */
    if(ui8Response == 1){
        if((ui8FunCode >> 4) == 0x8){
            ui8Response = 2;
        }else if((ui8FunCode >> 4) == 0x9){
            ui8Response = 2;
        }else if((ui8FunCode >> 4) == 0x1){
            ui8Response = 1;
        }else{
            if((ui8FunCode >> 4) != 0x0){
                ui8Response = 0;
            }
        }
    }

    return ui8Response;
}

uint32_t ModbusUtils::Modbus_Get_Baudrate(uint8_t ui8Index){
    return  Modbus_Baud_Display[ui8Index];
}

uint8_t ModbusUtils::Modbus_Get_Parity(uint8_t ui8Index){
    return  Modbus_Parity[ui8Index];
}

uint8_t ModbusUtils::Modbus_Get_TimePerByte(uint8_t ui8Index){
    return ModbusTimePerByte[ui8Index];
}

uint8_t ModbusUtils::Modbus_ExpectedBytes_RTU(uint8_t aui8Buffer[]){
    uint8_t ui8Response=0;
    uint16_t ui16NumberOfRegister=0;
    uint8_t  ui8FunctionCode=0;
    
    ui16NumberOfRegister = aui8Buffer[4];
    ui16NumberOfRegister = ui16NumberOfRegister << 8;
    ui16NumberOfRegister = ui16NumberOfRegister | aui8Buffer[5];
    
    ui8FunctionCode = aui8Buffer[1];
    
    
    if((ui8FunctionCode == MODBUS_FUNC_READ_COIL_REG) || (ui8FunctionCode == MODBUS_FUNC_READ_INPUT_STATUS_REG)){
        ui8Response = ui16NumberOfRegister/8;
        ui8Response = MODBUS_MANDATORY_RESPONSE_BYTES + ui8Response;
    }
    
    else if((ui8FunctionCode == MODBUS_FUNC_READ_HOLDING_REG) || (ui8FunctionCode == MODBUS_FUNC_READ_INPUT_REG)){
        ui8Response = MODBUS_MANDATORY_RESPONSE_BYTES + (ui16NumberOfRegister * 2);
    }
    else{
        ui8Response = 8;
    }
    
    return ui8Response;
}

uint8_t ModbusUtils::Modbus_Get_Error_Code(uint8_t ui8Byte){
    if(ui8Byte == MODBUS_ILLEGAL_FUN_CODE){
        return MODBUS_RTU_ERR_INVALID_FUNC;
    }else if(ui8Byte == MODBUS_ILLEGAL_ADDRESS_CODE){
        return MODBUS_RTU_ERR_INVALID_REG_ADDRESS;
    }else if(ui8Byte == MODBUS_ILLEGAL_VALUE_CODE){
        return MODBUS_RTU_ERR_INVALID_DATA_VALUE;
    }else if(ui8Byte == MODBUS_SLAVE_FAILURE){
        return MODBUS_RTU_ERR_GATEWAY_FAILED;
    }else{
        return MODBUS_RTU_ERR_UNKNOWN;
    }
}

// Preinstantiate Objects //////////////////////////////////////////////////////
#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_TWOWIRE)
ModbusUtils ModUtils;
#endif