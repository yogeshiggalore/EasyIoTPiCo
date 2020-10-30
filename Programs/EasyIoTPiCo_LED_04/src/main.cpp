/*******************************************************************************
* Project Name: EasyIoTPiCo_LED_04
*
* Version: 1.0cc
*
* Description:
* In this project EasyIoTPiCo, switches led1 blinks 3 times then off, led2 blinks 3 times
* Owner:
* Yogesh M Iggalore
*
********************************************************************************
* Copyright (2020-21) , EasyIoTPiCo
*******************************************************************************/

#include <Arduino.h>

#define LED_PIN_1 2
#define LED_PIN_2 14

#define BLINK_COUNT		2
#define BLINK_TIME		1000

uint8_t ui8LoopCounter=0;

void setup() {
	/* set LED pins to output*/
	pinMode(LED_PIN_1, OUTPUT);
	pinMode(LED_PIN_2, OUTPUT);
}

void loop() {

	/* led1 is off */	
	digitalWrite(LED_PIN_1, HIGH); 
	for(ui8LoopCounter=0;ui8LoopCounter<BLINK_COUNT;ui8LoopCounter++){
		digitalWrite(LED_PIN_2, HIGH);
		delay(BLINK_TIME);
		digitalWrite(LED_PIN_2, LOW);
		delay(BLINK_TIME);
	}

	/* led2 is off */	
	digitalWrite(LED_PIN_2, LOW);
	for(ui8LoopCounter=0;ui8LoopCounter<BLINK_COUNT;ui8LoopCounter++){
		digitalWrite(LED_PIN_1, LOW);
		delay(BLINK_TIME);
		digitalWrite(LED_PIN_1, HIGH);
		delay(BLINK_TIME);
	}
}