/*******************************************************************************
* Project Name: EasyIoTPiCo_UART_01
*
* Version: 1.0
*
* Description:
* In this project EasyIoTPiCo, uses UART for communication
* Owner:
* Yogesh M Iggalore
*
********************************************************************************
* Copyright (2020-21) , EasyIoTPiCo
*******************************************************************************/

/* include header files */
#include <Arduino.h>

#define LED_PIN 2

String sCMDString = "Use following commands to control LED \nA: Turn on led\nB: Turn off led\nC: Get current led status";

void setup(){

	delay(1000);

	/* set LED pin to output*/
	pinMode(LED_PIN, OUTPUT);
	digitalWrite(LED_PIN, HIGH);

	/* Start UART module with baudrate 921600 */
	/*
		baudrate : 921600
		databit  : 8
		parity   : None
		stopbit  : 1
	*/
	Serial.begin(921600,SERIAL_8N1);

	/* print project name */
	Serial.println("\nEasyIoTPiCo_UART_01");

	/* print command string */
	Serial.println(sCMDString);
}

void loop(){
	char cData;
	/* check if any uart data available */
	if(Serial.available()){
		/* read the byte */
		cData = Serial.read();
		Serial.print("Command received and command is ");
		Serial.println(cData);
		if(cData == 'A'){
			digitalWrite(LED_PIN,LOW);
			Serial.println("Turning LED on");	
		}else if(cData == 'B'){
			digitalWrite(LED_PIN,HIGH);
			Serial.println("Turning LED off");	
		}else if(cData == 'C'){
			if(digitalRead(LED_PIN)){
				Serial.println("LED is off");	
			}else{
				Serial.println("LED is on");	
			}
		}else{
			Serial.println(" :( :( Wrong command :( :(");
			Serial.println(sCMDString);
		}
	}
}