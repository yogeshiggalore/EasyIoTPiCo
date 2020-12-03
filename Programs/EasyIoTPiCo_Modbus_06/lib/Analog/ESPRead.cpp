/*******************************************************************************
* File Name: ESPRead.cpp
*
* Version: 1.0
*
* Description:
* This is source file for esp read . All the functions related to
* analog and dht22 read are implemented in this file
*
* Owner:
* Yogesh M Iggalore
*
* Code Tested With:
* 1. platformio and esp32
*
********************************************************************************
* Copyright (2020-21) , EasyIoT
********************************************************************************/

#include <Arduino.h>
#include <ESPUtils.h>
#include <ESPRead.h>
#include <ModbusTCPSlave.h>

// Constructors ////////////////////////////////////////////////////////////////
ESPRead::ESPRead(void):dht(DHT22_PIN, DHTTYPE){}

void ESPRead::Start(void){
    /* start dht22 sensor */
	dht.begin();

    /* ADC resolution 9 to 12 bit that is 511 to 4095 default 12bit resolution */
	analogReadResolution(ADC_RESOLUTION);

	/* sample width and resolution default 12 bit resolution */
	analogSetWidth(ADC_WIDTH);

	/* number of cycles per sample Range is 1 - 255 default 8 */
	analogSetCycles(ADC_CYCLE);

	/* number of samples in the range. Range is 1 - 255 Default is 1 */
	analogSetSamples(ADC_SAMPLES);

	/* divider for the ADC clock Range is 1 - 255 Default is 1 */
	analogSetClockDiv(ADC_CLOCK_DIV);

	/* attenuation for all ADC channels ADC_0db,ADC_2_5db,ADC_6db and default ADC_11db */
	analogSetAttenuation(ADC_11db);
	
	/* attenuation to pot pins */
	analogSetPinAttenuation(ADC_POT_PIN, ADC_11db);

	/* attach pot pin to ADC */
	adcAttachPin(ADC_POT_PIN);

	/* start pot adc read */
	adcStart(ADC_POT_PIN);
	
	/* attenuation to pot pins */
	analogSetPinAttenuation(ADC_LM35_PIN, ADC_11db);
	
	/* attach lm35 pin to ADC */
	adcAttachPin(ADC_LM35_PIN);

	/* start lm35 adc read */
	adcStart(ADC_LM35_PIN);
}

void ESPRead::Update(void){
    
    if(ui16ESPReadCounter > ESPREAD_TIME_INTERVAL){
        ui16ESPReadCounter = 0;
        Read_DHT22();
        Read_ADC_Values();
        ModTCPS.aui16InputRegister[0] = fTemperature * 10;
        ModTCPS.aui16InputRegister[1] = fHumidity * 10;
        ModTCPS.aui16InputRegister[2] = fLMTemperature * 100;
        ModTCPS.aui16InputRegister[3] = fPotVolt * 100; 
    }
    
}

/* function to read dht22 sensor */
uint8_t ESPRead::Read_DHT22(void){

	uint8_t ui8Response=0;

	/* read humidity */
	fHumidity = dht.readHumidity();

	/* read temperature */
	fTemperature = dht.readTemperature();

	/* check if data is valid */
	if(isnan(fHumidity) || isnan(fTemperature)){

		/* display read error in serial */
		fHeatIndex = 0;
		ui8Response = 0;
		Serial.println("DHT22 reading failure");
        ui8DHTValidFlag = 0;
	}else{

		/* if data is valid read heat index */
		fHeatIndex = dht.computeHeatIndex(fHumidity, fTemperature, false);
		
		ui8Response = 1;
        ui8DHTValidFlag = 1;

		/* display these values */
		Serial.printf("Temperature:%f degC Humidity:%f Heat index:%f\n",fTemperature,fHumidity,fHeatIndex);
	}
	
	return ui8Response;
}

/* read adc values */
void ESPRead::Read_ADC_Values(void){

	/* read potentiometer */
	ui16PotADCValue  = analogRead(ADC_POT_PIN);
	
	/* convert read adc values to voltage */
	fPotVolt = (float)ui16PotADCValue * (3.3 / 4095.0);

	/* read LM35 temperature sensor 10mv */
	ui16LM35ADCValue = analogRead(ADC_LM35_PIN);

	/* convert read adc values to voltage */
	fLMTemperature = (float)ui16LM35ADCValue * (3.3 / 4095.0);
	
	/* every 10milivolt change will result in 1 degree changes */
	fLMTemperature = fLMTemperature / 10.0;

	/* display values to serial */
	Serial.printf("Pot:%f Temperature:%f\n",fPotVolt,fLMTemperature);
}


// Preinstantiate Objects //////////////////////////////////////////////////////
#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_TWOWIRE)
ESPRead ERead;
#endif