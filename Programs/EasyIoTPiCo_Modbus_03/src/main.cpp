/*******************************************************************************
* Project Name: EasyIoT_22_03
*
* Version: 1.0
*
* Description:
* In this project esp32, modbus ascii master protocol example
* Owner:
* Yogesh M Iggalore
*
********************************************************************************
* Copyright (2020-21) , EasyIoT
*******************************************************************************/

/* include headers */
#include <Arduino.h>
#include <ESPUtils.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ModbusASCIIMaster.h>
#include <FS.h>
#include <ArduinoJson.h>
#include <ModbusUtils.h>
#include <ModbusASCIIMaster.h>

#define SSID		"IOTDev"
#define PASSWORD 	"IOTDev1234"

/* web page index */
const char* htmlfile = "/index.html";

/* webserver object on port 80 */
ESP8266WebServer ESPServer(80);

/* ESP Server functions*/
void ESPServer_HandleWebRequests(void);
void ESPServer_ModbusUpdate(void);
void ESPServer_HandleRoot(void);
bool ESPServer_LoadFromSpiffs(String path);

void setup(){

	/* start serial with baudrate 921600 */
	Serial1.begin(921600);
    Serial.begin(9600);

	/* wifi begin */
	WiFi.begin(SSID, PASSWORD);

	/* wait for wifi to connect */
	Serial1.println("Connecting to WiFi..");
	while (WiFi.status() != WL_CONNECTED) {
  		delay(500);
  		Serial1.print(".");
	}

	Serial1.println("\nConnected to the WiFi network IP:");
	Serial1.println(WiFi.localIP());

	/* Initialize SPIFFS */
	if(!SPIFFS.begin()){
		Serial1.println("An Error has occurred while mounting SPIFFS");
		while(1);
	}

	/* start modbus rtu master */
	Modbus.Start();

	/* Enable ESP Server function and start webserver */
    ESPServer.on("/",ESPServer_HandleRoot);
    ESPServer.on("/modbusupdate",ESPServer_ModbusUpdate);
    ESPServer.onNotFound(ESPServer_HandleWebRequests);
    ESPServer.begin();
}

void loop(){

	/* function to handle clients */
	ESPServer.handleClient();

}

/* this function used to load all web server related files from spiffs */
bool ESPServer_LoadFromSpiffs(String path){
    String dataType = "text/plain";
    if(path.endsWith("/")) path += "index.htm";
    if(path.endsWith(".src")) path = path.substring(0, path.lastIndexOf("."));
    else if(path.endsWith(".html")) dataType = "text/html";
    else if(path.endsWith(".htm")) dataType = "text/html";
    else if(path.endsWith(".css")) dataType = "text/css";
    else if(path.endsWith(".js")) dataType = "application/javascript";
    else if(path.endsWith(".png")) dataType = "image/png";
    else if(path.endsWith(".gif")) dataType = "image/gif";
    else if(path.endsWith(".jpg")) dataType = "image/jpeg";
    else if(path.endsWith(".ico")) dataType = "image/x-icon";
    else if(path.endsWith(".xml")) dataType = "text/xml";
    else if(path.endsWith(".pdf")) dataType = "application/pdf";
    else if(path.endsWith(".zip")) dataType = "application/zip";
    File dataFile = SPIFFS.open(path.c_str(), "r");
    if (ESPServer.hasArg("download")) dataType = "application/octet-stream";
    if (ESPServer.streamFile(dataFile, dataType) != dataFile.size()) {}

    dataFile.close();
    return true;
}

/* this function updates the modbus on client request */
void ESPServer_ModbusUpdate(void){
   	String sData;
    StaticJsonDocument<1048> doc;
    uint8_t aui8ModbusTxBuf[16];
    uint8_t ui8FunCode=0;
    uint8_t ui8NumberOfReg=0;
    uint8_t ui8ExpectedBytes;
    uint8_t RxByteCount;
    uint16_t ResponseTimeout;
    uint8_t ui8MobdusResponse;
    uint16_t ui16Value;
    String sTxString;
    String sRxString;
    uint8_t ui8LoopCounter=0;
    uint8_t  ui8TxByteCount=0;
    
	ui8FunCode = ESPServer.arg("funcode").toInt();
    ui8NumberOfReg = ESPServer.arg("numbreg").toInt();

    if(ui8FunCode < MODBUS_FUNC_WRITE_SINGLE_COIL){
        aui8ModbusTxBuf[0] = 0x01;  
        aui8ModbusTxBuf[1] = ui8FunCode;
        aui8ModbusTxBuf[2] = 0x00;
        aui8ModbusTxBuf[3] = 0x00;
        aui8ModbusTxBuf[4] = 0x00;
        aui8ModbusTxBuf[5] = ui8NumberOfReg;

        ui8TxByteCount = 6;

        ui8ExpectedBytes = ModUtils.Modbus_ExpectedBytes_RTU(aui8ModbusTxBuf);

        ui8MobdusResponse = Modbus.Modbus_RunCommand(aui8ModbusTxBuf,ui8TxByteCount,ui8ExpectedBytes,&   RxByteCount,300,&ResponseTimeout);

    }else if(ui8FunCode == MODBUS_FUNC_WRITE_SINGLE_COIL){
        aui8ModbusTxBuf[0] = 0x01;  
        aui8ModbusTxBuf[1] = ui8FunCode;
        aui8ModbusTxBuf[2] = 0x00;
        aui8ModbusTxBuf[3] = ui8NumberOfReg;
        
        if(ESPServer.arg("value").toInt()){
            aui8ModbusTxBuf[4] = 0xFF;
            aui8ModbusTxBuf[5] = 0x00;
        }else{
            aui8ModbusTxBuf[4] = 0x00;
            aui8ModbusTxBuf[5] = 0x00;
        }

        ui8TxByteCount = 6;

        ui8ExpectedBytes = ModUtils.Modbus_ExpectedBytes_RTU(aui8ModbusTxBuf);

        ui8MobdusResponse = Modbus.Modbus_RunCommand(aui8ModbusTxBuf,ui8TxByteCount,ui8ExpectedBytes,&   RxByteCount,300,&ResponseTimeout);
    }else if(ui8FunCode == MODBUS_FUNC_WRITE_SINGLE_REG){
        aui8ModbusTxBuf[0] = 0x01;  
        aui8ModbusTxBuf[1] = ui8FunCode;
        aui8ModbusTxBuf[2] = 0x00;
        aui8ModbusTxBuf[3] = ui8NumberOfReg;
        
        ui16Value = ESPServer.arg("value").toInt();
        aui8ModbusTxBuf[4] = ui16Value >> 8;
        aui8ModbusTxBuf[5] = ui16Value;

        ui8TxByteCount = 6;

        ui8ExpectedBytes = ModUtils.Modbus_ExpectedBytes_RTU(aui8ModbusTxBuf);
        MUtils.Modbus_Convert_RTU_To_ASCII();
        ui8MobdusResponse = Modbus.Modbus_RunCommand(aui8ModbusTxBuf,ui8TxByteCount,ui8ExpectedBytes,&   RxByteCount,300,&ResponseTimeout);
    }else if(ui8FunCode == MODBUS_FUNC_WRITE_MULTI_COIL){
        aui8ModbusTxBuf[0] = 0x01;  
        aui8ModbusTxBuf[1] = ui8FunCode;
        aui8ModbusTxBuf[2] = 0x00;
        aui8ModbusTxBuf[3] = 0x00;
        aui8ModbusTxBuf[4] = 0x00;
        aui8ModbusTxBuf[5] = 0x04;
        aui8ModbusTxBuf[6] = 0x01;

        if(ESPServer.arg("value4").toInt()){
            ui16Value = 1;
        }else{
            ui16Value = 0;
        }

        ui16Value = ui16Value << 1;

        if(ESPServer.arg("value3").toInt()){
            ui16Value = ui16Value + 1;
        }

        ui16Value = ui16Value << 1;

        if(ESPServer.arg("value2").toInt()){
           ui16Value = ui16Value + 1;
        }
        ui16Value = ui16Value << 1;

        if(ESPServer.arg("value1").toInt()){
            ui16Value = ui16Value + 1;
        }

        aui8ModbusTxBuf[7] = ui16Value;

        ui8TxByteCount = 8;

        ui8ExpectedBytes = ModUtils.Modbus_ExpectedBytes_RTU(aui8ModbusTxBuf);

        ui8MobdusResponse = Modbus.Modbus_RunCommand(aui8ModbusTxBuf,ui8TxByteCount,ui8ExpectedBytes,&   RxByteCount,300,&ResponseTimeout);
    }else if(ui8FunCode == MODBUS_FUNC_WRITE_MULTI_REG){
        aui8ModbusTxBuf[0] = 0x01;  
        aui8ModbusTxBuf[1] = ui8FunCode;
        aui8ModbusTxBuf[2] = 0x00;
        aui8ModbusTxBuf[3] = 0x00;
        aui8ModbusTxBuf[4] = 0x00;
        aui8ModbusTxBuf[5] = 0x04;
        aui8ModbusTxBuf[6] = 0x08;

        ui16Value = ESPServer.arg("value1").toInt();
        aui8ModbusTxBuf[7] = ui16Value >> 8;
        aui8ModbusTxBuf[8] = ui16Value;

        ui16Value = ESPServer.arg("value2").toInt();
        aui8ModbusTxBuf[9] = ui16Value >> 8;
        aui8ModbusTxBuf[10] = ui16Value;

        ui16Value = ESPServer.arg("value3").toInt();
        aui8ModbusTxBuf[11] = ui16Value >> 8;
        aui8ModbusTxBuf[12] = ui16Value;

        ui16Value = ESPServer.arg("value4").toInt();
        aui8ModbusTxBuf[13] = ui16Value >> 8;
        aui8ModbusTxBuf[14] = ui16Value;

        ui8TxByteCount = 15;

        ui8ExpectedBytes = ModUtils.Modbus_ExpectedBytes_RTU(aui8ModbusTxBuf);

        ui8MobdusResponse = Modbus.Modbus_RunCommand(aui8ModbusTxBuf,ui8TxByteCount,ui8ExpectedBytes,&   RxByteCount,300,&ResponseTimeout);
    }
    
    sTxString = " ";
    for(ui8LoopCounter=0;ui8LoopCounter<(ui8TxByteCount+2);ui8LoopCounter++){
        sTxString = sTxString + String(aui8ModbusTxBuf[ui8LoopCounter],HEX) + " ";
    }

    sRxString = " ";
    for(ui8LoopCounter=0;ui8LoopCounter<RxByteCount;ui8LoopCounter++){
        sRxString = sRxString + String(ModUtils.aui8ModbusRxBuffer[ui8LoopCounter],HEX) + " ";
    }

    if(ui8MobdusResponse == MODBUS_ASCII_ERR_NONE){
        doc["Error"] = 0;

        if((ui8FunCode == 0x03) | (ui8FunCode == 0x04)){
            ui16Value = ModUtils.aui8ModbusRxBuffer[3];
            ui16Value = ui16Value << 8;
            ui16Value = ui16Value | ModUtils.aui8ModbusRxBuffer[4];
            doc["reg1"] = ui16Value;

            ui16Value = ModUtils.aui8ModbusRxBuffer[5];
            ui16Value = ui16Value << 8;
            ui16Value = ui16Value | ModUtils.aui8ModbusRxBuffer[6];
            doc["reg2"] = ui16Value;

            ui16Value = ModUtils.aui8ModbusRxBuffer[7];
            ui16Value = ui16Value << 8;
            ui16Value = ui16Value | ModUtils.aui8ModbusRxBuffer[8];
            doc["reg3"] = ui16Value;

            ui16Value = ModUtils.aui8ModbusRxBuffer[9];
            ui16Value = ui16Value << 8;
            ui16Value = ui16Value | ModUtils.aui8ModbusRxBuffer[10];
            doc["reg4"] = ui16Value;

        }else if((ui8FunCode == 0x01) | (ui8FunCode == 0x02)){
            ui16Value = ModUtils.aui8ModbusRxBuffer[3];
            if(ui16Value & 0x01){
                doc["reg1"] = 1;
            }else{
                doc["reg1"] = 0;
            }
            
            if(ui16Value & 0x02){
                doc["reg2"] = 1;
            }else{
                doc["reg2"] = 0;
            }

            if(ui16Value & 0x04){
                doc["reg3"] = 1;
            }else{
                doc["reg3"] = 0;
            }

            if(ui16Value & 0x08){
                doc["reg4"] = 1;
            }else{
                doc["reg4"] = 0;
            }
        }else{
            //do nothing
        }
    }else{
        if(ui8MobdusResponse == MODBUS_ASCII_ERR_TIMEOUT){
            doc["Error"] = "timeout";
        }else if(ui8MobdusResponse == MODBUS_ASCII_ERR_CRC){
            doc["Error"] = "error crc";
        }else if(ui8MobdusResponse == MODBUS_ASCII_ERR_INVALID_FUNC){
            doc["Error"] = "error funcode";
        }else if(ui8MobdusResponse == MODBUS_ASCII_ERR_INVALID_REG_ADDRESS){
            doc["Error"] = "error invalid addr";
        }else if(ui8MobdusResponse == MODBUS_ASCII_ERR_INVALID_MODBUS_COMMAND){
            doc["Error"] = "error invalid cmd";
        }else{
            doc["Error"] = "error unknow";
        }
        
    }

    /* Measure Signal Strength (RSSI) of Wi-Fi connection */
    if(WiFi.RSSI() <= -100){
        doc["rssi"] = 0;
    }else if(WiFi.RSSI() >= -50){
        doc["rssi"] = 100;
    }else{
        doc["rssi"] = 2 * (WiFi.RSSI() + 100);
    }

    doc["MAC"] = WiFi.macAddress();
    
	doc["LEDState"] = "ON";
    
    doc["TxCMD"] = sTxString;

    doc["RxCMD"] = sRxString;

    doc["funcode"] = ui8FunCode;

    serializeJson(doc, sData);

    ESPServer.send(200, "text/json",sData);
}

/* This handling root file */
void ESPServer_HandleRoot(void){
  ESPServer.sendHeader("Location", "/index.html",true);   //Redirect to our html web page
  ESPServer.send(302, "text/plane","");
}

/* this function handle when wrong url path requented */
void ESPServer_HandleWebRequests(void){
    if(ESPServer_LoadFromSpiffs(ESPServer.uri())) return;
    String message = "File Not Detected\n\n";
    message += "URI: ";
    message += ESPServer.uri();
    message += "\nMethod: ";
    message += (ESPServer.method() == HTTP_GET)?"GET":"POST";
    message += "\nArguments: ";
    message += ESPServer.args();
    message += "\n";
    for (uint8_t i=0; i<ESPServer.args(); i++){
        message += " NAME:"+ESPServer.argName(i) + "\n VALUE:" + ESPServer.arg(i) + "\n";
    }

    ESPServer.send(404, "text/plain", message);
}