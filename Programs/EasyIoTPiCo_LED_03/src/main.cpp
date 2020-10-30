/*******************************************************************************
* Project Name: EasyIoTPiCo_LED_03
*
* Version: 1.0cc
*
* Description:
* In this project EasyIoTPiCo, switches leds on/off using different firmware delay. LED pins are 2 and 14
* Owner:
* Yogesh M Iggalore
*
********************************************************************************
* Copyright (2020-21) , EasyIoTPiCo
*******************************************************************************/

#include <Arduino.h>

#define LED_PIN_1 14
#define LED_PIN_2 2

uint8_t ui8LoopCounter=0;
uint8_t test;

void setup() {
	/* set LED pins to output*/
	pinMode(LED_PIN_1, OUTPUT);
	pinMode(LED_PIN_2, OUTPUT);
}

void loop() {
		
	digitalWrite(LED_PIN_1, LOW);
	for(ui8LoopCounter=0;ui8LoopCounter<5;ui8LoopCounter++){
		digitalWrite(LED_PIN_2, LOW);
		delay(100);
		digitalWrite(LED_PIN_2, HIGH);
		delay(100);
	}

	digitalWrite(LED_PIN_1, HIGH);
	for(ui8LoopCounter=0;ui8LoopCounter<5;ui8LoopCounter++){
		digitalWrite(LED_PIN_2, LOW);
		delay(100);
		digitalWrite(LED_PIN_2, HIGH);
		delay(100);
	}
}