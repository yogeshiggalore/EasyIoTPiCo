/*******************************************************************************
* Project Name: EasyIoTPiCo_Switch_02
*
* Version: 1.0
*
* Description:
* In this project EasyIoTPiCo, interrupt is used in switch press
*
* Owner:
* Yogesh M Iggalore
*
********************************************************************************
* Copyright (2020-21) , EasyIoTPiCo
*******************************************************************************/

#include <Arduino.h>

#define SWITCH_PIN	12
#define LED1_PIN	2

uint8_t ui8LEDState=1;

/* switch isr function */
void switch_isr(void);

void setup() {

	delay(5000);

	/* set LED1 pin output */
	pinMode(LED1_PIN, OUTPUT);

	/* set switch pin as input */
	pinMode(SWITCH_PIN, INPUT);

	/* attach a intrrupt to it with callback function on falling edge */
	attachInterrupt(digitalPinToInterrupt(SWITCH_PIN), switch_isr, FALLING);
}

void ICACHE_RAM_ATTR switch_isr(void){

	/* read current status of led pin and write invert of it */
	if(ui8LEDState){
		digitalWrite(LED1_PIN, LOW);
		ui8LEDState = 0;
	}else{
		digitalWrite(LED1_PIN, HIGH);
		ui8LEDState = 1;
	}
}

void loop() {
	/* empty loop */
}

