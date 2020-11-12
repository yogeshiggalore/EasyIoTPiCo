/*******************************************************************************
* Project Name: EasyIoTPiCo_PWM_01
*
* Version: 1.0
*
* Description:
* In this project EasyIoTPiCo, will controls led with pulse width modulation 
* Owner:
* Yogesh M Iggalore
*
********************************************************************************
* Copyright (2020-21) , EasyIoTPiCo
*******************************************************************************/

#include <Arduino.h>

#define LED1_PIN	2

void setup() {

}

void loop() {
	// increase the LED brightness
	for(int dutyCycle = 0; dutyCycle < 1023; dutyCycle++){   
    	// changing the LED brightness with PWM
    	analogWrite(LED1_PIN, dutyCycle);a
    	delay(1);
 	}

 	// decrease the LED brightness
  	for(int dutyCycle = 1023; dutyCycle > 0; dutyCycle--){
    	// changing the LED brightness with PWM
    	analogWrite(LED1_PIN, dutyCycle);
    	delay(1);
  	}
}


