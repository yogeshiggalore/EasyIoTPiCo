#include "mbed.h"
 
Serial      pc(PA_2, PA_3); // TX, RX
DigitalOut  myled(PC_13);   // on-board LED
  
int main() 
{  
    while(1) {
        // The on-board LED is connected via a resistor to +3.3V (not to GND). 
        // So the LED is active on 0
        myled = 0;      // turn the LED on
        wait_ms(100);   // wait 200 millisecond
        myled = 1;      // turn the LED off
        pc.printf("Blink\r\n");
        wait_ms(500);  // wait 1000 millisecond
    }
}