/*******************************************************************************
*File Name: ESPFile.cpp
*
* Version: 1.0
*
* Description:
* In this source code for managing files in SPIFFS
*
*
* Owner:
* Yogesh M Iggalore
*
********************************************************************************
* Copyright (2020-21) , EasyIoTPiCo
*******************************************************************************/
#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include <ESPUtils.h>
#include <ESPFile.h>

// Constructors ////////////////////////////////////////////////////////////////
ESPFile::ESPFile(){}

/****************************************************************************** 
* Function Name: Start
*******************************************************************************
*
* Summary:
*  This function calls on board starts, it check for files to be present in flash
*  If any file missing it will create.
*
* Parameters:  
*  None
*
* Return: 
*  None
*  
*******************************************************************************/
void ESPFile::Start(void){
    uint8_t ui8LoopCounter=0;
    uint8_t ui8Status=0;
    String sLog;
    String sFileName;

    Read_FileInfo();

    for(ui8LoopCounter=0;ui8LoopCounter<MAX_FILE_COUNT;ui8LoopCounter++){
        ui8Status = CheckFile(sESPFiles[ui8LoopCounter],&aui32ESPFileSize[ui8LoopCounter]);
        if(ui8Status == FILE_NOT_EXIST){
            Serial.printf("File %s doenot exist\r\n",sESPFiles[ui8LoopCounter]);
            ui8Status = CreateFile(sESPFiles[ui8LoopCounter],&aui32ESPFileSize[ui8LoopCounter]);
            if(ui8Status == FILE_CREATE_SUCCESS){
                Serial.printf("File %s created and size %d \r\n",sESPFiles[ui8LoopCounter],aui32ESPFileSize[ui8LoopCounter]);
            }else{
                Serial.printf("File %s create fail\r\n",sESPFiles[ui8LoopCounter]);
            }
        }else{
            Serial.printf("File %s exist\r\n",sESPFiles[ui8LoopCounter]);
        }
        Serial.println("Complete current");
    }
    Serial.println("Complete all");
}

/****************************************************************************** 
* Function Name: Read_FileInfo
*******************************************************************************
*
* Summary:
*  Flash File system info 
*
* Parameters:  
*  None
*
* Return: 
*  None
*  
*******************************************************************************/
void ESPFile::Read_FileInfo(void){

    SPIFFS.info(fFileInfo);
    Serial.println("SPIFFS file info \r\n");
    Serial.printf("totalBytes    : %d \r\n",fFileInfo.totalBytes);
    Serial.printf("usedBytes     : %d \r\n",fFileInfo.usedBytes);
    Serial.printf("blockSize     : %d \r\n",fFileInfo.blockSize);
    Serial.printf("pageSize      : %d \r\n",fFileInfo.pageSize);
    Serial.printf("maxOpenFiles  : %d \r\n",fFileInfo.maxOpenFiles);
    Serial.printf("maxPathLength : %d \r\n",fFileInfo.maxPathLength);
}

/****************************************************************************** 
* Function Name: CreateFile
*******************************************************************************
*
* Summary:
*  This function creates file with given file name and returns its size and status.
*
* Parameters:  
*  sFileName, ui32FileSize
*
* Return: 
*  ui8FileStatus
*  
*******************************************************************************/
uint8_t ESPFile::CreateFile(String sFileName,uint32_t* ui32FileSize){
    String sData;
    uint8_t ui8FileStatus;

    File fFile = SPIFFS.open(sFileName,"r");
    if(fFile){
        ui8FileStatus = FILE_ALREADY_CREATED;
    }else{
        fFile = SPIFFS.open(sFileName,"w");
        if(!fFile){
            ui8FileStatus =  FILE_CREATE_FAIL;
        }else{
            sData = " ";
            fFile.print(sData);
            *ui32FileSize = fFile.size();
            ui8FileStatus =  FILE_CREATE_SUCCESS;
        }
    }

    fFile.close();
    return ui8FileStatus;
}

/****************************************************************************** 
* Function Name: DeleteFile
*******************************************************************************
*
* Summary:
*  This function deleted file with given file name and returns its size and status.
*
* Parameters:  
*  sFileName, ui32FileSize
*
* Return: 
*  ui8FileStatus
*  
*******************************************************************************/
uint8_t ESPFile::DeleteFile(String sFileName,uint32_t* ui32FileSize){
    uint8_t ui8FileStatus;
    File fFile = SPIFFS.open(sFileName,"r");

    if(!fFile){
        ui8FileStatus = FILE_NOT_EXIST;
        fFile.close();
    }else{
        SPIFFS.remove(sFileName);
        delay(1000);
        fFile = SPIFFS.open(sFileName,"r");
        if(fFile){
            ui8FileStatus = FILE_DELETE_FAIL;
            fFile.close();
        }else{
            ui8FileStatus = FILE_DELETE_SUCCESS;
        }
    }

    *ui32FileSize = 0;

    return ui8FileStatus;
}

/****************************************************************************** 
* Function Name: CheckFile
*******************************************************************************
*
* Summary:
*  This function checks whether file is present or not with given file name and 
*  returns its size and status.
*
* Parameters:  
*  sFileName, ui32FileSize
*
* Return: 
*  ui8FileStatus
*  
*******************************************************************************/
uint8_t ESPFile::CheckFile(String sFileName,uint32_t* ui32FileSize){
    uint8_t ui8FileStatus;
    File fFile = SPIFFS.open(sFileName,"r");
    if(fFile){
        *ui32FileSize = fFile.size();
        fFile.close();
        ui8FileStatus = FILE_EXIST;
    }else{
        *ui32FileSize = 0;
        ui8FileStatus = FILE_NOT_EXIST;
    }

    return ui8FileStatus;
}

/****************************************************************************** 
* Function Name: Write_Data_To_File
*******************************************************************************
*
* Summary:
*  This function writes data to file
*
* Parameters:  
*  sFileName, sData
*
* Return: 
*  ui8FileStatus
*  
*******************************************************************************/
uint8_t ESPFile::Write_Data_To_File(String sFileName, String sData){
    uint8_t ui8Status;
    String sLog;

    File fFile = SPIFFS.open(sFileName,"w");
    if(!fFile){
        ui8Status = FILE_STORE_FAIL;
        sLog = "FILE_STORE_FAIL " + sFileName;
    }else{
        fFile.println(sData);
        ui8Status = FILE_STORE_SUCCESS;
    }

    fFile.close();

    return ui8Status;
}

/****************************************************************************** 
* Function Name: Read_Data_From_File
*******************************************************************************
*
* Summary:
*  This function reads data to file
*
* Parameters:  
*  sFileName
*
* Return: 
*  sString
*  
*******************************************************************************/
String ESPFile::Read_Data_From_File(String sFileName){
    String sString;
    uint16_t ui16Length=0;

    File fFile = SPIFFS.open(sFileName,"r");
    
    if(fFile){
        sString = fFile.readStringUntil('\n');
    }
    
    fFile.close();

    if(sString){
        ui16Length = sString.length();
        sString.remove(ui16Length-1);
    }
    
    return sString;
}

/****************************************************************************** 
* Function Name: Get_File_Size
*******************************************************************************
*
* Summary:
*  This function returns size of requested file
*
* Parameters:  
*  ui8Index
*
* Return: 
*  aui32ESPFileSize
*  
*******************************************************************************/
uint32_t ESPFile::Get_File_Size(uint8_t ui8Index){
    return aui32ESPFileSize[ui8Index];
}

/****************************************************************************** 
* Function Name: Delete_All_Files
*******************************************************************************
*
* Summary:
*  This function deletes all files 
*
* Parameters:  
*  None
*
* Return: 
*  None
*  
*******************************************************************************/
void ESPFile::Delete_All_Files(void){
    uint8_t ui8LoopCounter=0;
    uint8_t ui8Status;

    Serial.println("Deleting all files");
    for(ui8LoopCounter=0;ui8LoopCounter<MAX_FILE_COUNT;ui8LoopCounter++){
        ui8Status = CheckFile(sESPFiles[ui8LoopCounter],&aui32ESPFileSize[ui8LoopCounter]);
        if(ui8Status == FILE_EXIST){
            ui8Status = DeleteFile(sESPFiles[ui8LoopCounter],&aui32ESPFileSize[ui8LoopCounter]);
            if(ui8Status == FILE_DELETE_SUCCESS){
                Serial.printf("File %s deleted\r\n",sESPFiles[ui8LoopCounter]);
            }else{
                Serial.printf("File %s deleted fail\r\n",sESPFiles[ui8LoopCounter]);
            }
        }
    }
}

/****************************************************************************** 
* Function Name: File_Test
*******************************************************************************
*
* Summary:
*  This function test read writes of file 
*
* Parameters:  
*  sFileName
*
* Return: 
*  None
*  
*******************************************************************************/
void ESPFile::File_Test(String sFileName){
    EUtils.println("File_Test");
    EUtils.println("Writing to file : Test Test Test");
    
    if(Write_Data_To_File(sFileName,"Test Test Test") == FILE_STORE_SUCCESS){
        delay(1000);
        EUtils.println("Read data");
        EUtils.println(Read_Data_From_File(sFileName));
    }
}

/****************************************************************************** 
* Function Name: File_Test_Read_All
*******************************************************************************
*
* Summary:
*  This function reads all files and print its data
*
* Parameters:  
*  None
*
* Return: 
*  None
*  
*******************************************************************************/
void ESPFile::File_Test_Read_All(void){
    uint8_t ui8LoopCounter;
    for(ui8LoopCounter=0;ui8LoopCounter<MAX_FILE_COUNT;ui8LoopCounter++){
        Serial.print("Reading file ");
        Serial.println(sESPFiles[ui8LoopCounter]);
        Serial.print("Data: ");
        Serial.println(Read_Data_From_File(sESPFiles[ui8LoopCounter]));
    }
}

// Preinstantiate Objects //////////////////////////////////////////////////////
#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_TWOWIRE)
ESPFile EFile;
#endif
