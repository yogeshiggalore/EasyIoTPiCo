/*******************************************************************************
*File Name: AnalogRead.cpp
*
* Version: 1.0
*
* Description:
* In this source file for ESP Read
*
*
* Owner:
* Yogesh M Iggalore
*
********************************************************************************
* Copyright (2020-21) EasyIoTPiCo
*******************************************************************************/

#include<Arduino.h>
#include<ESPRead.h>

// Constructors ////////////////////////////////////////////////////////////////
ESPRead::ESPRead(void):dht(DHT11_PIN, DHTTYPE){}

void ESPRead::Start(void){
    /* start dht22 sensor */
	dht.begin();
}

void ESPRead::Update(void){
    
    if(tdfParameters.ui16ESPReadCounter > ESPREAD_TIME_INTERVAL){
        tdfParameters.ui16ESPReadCounter = 0;
        Read_DHT22();
        Read_ADC_Values();
    }
    
}

/* function to read dht22 sensor */
uint8_t ESPRead::Read_DHT22(void){

	uint8_t ui8Response=0;

	/* read humidity */
	tdfParameters.fHumidity = dht.readHumidity();

	/* read temperature */
	tdfParameters.fTemperature = dht.readTemperature();

	/* check if data is valid */
	if(isnan(tdfParameters.fHumidity) || isnan(tdfParameters.fTemperature)){

		/* display read error in serial */
		tdfParameters.fHeatIndex = 0;
		ui8Response = 0;
		Serial.println("DHT22 reading failure");
        tdfParameters.bDHTDataValid = 0;
		tdfParameters.fHeatIndex = 32767;
		tdfParameters.fHumidity = 32767;
		tdfParameters.fTemperature = 32767;
	}else{

		/* if data is valid read heat index */
		tdfParameters.fHeatIndex = dht.computeHeatIndex(tdfParameters.fHumidity, tdfParameters.fTemperature, false);
		
		ui8Response = 1;
        tdfParameters.bDHTDataValid = 1;

		/* display these values */
		Serial.printf("Temperature:%f degC Humidity:%f Heat index:%f\n",tdfParameters.fTemperature,tdfParameters.fHumidity,tdfParameters.fHeatIndex);
	}
	
	return ui8Response;
}

/* read adc values */
void ESPRead::Read_ADC_Values(void){

	/* read LM35 */
	tdfParameters.ui16ADCValue  = analogRead(A0);
	
	/* convert read adc values to voltage */
	tdfParameters.fLMTemperature = (float)tdfParameters.ui16ADCValue * (3.3 / 4095.0);

	/* display values to serial */
	Serial.printf("LM:%f\n",tdfParameters.fLMTemperature);
}

// Preinstantiate Objects //////////////////////////////////////////////////////
#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_TWOWIRE)
ESPRead ERead;
#endif

	