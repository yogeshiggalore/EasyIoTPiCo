/*******************************************************************************
* Project Name: EasyIoTPiCo_Timer_02
*
* Version: 1.0
*
* Description:
* In this project EasyIoTPiCo, switches led on/off using timer. LED pins are 2 and 14
*
* Owner:
* Yogesh M Iggalore
*
********************************************************************************
* Copyright (2020-21) , EasyIoTPiCo
*******************************************************************************/

/* include header files */
#include <Arduino.h>

#define LED_PIN_1 2
#define LED_PIN_2 14

#define LED_1_BLINK_TIME	500
#define LED_2_BLINK_TIME	1000

#define LED_OFF	0
#define LED_ON	1

/* timer and intruppt function for 1 milisecond */
void Timer_1ms_Start(void);
void ICACHE_RAM_ATTR timercallback1ms(void);

/* LED update function */
void ICACHE_RAM_ATTR Update_LED_Status(void);

/* timer flags */
uint8_t  ui81msFlag=0;
uint64_t ui64OnemsTimeCounter=0;

uint16_t ui16LED1TimeCounter=0;
uint16_t ui16LED2TimeCounter=0;

uint8_t ui8LED1State=0;
uint8_t ui8LED2State=0;

void setup() {

	/* set LED pins to output*/
	pinMode(LED_PIN_1, OUTPUT);
	pinMode(LED_PIN_2, OUTPUT);

	/* set led initaial state to LOW */
	digitalWrite(LED_PIN_1,LOW);
	digitalWrite(LED_PIN_2,LOW);
	ui8LED1State = LED_OFF;
	ui8LED2State = LED_OFF;

	/* start timer module */
	noInterrupts();
    Timer_1ms_Start();
    interrupts();
}

void loop() {

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
    ui81msFlag=1;
    ui64OnemsTimeCounter++;
	
	/* call led update function */
	Update_LED_Status();
}

/* LEDs update functions */
void ICACHE_RAM_ATTR Update_LED_Status(void){
	ui16LED1TimeCounter++;
	ui16LED2TimeCounter++;

	/* Check if counter exceeds timer value set */
	if(ui16LED1TimeCounter > LED_1_BLINK_TIME){
		ui16LED1TimeCounter = 0;
		if(ui8LED1State == LED_ON){
			ui8LED1State = LED_OFF;
			digitalWrite(LED_PIN_1,LOW);
		}else{
			ui8LED1State = LED_ON;
			digitalWrite(LED_PIN_1,HIGH);
		}
	}

	/* Check if counter exceeds timer value set */
	if(ui16LED2TimeCounter > LED_2_BLINK_TIME){
		ui16LED2TimeCounter = 0;
		if(ui8LED2State == LED_ON){
			ui8LED2State = LED_OFF;
			digitalWrite(LED_PIN_2,LOW);
		}else{
			ui8LED2State = LED_ON;
			digitalWrite(LED_PIN_2,HIGH);
		}
	}
}
