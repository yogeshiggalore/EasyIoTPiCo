#include <Arduino.h>

#define LED_PIN	2

void setup() {
	/* set led pin as output */
	pinMode(LED_PIN, OUTPUT);
}

void loop() {

	/* turn  on led */
	digitalWrite(LED_PIN, HIGH);

	/* wait for 100ms */
	delay(100);

	/* turn off led */
	digitalWrite(LED_PIN, LOW);

	/* wait for 100ms */
	delay(100);

}