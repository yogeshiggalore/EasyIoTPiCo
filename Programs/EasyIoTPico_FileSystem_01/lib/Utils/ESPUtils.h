/*******************************************************************************
*File Name: ESPUtils.c
*
* Version: 1.0
*
* Description:
* In this source for utility functions
*
*
* Owner:
* Yogesh M Iggalore
*
********************************************************************************
* Copyright (2020-21) , EasyIoTPiCo
*******************************************************************************/

#ifndef ESPUtils_h
#define ESPUtils_h

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>


class ESPUtils{
public:
    ESPUtils();
    String sMACAddress;
    String sDeviceName;
    String sHostNameWifi;
    long lMACAddress;
    uint8 ui8DebugEnable;
    void  Start(void);
    String Get_MAC(void);
    String Get_Device_Name(void);
    void Set_Host_Name(void);
    uint8 Get_Serial_Debug(void);
    String GetTime(void);
    int Get_Day(void);
    int Get_Hours(void);
    int Get_Minutes(void);
    int Get_Seconds(void);
    int Get_Date(void);
    int Get_Month(void);
    int Get_Year(void);
    byte DecToBcd(byte bValue);
    byte BcdToDec(byte val);
    uint8_t Convert_ASCIIHex_To_Hex(char sASCIIHex[]);
    void Disable_Serial_Debug(void);
    void Enable_Serial_Debug(void);
    void Parse_String_To_Bytes(const char* str, char sep, uint8_t* bytes, int maxBytes, int base);
    size_t printf(const char * format, ...)  __attribute__ ((format (printf, 2, 3)));
    size_t printf_P(PGM_P format, ...) __attribute__((format(printf, 2, 3)));
    size_t print(const __FlashStringHelper *);
    size_t print(const String &);
    size_t print(const char[]);
    size_t print(char);
    size_t print(unsigned char, int = DEC);
    size_t print(int, int = DEC);
    size_t print(unsigned int, int = DEC);
    size_t print(long, int = DEC);
    size_t print(unsigned long, int = DEC);
    size_t print(double, int = 2);
    size_t print(const Printable&);

    size_t println(const __FlashStringHelper *);
    size_t println(const String &s);
    size_t println(const char[]);
    size_t println(char);
    size_t println(unsigned char, int = DEC);
    size_t println(int, int = DEC);
    size_t println(unsigned int, int = DEC);
    size_t println(long, int = DEC);
    size_t println(unsigned long, int = DEC);
    size_t println(double, int = 2);
    size_t println(const Printable&);
    size_t println(void);
private:
    WiFiUDP ntpUDP;
    NTPClient timeClient;
};


#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_TWOWIRE)
extern ESPUtils EUtils;
#endif
#endif
