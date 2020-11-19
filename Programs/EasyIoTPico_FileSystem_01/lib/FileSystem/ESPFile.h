/*******************************************************************************
*File Name: ESPFile.h
*
* Version: 1.0
*
* Description:
* In this header used for managing files in spiffs system
*
*
* Owner:
* Yogesh M Iggalore
*
********************************************************************************
* Copyright (2020-21) , EasyIoTPiCo
*******************************************************************************/
#ifndef ESPFILE_h
#define ESPFILE_h
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <FS.h>

#define FILE_NONE                0
#define FILE_NOT_EXIST           1
#define FILE_EXIST               2
#define FILE_ALREADY_CREATED     3
#define FILE_CREATE_SUCCESS      4
#define FILE_CREATE_FAIL         5
#define FILE_READ_FAIL           6
#define FILE_READ_SUCCESS        7
#define FILE_WRITE_FAIL          8
#define FILE_WRITE_SUCCESS       9
#define FILE_APPEND_FAIL         10
#define FILE_APPEND_SUCCESS      11
#define FILE_DELETE_FAIL         12
#define FILE_DELETE_SUCCESS      13
#define FILE_STORE_FAIL          14
#define FILE_STORE_SUCCESS       15
#define FILE_NAME_ERROR          16
#define FILE_LOG_FAIL            17
#define FILE_LOG_SUCESS          18


#define ALEXA_TOKEN_INDEX               0
#define ALEXA_ENTROL_ID_INDEX           1
#define ALEXA_ENTROL_NAME_INDEX         2

#define MQTT_PUB_VOLTAGE_TOPIC_INDEX    3
#define MQTT_PUB_CURRENT_TOPIC_INDEX    4
#define MQTT_PUB_POWER_TOPIC_INDEX      5
#define MQTT_PUB_UNIT_TOPIC_INDEX       6
#define MQTT_PUB_FREQUENCY_TOPIC_INDEX  7
#define MQTT_PUB_PF_TOPIC_INDEX         8

#define MQTT_SUB_GETALL_TOPIC_INDEX     9

#define MQTT_SERVER_INDEX               10
#define MQTT_PORT_INDEX                 11 
#define MQTT_USERNAME_INDEX             12
#define MQTT_PASSWORD_INDEX             13
#define MQTT_CLIENTID_INDEX             14

#define MQTT_QOS_INDEX                  15
#define MQTT_KEEPALIVE_INDEX            16

#define WIFI_1_SSID_INDEX               17
#define WIFI_2_SSID_INDEX               18
#define WIFI_3_SSID_INDEX               19
#define WIFI_4_SSID_INDEX               20
#define WIFI_1_PASSWORD_INDEX           21
#define WIFI_2_PASSWORD_INDEX           22
#define WIFI_3_PASSWORD_INDEX           23
#define WIFI_4_PASSWORD_INDEX           24
#define WIFI_1_PARAMETER_INDEX          25
#define WIFI_2_PARAMETER_INDEX          26
#define WIFI_3_PARAMETER_INDEX          27
#define WIFI_4_PARAMETER_INDEX          28
#define WIFI_HOSTNAME_INDEX             29

#define HOST_PASSWORD_INDEX             30

/*#define MAX_FILE_COUNT                  31

static const char *sESPFiles[] = {
    "/AlexaToken.txt",
    "/AlexaeNtroLId.txt",
    "/AlexaeNtroLName.txt",
    "/MqttPubVoltageTopic.txt",
    "/MqttPubCurrentTopic.txt",
    "/MqttPubPowerTopic.txt",
    "/MqttPubUnitTopic.txt",
    "/MqttPubFrequencyTopic.txt",
    "/MqttPubPfTopic.txt",
    "/MqttSubGetAllTopic.txt",
    "/MqttServer.txt",
    "/MqttPort.txt",
    "/MqttUsername.txt",
    "/MqttPassword.txt",
    "/MqttClientid.txt",
    "/MqttQos.txt",
    "/MqttKeepalive.txt",
    "/Wifi1SSID.txt",
    "/Wifi2SSID.txt",
    "/Wifi3SSID.txt",
    "/Wifi4SSID.txt",
    "/Wifi1Password.txt",
    "/Wifi2Password.txt",
    "/Wifi3Password.txt",
    "/Wifi4Password.txt",
    "/Wifi1Parameter.txt",
    "/Wifi2Parameter.txt",
    "/Wifi3Parameter.txt",
    "/Wifi4Parameter.txt",
    "/WifiHostname.txt",
    "/Hostpassword.txt"
};
*/

#define MAX_FILE_COUNT                  4

static const char *sESPFiles[] = {
    "/testfile1.txt",
    "/testfile2.txt",
    "/testfile3.txt",
    "/testfile4.txt"
};

class ESPFile{
public:
    uint32_t aui32ESPFileSize[MAX_FILE_COUNT];
    FSInfo fFileInfo;

    ESPFile();
    void Start(void);
    uint8_t CreateFile(String sFileName,uint32_t *ui32FileSize);
    uint8_t DeleteFile(String sFileName,uint32_t *ui32FileSize);
    uint8_t CheckFile(String sFileName,uint32_t *ui32FileSize);
    uint32_t Get_File_Size(uint8_t ui8Index);
    uint32_t Get_LogFile_Size(void);
    void Delete_All_Files(void);
    void Read_FileInfo(void);
    uint8_t Write_Data_To_File(String sFileName, String sData);
    String Read_Data_From_File(String sFileName);

    void File_Test(String sFileName);
    void File_Test_Read_All(void);
};

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_TWOWIRE)
extern ESPFile EFile;
#endif
#endif
