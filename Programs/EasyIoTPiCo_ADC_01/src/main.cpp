/*******************************************************************************
* Project Name: EasyIoTPiCo_ADC_01
*
* Version: 1.0
*
* Description:
* This project tests ESP8266 ADC
* 
*
* Owner:
* Yogesh M Iggalore
********************************************************************************
* Copyright (2020-21) , EasyIoTPiCo
*******************************************************************************/

#include<Arduino.h>

#define ANALOG_PIN	A0

/* ADC veriables */
uint16_t ui16ADCValues=0;
float    fVoltage=0;
float    fTemperature;

void setup(void){
	/* start serial with baudrate 9600 */
	Serial.begin(9600);

	Serial.println("ADC example ");
}

void loop(void){

	/* read ADC */
	ui16ADCValues = analogRead(ANALOG_PIN);

	/* convert to voltage */
	fVoltage = (float)ui16ADCValues * (3300 / 4095);

	/* convert voltage to temperature */
	fTemperature = fVoltage / 10;

	Serial.print("Temperature :");
	Serial.print(fTemperature);
	Serial.println(" degree C");
	delay(1000);
}

