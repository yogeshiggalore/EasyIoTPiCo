/*******************************************************************************
* Project Name: EasyIoTPiCo_PWM_02
*
* Version: 1.0
*
* Description:
* In this project EasyIoTPiCo, will controls leds with pulse width modulation 
* Owner:
* Yogesh M Iggalore
*
********************************************************************************
* Copyright (2020-21) , EasyIoTPiCo
*******************************************************************************/

#include <Arduino.h>

#define LED1_PIN	2
#define LED2_PIN 	14

void setup() {

}

void loop() {


	// increase the LED brightness
	for(int dutyCycle = 0; dutyCycle < 1023; dutyCycle++){   
    	// changing the LED brightness with PWM
    	analogWrite(LED1_PIN, dutyCycle);
		analogWrite(LED2_PIN, 1023 - dutyCycle);
    	delay(1);
 	}

 	// decrease the LED brightness
  	for(int dutyCycle = 1023; dutyCycle > 0; dutyCycle--){
    	// changing the LED brightness with PWM
    	analogWrite(LED1_PIN, dutyCycle);
		analogWrite(LED2_PIN, 1023 - dutyCycle);
    	delay(1);
  	}
}