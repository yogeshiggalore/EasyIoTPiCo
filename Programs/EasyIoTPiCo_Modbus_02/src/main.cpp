/*******************************************************************************
* Project Name: EasyIoTPiCo_Modbus_02
*
* Version: 1.0
*
* Description:
* In this project esp32, modbus rtu slave protocol example
* Owner:
* Yogesh M Iggalore
*
********************************************************************************
* Copyright (2020-21) , EasyIoTPiCo
*******************************************************************************/

/* include headers */
#include <Arduino.h>
#include <ESPUtils.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ModbusSlave.h>
#include <FS.h>
#include <ArduinoJson.h>
#include <ModbusUtils.h>
#include <ESPRead.h>
#include <ESPMemory.h>

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

/* timer and intruppt function for 1 milisecond */
void Timer_1ms_Start(void);
void IRAM_ATTR timercallback1ms();

void setup(){

	/* start serial with baudrate 921600 */
	Serial.begin(9600);
    Serial1.begin(921600);
    
	/* wifi begin */
	WiFi.begin(SSID, PASSWORD);

	/* wait for wifi to connect */
	Serial1.println("Connecting to WiFi..");
	while (WiFi.status() != WL_CONNECTED) {
  		delay(500);
  		Serial1.print(".");
	}

    /* Start EEPROM */
    EMemory.Init();

    /* LED pin */
    pinMode(LED1_PIN,OUTPUT);
    if(Modbus.ui8CoilStatus & 0x01){
        digitalWrite(LED1_PIN,HIGH);
    }else{
        digitalWrite(LED1_PIN,LOW);
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

    /* start timer module */
	noInterrupts();
    Timer_1ms_Start();
    interrupts();

    /* start esp analog and dht read */
    ERead.Start();

}

void loop(){

	/* handle modbus request */
	Modbus.Update();

	/* function to handle clients */
	ESPServer.handleClient();

    /* handle ESP read */
    ERead.Update();
}

/* This function sets 1 milisecond timer */
void Timer_1ms_Start(void){
	timer1_isr_init();
    timer1_attachInterrupt(timercallback1ms);
    timer1_enable(TIM_DIV1, TIM_EDGE, TIM_LOOP); //80MHZ
    timer1_write(80000);//1ms
}

/* callback function 1ms interrupt */
void IRAM_ATTR timercallback1ms() {
	ERead.ui16ESPReadCounter++;
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
    
    /* Measure Signal Strength (RSSI) of Wi-Fi connection */
    if(WiFi.RSSI() <= -100){
        doc["rssi"] = 0;
    }else if(WiFi.RSSI() >= -50){
        doc["rssi"] = 100;
    }else{
        doc["rssi"] = 2 * (WiFi.RSSI() + 100);
    }

    doc["MAC"] = WiFi.macAddress();

    doc["TxCMD"] = Modbus.sWebServerModbusTxCopy;

    doc["RxCMD"] = Modbus.sWebServerModbusTxCopy;;

    doc["funcode"] = Modbus.ui8LastFunCodeRequested;

    if(Modbus.ui8ModbusErrorCode == MODBUS_RTU_ERR_TIMEOUT){
        doc["Error"] = "timeout";
    }else if(Modbus.ui8ModbusErrorCode == MODBUS_RTU_ERR_CRC){
        doc["Error"] = "error crc";
    }else if(Modbus.ui8ModbusErrorCode == MODBUS_RTU_ERR_INVALID_FUNC){
        doc["Error"] = "error funcode";
    }else if(Modbus.ui8ModbusErrorCode == MODBUS_RTU_ERR_INVALID_REG_ADDRESS){
        doc["Error"] = "error invalid addr";
    }else if(Modbus.ui8ModbusErrorCode == MODBUS_RTU_ERR_INVALID_MODBUS_COMMAND){
        doc["Error"] = "error invalid cmd";
    }else{
        doc["Error"] = "error unknow";
    }

    doc["RxCount"] = Modbus.ui16ModbusRxCount;
    doc["TxCount"] = Modbus.ui16ModbusTxCount;

    doc["holdR1"] = Modbus.aui16HoldingRegister[0];
    doc["holdR2"] = Modbus.aui16HoldingRegister[1];
    doc["holdR3"] = Modbus.aui16HoldingRegister[2];
    doc["holdR4"] = Modbus.aui16HoldingRegister[3];

    doc["inptR1"] = Modbus.aui16InputRegister[0];
    doc["inptR2"] = Modbus.aui16InputRegister[1];
    doc["inptR3"] = Modbus.aui16InputRegister[2];
    doc["inptR4"] = Modbus.aui16InputRegister[3];

    if(Modbus.ui8CoilStatus & 0x01){
        doc["coilB1"] = 1;
    }else{
        doc["coilB1"] = 0;
    }
    
    if(Modbus.ui8CoilStatus & 0x02){
        doc["coilB2"] = 1;
    }else{
        doc["coilB2"] = 0;
    }

    if(Modbus.ui8CoilStatus & 0x04){
        doc["coilB3"] = 1;
    }else{
        doc["coilB3"] = 0;
    }

    if(Modbus.ui8CoilStatus & 0x08){
        doc["coilB4"] = 1;
    }else{
        doc["coilB4"] = 0;
    }

    if(Modbus.ui8InputStatus & 0x01){
        doc["statusB1"] = 1;
    }else{
        doc["statusB1"] = 0;
    }
    
    if(Modbus.ui8InputStatus & 0x02){
        doc["statusB2"] = 1;
    }else{
        doc["statusB2"] = 0;
    }

    if(Modbus.ui8InputStatus & 0x04){
        doc["statusB3"] = 1;
    }else{
        doc["statusB3"] = 0;
    }

    if(Modbus.ui8InputStatus & 0x08){
        doc["statusB4"] = 1;
    }else{
        doc["statusB4"] = 0;
    }

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
