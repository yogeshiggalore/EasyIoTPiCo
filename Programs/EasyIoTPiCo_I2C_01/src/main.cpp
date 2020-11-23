/*******************************************************************************
* Project Name: EasyIoTPiCo_I2C_01
*
* Version: 1.0
*
* Description:
* This project tests I2C oled module
* 
*
* Owner:
* Yogesh M Iggalore
********************************************************************************
* Copyright (2020-21) , EasyIoTPiCo
*******************************************************************************/
#include<Arduino.h>

/* I2C display header */
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>

#include <ESPRead.h>

/* OLED display width, in pixels */
#define SCREEN_WIDTH 128 

/* OLED display height, in pixels */
#define SCREEN_HEIGHT 64 

/* Declaration for an SSD1306 display connected to I2C (SDA, SCL pins) */
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {

	delay(2000);

 	Serial.begin(921600);

	Serial.println("\nEasyIoTPiCo_I2C_01");
	
	if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    	Serial.println(F("SSD1306 allocation failed"));
    	for(;;);
 	}

	/* start esp read */
	ERead.Start();

  	delay(2000);
  	display.clearDisplay();
  	display.setTextColor(WHITE);
	display.setTextSize(3);
	display.setCursor(0,0);
	display.println("EasyIoT");
	display.setCursor(0,30);
	display.setTextSize(4);
	display.println("PiCo");
	display.display();

	delay(2000);
}

void loop() {
	ERead.Read_DHT11();
	display.clearDisplay();
  	display.setTextColor(WHITE);
	display.setTextSize(3);
	display.setCursor(0,0);
	display.print("T:");
	display.println(ERead.tdfParameters.fTemperature);
	display.setCursor(0,30);
	display.setTextSize(3);
	display.print("H:");
	display.println(ERead.tdfParameters.fHumidity);
	display.display();
	delay(2000);
}