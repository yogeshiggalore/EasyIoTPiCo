/*******************************************************************************
* Project Name: eNtroL_LED_Web_Memory_01
*
* Version: 1.0
*
* Description:
* In this project ESP8266 handles web pages using spiffs and controls LED and stores LED 
* status in EEPROM
* Owner:
* Yogesh M Iggalore
*
********************************************************************************
* Copyright (2020-21) , eNtroL
*******************************************************************************/

/* include headers */
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include <ArduinoJson.h>
#include <EEPROM.h>

#define ACCESS_POINT_NAME     "myesp"
#define ACCESS_POINT_PASSWORD "test1234"

#define MEMORY_SIZE       10

#define MEMORY_LED1_BYTE        0
#define MEMORY_INIT_BYTE        9

uint8_t ui8MemoryLED1Value=0;

/* led web page index */
const char* htmlfile = "/ledweb.html";

/* Static i config for server */
IPAddress local_IP(192,168,4,1);
IPAddress gateway(192,168,4,0);
IPAddress subnet(255,255,255,0);

/* webserver object on port 80 */
ESP8266WebServer ESPServer(80);

/* ESP Server functions*/
void ESPServer_HandleWebRequests(void);
void ESPServer_LedUpdate(void);
void ESPServer_HandleRoot(void);
bool ESPServer_LoadFromSpiffs(String path);

/* Memory function */
void EEPROM_Init(void);
void EEPROM_Update_All(void);

void setup(){
	
    delay(1000);

    /* start serial port with speed 921600 */
    Serial.begin(921600);
	Serial.setDebugOutput(false);
	Serial.println("eNtroL_led_02");
	
    /* start EEPROM */
    EEPROM.begin(MEMORY_SIZE);
    EEPROM_Init();

    
	/* Initialize SPIFFS */
	if(!SPIFFS.begin()){
		Serial.println("An Error has occurred while mounting SPIFFS");
		while(1);
	}

	/* Enable ESP Server function and start webserver */
    ESPServer.on("/",ESPServer_HandleRoot);
    ESPServer.on("/ledupdate",ESPServer_LedUpdate);
    ESPServer.onNotFound(ESPServer_HandleWebRequests);
    ESPServer.begin();

    /* start soft access point connections */
	WiFi.softAPConfig(local_IP, gateway, subnet);
	WiFi.softAP(ACCESS_POINT_NAME,ACCESS_POINT_PASSWORD);
	Serial.print("Soft Aceess point address: ");
	Serial.println(WiFi.softAPIP().toString());

    /* set led pin as output and set to low */
	pinMode(2,OUTPUT);
	/* update led memory status to led */
    if(ui8MemoryLED1Value == HIGH){
        digitalWrite(2,HIGH);
    }else{
        digitalWrite(2,LOW);
    }

    
}

void loop(){
    /* function to handle clients */
	ESPServer.handleClient();
}

/* this function used to load all web server related files from spiffs */
bool ESPServer_LoadFromSpiffs(String path){
    String dataType = "text/plain";
    if(path.endsWith("/")) path += "ledweb.htm";
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

/* this function updates the led status on client request */
void ESPServer_LedUpdate(void){
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
    
    Serial.println(ESPServer.arg("state"));
	if (ESPServer.arg("state")=="0"){
		digitalWrite(2,LOW);
        EEPROM.write(MEMORY_LED1_BYTE,0);
        EEPROM.commit();
		Serial.println("OFF");
        doc["LEDState"] = "OFF";
	}else{
		digitalWrite(2,HIGH);
        EEPROM.write(MEMORY_LED1_BYTE,1);
        EEPROM.commit();
		Serial.println("ON");
        doc["LEDState"] = "ON";
	}

    serializeJson(doc, sData);

    ESPServer.send(200, "text/json",sData);
}

/* This handling root file */
void ESPServer_HandleRoot(void){
  ESPServer.sendHeader("Location", "/ledweb.html",true);   //Redirect to our html web page
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

/* ini memory and update all bytes*/
void EEPROM_Init(void){
    if(EEPROM.read(MEMORY_INIT_BYTE) != 1){
        EEPROM.write(MEMORY_LED1_BYTE,0);
        EEPROM.write(MEMORY_INIT_BYTE,1);
        EEPROM.commit();
    }

	/* update all memory bytes */
	EEPROM_Update_All();
}

/* This function updates all memory bytes */
void EEPROM_Update_All(void){
	ui8MemoryLED1Value = EEPROM.read(MEMORY_LED1_BYTE);
}
