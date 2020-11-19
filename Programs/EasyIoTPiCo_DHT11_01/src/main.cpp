/*******************************************************************************
* Project Name: EasyIoTPiCo_DHT11_01
*
* Version: 1.0
*
* Description:
* This project tests ESP8266 with DHT11
* 
*
* Owner:
* Yogesh M Iggalore
********************************************************************************
* Copyright (2020-21) , EasyIoTPiCo
*******************************************************************************/

#include <Arduino.h>
#include <DHT.h>

/* define dht11 pins */
#define DHT11_PIN	14

/* define type as DHT11 */
#define DHTTYPE 	DHT11 

/* create instance of DHT11 */
DHT dht(DHT11_PIN, DHTTYPE);

float fHumidity=0;
float fTemperature=0;
float fHeatIndex=0;

void setup() {
	
	/* start serial with baudrate 9600 */
	Serial.begin(9600);

	/* start dht11 */
	dht.begin();

	Serial.println("DHT11 example ");
}

void loop() {

	/* read humidity */
	fHumidity = dht.readHumidity();

	/* read temperature */
	fTemperature = dht.readTemperature();

	/* check if data is valid */
	if(isnan(fHumidity) || isnan(fTemperature)){

		/* display read error in serial */
		fHeatIndex = 0;
		Serial.println("DHT11 reading failure");
	}else{

		/* if data is valid read heat index */
		fHeatIndex = dht.computeHeatIndex(fHumidity, fTemperature, false);
		
		/* display these values */
		Serial.printf("Temperature:%f degC Humidity:%f Heat index:%f\n",fTemperature,fHumidity,fHeatIndex);
	}

	delay(1000);
}