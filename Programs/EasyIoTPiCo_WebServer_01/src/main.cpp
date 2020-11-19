/*******************************************************************************
* Project Name: EasyIoTPiCo_WebServer_01
*
*
* Description:
* This project controls led on/off using station mode web page
* 
*
* Owner:
* Yogesh M Iggalore
********************************************************************************
* Copyright (2020-21) , EasyIoTPiCo
*******************************************************************************/

#include <Arduino.h>
#include <ESP8266WiFi.h>

/* Wifi credentials */
#define WIFI_SSID		"IOTDev"
#define WIFI_PASSWORD	"IOTDev1234"

/* Set web server port number to 80 */
WiFiServer server(80);

/* Variable to store the HTTP request */
String header;

/* Auxiliar variables to store the current output state */
String sOutput2State = "off";

/* Assign output variables to GPIO pins */
uint8_t ui8OutputPin2 = 2;

/* Current time */

unsigned long currentTime = millis();

/* Previous time */
unsigned long previousTime = 0; 

/* Define timeout time in milliseconds (example: 2000ms = 2s) */
const long timeoutTime = 2000;

void setup() {
	/* set UART1 baudrate to 9600 */
	Serial.begin(9600);
	
	/* Initialize the pins to output */
	pinMode(ui8OutputPin2, OUTPUT);
	
	/* Set outputs to LOW at init */
	digitalWrite(ui8OutputPin2, HIGH);
	
	/* Connect to Wi-Fi network with SSID and password */
	Serial.print("Connecting to ");
	Serial.println(WIFI_SSID);
	WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
	}
	
	/* Print local IP address and start web server */
	Serial.println("");
	Serial.println("WiFi connected.");
	Serial.println("IP address: ");
	Serial.println(WiFi.localIP());
	server.begin();
}

void loop(){
	/* Listen for incoming clients */
	WiFiClient client = server.available();
	
	/* if any new connection */
	if (client) {
		Serial.println("New Client.");
		String currentLine = "";
		currentTime = millis();
		previousTime = currentTime;
		while (client.connected() && currentTime - previousTime <= timeoutTime) {
			currentTime = millis();
			if (client.available()) {
				char c = client.read();
				Serial.write(c);
				header += c;
				if (c == '\n') {
					if (currentLine.length() == 0) {
						client.println("HTTP/1.1 200 OK");
						client.println("Content-type:text/html");
						client.println("Connection: close");
						client.println();
						if (header.indexOf("GET /2/on") >= 0) {
							Serial.println("GPIO 2 on");
							sOutput2State = "on";
							digitalWrite(ui8OutputPin2, LOW);
						} else{
							Serial.println("GPIO 2 off");
							sOutput2State = "off";
							digitalWrite(ui8OutputPin2, HIGH);
						}
						/* Display the HTML web page */
						client.println("<!DOCTYPE html><html>");
						client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
						client.println("<link rel=\"icon\" href=\"data:,\">");
            			client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            			client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
            			client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            			client.println(".button2 {background-color: #77878A;}</style></head>");
            
            			/* Web Page Heading */
            			client.println("<body><h1>ESP8266 Web Server</h1>");
            
	            		// Display current state, and ON/OFF buttons for GPIO 2  
      	      			client.println("<p>GPIO 2 - State " + sOutput2State + "</p>");
            			// If the output2State is off, it displays the ON button       
            			if (sOutput2State=="off") {
              				client.println("<p><a href=\"/2/on\"><button class=\"button\">ON</button></a></p>");
            		} else {
              			client.println("<p><a href=\"/2/off\"><button class=\"button button2\">OFF</button></a></p>");
            		}
            		client.println("</body></html>");
            
            		// The HTTP response ends with another blank line
            		client.println();
            		// Break out of the while loop
            		break;
          		} else { // if you got a newline, then clear currentLine
            		currentLine = "";
          		}
        	} else if (c != '\r') {  // if you got anything else but a carriage return character,
        		currentLine += c;      // add it to the end of the currentLine
    		}
    	}
	}
    	// Clear the header variable
    	header = "";
    	// Close the connection
    	client.stop();
    	Serial.println("Client disconnected.");
    	Serial.println("");
	}
}