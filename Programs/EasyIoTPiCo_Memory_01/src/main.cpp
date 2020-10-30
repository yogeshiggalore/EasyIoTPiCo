/*******************************************************************************
* Project Name: EasyIoTPiCo_Memory_01
*
* Version: 1.0
*
* Description:
* This project tests ESP8266 memory
* 
*
* Owner:
* Yogesh M Iggalore
********************************************************************************
* Copyright (2020-21) , EasyIoTPiCo
*******************************************************************************/
#include <Arduino.h>
#include <EEPROM.h>

#define SWITCH_PIN	12
#define LED1_PIN	14

uint8_t ui8LEDState=1;
uint8_t ui8MemoryAddress=0;

/* switch isr function */
void switch_isr(void);

void setup() {

	delay(1000);

	/* Initialize EEPROM with 512 byte size */
	EEPROM.begin(512);   

	ui8LEDState = EEPROM.read(ui8MemoryAddress);
	if(ui8LEDState > 1){
		ui8LEDState = 1;
	}

	/* set LED1 pin output */
	pinMode(LED1_PIN, OUTPUT);
	digitalWrite(LED1_PIN, ui8LEDState);

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
		EEPROM.write(ui8MemoryAddress, ui8LEDState);
		EEPROM.commit();
	}else{
		digitalWrite(LED1_PIN, HIGH);
		ui8LEDState = 1;
		EEPROM.write(ui8MemoryAddress, ui8LEDState);
		EEPROM.commit();
	}
}

void loop() {
	/* empty loop */
}

