/*******************************************************************************
* Project Name: EasyIoTPiCo_LED_01
*
* Version: 1.0
*
* Description:
* In this project EasyIoTPiCo, switches leds on/off using firmware delay. LED pins are 2 and 14
* Owner:
* Yogesh M Iggalore
*
********************************************************************************
* Copyright (2020-21) , EasyIoTPiCo
*******************************************************************************/

/* include header files */
#include <Arduino.h>

#define LED_PIN_1 2
#define LED_PIN_2 14

void setup(){
	/* set LED pins to output*/
	pinMode(LED_PIN_1, OUTPUT);
	pinMode(LED_PIN_2, OUTPUT);
}

void loop(){
	
	/* turn on LED pins */
	digitalWrite(LED_PIN_1,LOW);
	digitalWrite(LED_PIN_2,HIGH);

	/* wait for 500 milisecond */
	delay(500);

	/* turn off LED pin */
	digitalWrite(LED_PIN_1,HIGH);
	digitalWrite(LED_PIN_2,LOW);

	/* wait for 500 milisecond */
	delay(500);

}