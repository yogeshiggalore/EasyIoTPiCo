/*******************************************************************************
* Project Name: EasyIoTPiCo_LED_01
*
* Version: 1.0
*
* Description:
* In this project EasyIoTPiCo, switches led on/off using firmware delay. LED pins are D6 and D7
* Owner:
* Yogesh M Iggalore
*
********************************************************************************
* Copyright (2020-21) , EasyIoTPiCo
*******************************************************************************/

/* include header files */
#include <Arduino.h>

#define LED1_PIN 2

void setup(){
	/* set LED pin to output*/
	pinMode(LED1_PIN, OUTPUT);
}

void loop(){
	
	/* turn on LED pin */
	digitalWrite(LED1_PIN,HIGH);

	/* wait for 500 milisecond */
	delay(500);

	/* turn off LED pin */
	digitalWrite(LED1_PIN,LOW);

	/* wait for 500 milisecond */
	delay(500);

}