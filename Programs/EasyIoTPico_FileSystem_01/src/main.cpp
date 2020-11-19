/*******************************************************************************
* Project Name: EasyIoTPiCo_FileSystem_01
*
* Version: 1.0
*
* Description:
* In this project ESP8266 handles file system using spiffs
*
* Owner:
* Yogesh M Iggalore
*
********************************************************************************
* Copyright (2020-21) , EasyIoTPiCo
*******************************************************************************/

#include <Arduino.h>
/* include file system supporting library */
#include <FS.h>
#include <ESPFile.h>
#include <ESPUtils.h>

FSInfo fs_info;

void setup() {
    /* start uart1 with baudrate 921600*/
	Serial.begin(921600);

	Serial.println("EasyIoTPiCo_FileSystem_01");
	
    /* Initialize File System */
    if(SPIFFS.begin()){
        EUtils.println("File System Initialized sucess ");
    }else{
        EUtils.println("File System Initialized failed ");
    }

    /* display file system info */
    SPIFFS.info(fs_info);
    EUtils.println("File Info");    
    EUtils.println(fs_info.totalBytes);
    EUtils.println(fs_info.usedBytes);
    EUtils.println(fs_info.blockSize);
    EUtils.println(fs_info.pageSize);
    EUtils.println(fs_info.maxOpenFiles);
    EUtils.println(fs_info.maxPathLength);
    
    /* start file system and read all files */
    EFile.Start();
	EFile.File_Test_Read_All();
}

void loop() {
	
}