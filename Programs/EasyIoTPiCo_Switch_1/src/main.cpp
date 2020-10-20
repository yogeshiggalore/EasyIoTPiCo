/*******************************************************************************
* Project Name: EasyIoTPiCoSwitch1
*
* Version: 1.0
*
* Description:
* This project tests switch input
* 
*
* Owner:
* Yogesh M Iggalore
********************************************************************************
* Copyright (2020-21) , EasyIoTPiCo
*******************************************************************************/

#include <Arduino.h>

/* define LED1 and switch pins */
#define LED1_PIN		2
#define SWITCH_PIN		D6

void setup(){

	/* set LED1 pin as output */
	pinMode(LED1_PIN, OUTPUT);

	/* set switch pin as input */
	pinMode(SWITCH_PIN, INPUT);
}

void loop() {

	/* read switch input and control led */
	if(digitalRead(SWITCH_PIN)){
		/* turn on LED */
		digitalWrite(LED1_PIN, HIGH);
	}else{
		/* turn off LED */
		digitalWrite(LED1_PIN, LOW);
	}
}