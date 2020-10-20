/*******************************************************************************
* Project Name: EasyIoTPiCoV1Test
*
* Version: 1.0
*
* Description:
* This project tests all components/modules present in PiCoV1 board
* 
*
* Owner:
* Yogesh M Iggalore
********************************************************************************
* Copyright (2020-21) , EasyIoTPiCo
*******************************************************************************/
#include<Arduino.h>

/* ESPRead header */
#include<ESPRead.h>

/* I2C display header */
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>


//#define 
/* timer and intruppt function for 1 milisecond */
void Timer_1ms_Start(void);
void ICACHE_RAM_ATTR timercallback1ms(void);

/* OLED display width, in pixels */
#define SCREEN_WIDTH 128 

/* OLED display height, in pixels */
#define SCREEN_HEIGHT 64 

/* Declaration for an SSD1306 display connected to I2C (SDA, SCL pins) */
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

bool bmsFlag=0;
uint64_t ui64OnemsTimeCounter=0;
uint16_t ui16OneSecondCounter=0;

void OneSecond_Test(void);

void setup() {
	Serial1.begin(921600);
    Serial.begin(921600);

	Serial.println("\nEasyIoT PiCo");
	
	/* Start ESP Read */
	ERead.Start();

	//Wire.begin(D2,D1);

	if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    	Serial.println(F("SSD1306 allocation failed"));
    	for(;;);
 	}

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

	//pinMode(14,OUTPUT);

	/* start timer module */
	noInterrupts();
    Timer_1ms_Start();
    interrupts();

	delay(10000);
	//pinMode(D5,OUTPUT);
	//digitalWrite(D5,LOW);
}

void loop() {
	//ERead.Update();
	//digitalWrite(14,HIGH);
	//delay(1000);
	//Serial.println("Testing");
	//digitalWrite(14,LOW);
	//delay(1000);
	ERead.Read_DHT22();
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
	delay(1000);
	/*digitalWrite(D5,LOW);
	delay(1000);
	digitalWrite(D5,HIGH);
	delay(1000);*/
}

/* This function sets 1 milisecond timer */
void Timer_1ms_Start(void){
    timer1_isr_init();
    timer1_attachInterrupt(timercallback1ms);
    timer1_enable(TIM_DIV1, TIM_EDGE, TIM_LOOP); //80MHZ
    timer1_write(80000);//1ms
}

/* callback function 1ms interrupt */
void ICACHE_RAM_ATTR timercallback1ms(void){
    bmsFlag=1;
    ui64OnemsTimeCounter++;
	ERead.tdfParameters.ui16ESPReadCounter++;
    ui16OneSecondCounter++;
}

void OneSecond_Test(void){
    if(ui16OneSecondCounter > 1000){
        ui16OneSecondCounter = 0;
    }
}
