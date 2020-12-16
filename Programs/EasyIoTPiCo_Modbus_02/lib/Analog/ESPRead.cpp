/*******************************************************************************
* File Name: ESPRead.cpp
*
* Version: 1.0
*
* Description:
* This is source file for esp read . All the functions related to
* analog and dht11 read are implemented in this file
*
* Owner:
* Yogesh M Iggalore
*
* Code Tested With:
* 1. platformio and esp8266
*
********************************************************************************
* Copyright (2020-21) , EasyIoTPiCo
********************************************************************************/

#include <Arduino.h>
#include <ESPUtils.h>
#include <ESPRead.h>
#include <ModbusSlave.h>

// Constructors ////////////////////////////////////////////////////////////////
ESPRead::ESPRead(void):dht(DHT11_PIN, DHTTYPE){}

void ESPRead::Start(void){
    /* start dht11 sensor */
	dht.begin();
}

void ESPRead::Update(void){
    
    if(ui16ESPReadCounter > ESPREAD_TIME_INTERVAL){
        ui16ESPReadCounter = 0;
        Read_DHT11();
        Read_ADC_Values();
        Modbus.aui16InputRegister[0] = fTemperature * 10;
        Modbus.aui16InputRegister[1] = fHumidity * 10;
        Modbus.aui16InputRegister[2] = fLMTemperature * 100;
        Modbus.aui16InputRegister[3] = fPotVolt * 100; 
    }
    
}

/* function to read dht11 sensor */
uint8_t ESPRead::Read_DHT11(void){

	uint8_t ui8Response=0;

	/* read humidity */
	fHumidity = dht.readHumidity();

	/* read temperature */
	fTemperature = dht.readTemperature();

	/* check if data is valid */
	if(isnan(fHumidity) || isnan(fTemperature)){

		/* display read error in Serial1 */
		fHeatIndex = 0;
		ui8Response = 0;
		Serial1.println("DHT22 reading failure");
        ui8DHTValidFlag = 0;
	}else{

		/* if data is valid read heat index */
		fHeatIndex = dht.computeHeatIndex(fHumidity, fTemperature, false);
		
		ui8Response = 1;
        ui8DHTValidFlag = 1;

		/* display these values */
		Serial1.printf("Temperature:%f degC Humidity:%f Heat index:%f\n",fTemperature,fHumidity,fHeatIndex);
	}
	
	return ui8Response;
}

/* read adc values */
void ESPRead::Read_ADC_Values(void){

	/* read potentiometer */
	ui16PotADCValue  = 0;
	
	/* convert read adc values to voltage */
	fPotVolt = (float)ui16PotADCValue * (3.3 / 4095.0);

	/* read LM35 temperature sensor 10mv */
	ui16LM35ADCValue = analogRead(ADC_LM35_PIN);

	/* convert read adc values to voltage */
	fLMTemperature = (float)ui16LM35ADCValue * (3.3 / 4095.0);
	
	/* every 10milivolt change will result in 1 degree changes */
	fLMTemperature = fLMTemperature / 10.0;

	/* display values to Serial1 */
	Serial1.printf("Pot:%f Temperature:%f\n",fPotVolt,fLMTemperature);
}


// Preinstantiate Objects //////////////////////////////////////////////////////
#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_TWOWIRE)
ESPRead ERead;
#endif