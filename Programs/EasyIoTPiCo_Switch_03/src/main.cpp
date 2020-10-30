/*******************************************************************************
* Project Name: EasyIoTPiCo_Switch_03
*
* Version: 1.0
*
* Description:
* This project controls led on/off on switch press
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

#define LOW		1
#define HIGH	0

uint8_t ui8SwitchPressed=false;
uint8_t ui8LEDState=0;

void setup(){

	/* set LED1 pin as output */
	pinMode(LED1_PIN, OUTPUT);

	/* turn off the led */
	digitalWrite(LED1_PIN, LOW);
	ui8LEDState = false;

	/* set switch pin as input */
	pinMode(SWITCH_PIN, INPUT_PULLUP);
}

void loop() {

	/* read switch input */
	while( digitalRead(SWITCH_PIN) == false){
		/* small delay */
		delay(1);
	`
	00
		ui8SwitchPressed = true;
	}
	
	if(ui8SwitchPressed){
		ui8SwitchPressed = false;
		if(ui8LEDState == false){
			digitalWrite(LED1_PIN, HIGH);
			ui8LEDState = true;
		}else{
			digitalWrite(LED1_PIN, LOW);
			ui8LEDState = false;
		}
	}
}
