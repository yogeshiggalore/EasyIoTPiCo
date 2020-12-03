/*******************************************************************************
* Project Name: EasyIoTPiCo_Mqtt_01
*
* Version: 1.0
*
* Description:
* In this project ESP8266 handles Mqtt connections
*
* Written By:
* Yogesh M Iggalore
*
********************************************************************************
* Copyright (2019-20) , EasyIoTPiCo
*******************************************************************************/

/* include wifi library for esp8266 module */
#include <ESP8266WiFi.h>

/* dht11 library */
#include <DHT.h>

/* include mqtt asynchronous client */
#include <AsyncMqttClient.h>

/* include ticker module */
#include <Ticker.h>

/* define dht22 pins */
#define DHT11_PIN 14

/* define type as DHT11 */
#define DHTTYPE DHT11

/* wifi ssid and password */
#define WIFI_SSID "IOTDev"
#define WIFI_PASSWORD "IOTDev1234"

/* mqtt server/host */
#define MQTT_HOST "silicosmos.in"

/* mqtt port secure communication use 8883 */
#define MQTT_PORT 1883

/* mqtt credentails for connection username and password */
#define MQTT_USERNAME "yogesh"
#define MQTT_PASSWORD "yogesh"

/* keep alive in second decides how long server should keep client connection */
#define MQTT_KEEPALIVE 90

/* mqtt Quality of Service 
	0 : At most once
	1 : At least once
	2 : Exactly once 
	*/

#define MQTT_QOS 2

/* retain flag if client disconnect from server */
#define MQTT_RETAIN 1

/* onboard led pin number */
#define LED_PIN 2

/* create dht11 intence */
DHT dht(DHT11_PIN, DHTTYPE);

/* DHT11 veriables */
float fHumidity;
float fTemperature;
float fHeatIndex;

/* cleintid should be unique */
String sClientId;

/* will messege on client disconnection and connection to other client */
String sMqttWillTopic;

/* mqtt topic for demo */
String sMqttTemperatureTopic;
String sMqttHumidityTopic;
String sMqttLedTopic;

uint16_t ui16TempPacketId;
uint16_t ui16HumidityPacketId;
uint16_t ui16LedPacketId;
uint8_t ui8MqttStatus = 0;

char buff[10];

/* mqtt client instance */
AsyncMqttClient mqttClient;

/* timer handling for mqtt and wifi reconnection */
WiFiEventHandler wifiConnectHandler;
WiFiEventHandler wifiDisconnectHandler;
Ticker wifiReconnectTimer;
Ticker mqttReconnectTimer;

/* timer interval for reading temperature and humidity sensor */
Ticker ReadPublishIntervalTimer;

/* function to read dht11 */
uint8_t Read_DHT11(void);
void Connect_To_Wifi(void);
void Read_Publish_Temp_Humidity(void);
void Connect_To_Mqtt(void);
void WiFiEvent(WiFiEvent_t event);
void On_Mqtt_Connect(bool sessionPresent);
void On_Mqtt_Disconnect(AsyncMqttClientDisconnectReason reason);
void On_Mqtt_Subscribe(uint16_t packetId, uint8_t qos);
void On_Mqtt_Unsubscribe(uint16_t packetId);
void On_Mqtt_Message(char *topic, char *payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total);
void On_Mqtt_Publish(uint16_t packetId);
void onWifiConnect(const WiFiEventStationModeGotIP &event);
void onWifiDisconnect(const WiFiEventStationModeDisconnected &event);

/****************************************************************************** 
	* Function Name: setup
	*******************************************************************************
	*
	* Summary:
	*  This function calls on start of program
	*  
	*
	*  
	*
	* Parameters:  
	*  None
	*
	* Return: 
	*  None
	*  
	*******************************************************************************/
void setup()
{

	/* set serial baudrate to 115200 and start serial port */
	Serial.begin(115200);
	Serial.println();
	Serial.println();
	Serial.println("MQTT demo");

	/* start dht11 sensor */
	dht.begin();

	delay(1000);

	Read_DHT11();

	wifiConnectHandler = WiFi.onStationModeGotIP(onWifiConnect);
	wifiDisconnectHandler = WiFi.onStationModeDisconnected(onWifiDisconnect);

	/* set callback function on mqtt connect */
	mqttClient.onConnect(On_Mqtt_Connect);

	/* set callback function on mqtt disconnect */
	mqttClient.onDisconnect(On_Mqtt_Disconnect);

	/* set callback function on client subscribe to topic */
	mqttClient.onSubscribe(On_Mqtt_Subscribe);

	/* set callback function on client unsubscribe to topic */
	mqttClient.onUnsubscribe(On_Mqtt_Unsubscribe);

	/* set callback function client on received mqtt messege */
	mqttClient.onMessage(On_Mqtt_Message);

	/* set callback function on client publish topic */
	mqttClient.onPublish(On_Mqtt_Publish);

	/* connect to wifi */
	Connect_To_Wifi();

	/* set onbaord led pin as output */
	pinMode(LED_PIN, OUTPUT);

	/* publish evry 10 seconds */
	ReadPublishIntervalTimer.attach(10, Read_Publish_Temp_Humidity);
}

void loop()
{
	/* read DHT11 */
	Read_DHT11();

	/* delay for 5 seconds */
	delay(5000);
}

/****************************************************************************** 
	* Function Name: Connect_To_Wifi
	*******************************************************************************
	*
	* Summary:
	*  This function connects esp32 module to wifi with given ssid and password
	*  
	*
	*  
	*
	* Parameters:  
	*  None
	*
	* Return: 
	*  None
	*  
	*******************************************************************************/
void Connect_To_Wifi(void)
{
	Serial.println("Connecting to Wi-Fi...");

	/* this line begins the wifi connction */
	WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

	/* for client id we are using mac address */
	sClientId = WiFi.macAddress();

	/* will message topic  */
	sMqttWillTopic = WiFi.macAddress() + "/status";
	Serial.println(sMqttWillTopic);

	/* temperature read topic */
	sMqttTemperatureTopic = WiFi.macAddress() + "/temperature";
	Serial.println(sMqttTemperatureTopic);

	/* humidity sensor read topic */
	sMqttHumidityTopic = WiFi.macAddress() + "/humidity";
	Serial.println(sMqttHumidityTopic);

	/* led control topic */
	sMqttLedTopic = WiFi.macAddress() + "/led";
	Serial.println(sMqttLedTopic);
}

/****************************************************************************** 
	* Function Name: Read_Publish_Temp_Humidity
	*******************************************************************************
	*
	* Summary:
	*  This function reads temperature and humidity values and publish to server
	*  
	*
	*  
	*
	* Parameters:  
	*  None
	*
	* Return: 
	*  None
	*  
	*******************************************************************************/
void Read_Publish_Temp_Humidity(void)
{
	uint16_t ui16UnScb;
	static long time;

	Serial.println(millis() - time);
	time = millis();

	if (ui8MqttStatus)
	{
		/* publish temperature topic to server */
		snprintf(buff, sizeof(buff), "%f", fTemperature);
		ui16TempPacketId = mqttClient.publish(sMqttTemperatureTopic.c_str(), MQTT_QOS, MQTT_RETAIN, buff);
		Serial.printf("Publishing at QoS %d, packetId: %d\n", MQTT_QOS, ui16TempPacketId);

		/* publish hall sensor topic to server */
		snprintf(buff, sizeof(buff), "%f", fHumidity);
		ui16HumidityPacketId = mqttClient.publish(sMqttHumidityTopic.c_str(), MQTT_QOS, MQTT_RETAIN, buff);
		Serial.printf("Publishing at QoS %d, packetId: %d\n", MQTT_QOS, ui16HumidityPacketId);
	}
}

/****************************************************************************** 
	* Function Name: Connect_To_Mqtt
	*******************************************************************************
	*
	* Summary:
	*  This function connects esp32 module to wifi with given ssid and password
	*  
	*
	*  
	*
	* Parameters:  
	*  None
	*
	* Return: 
	*  None
	*  
	*******************************************************************************/
void Connect_To_Mqtt(void)
{
	Serial.println("Connecting to MQTT...");

	/* set mqtt client id */
	mqttClient.setClientId(sClientId.c_str());

	/* set username and password */
	mqttClient.setCredentials(MQTT_USERNAME, MQTT_PASSWORD);

	/* set server and port */
	mqttClient.setServer(MQTT_HOST, MQTT_PORT);

	/* set keepalive value */
	mqttClient.setKeepAlive(MQTT_KEEPALIVE);

	/* set will message */
	mqttClient.setWill(sMqttWillTopic.c_str(), MQTT_QOS, MQTT_RETAIN, "offline", strlen("offline"));

	/* connect to mqtt */
	mqttClient.connect();
}

/****************************************************************************** 
	* Function Name: WiFiEvent
	*******************************************************************************
	*
	* Summary:
	*  This function calls when any wifi event like connection and disconnection happens
	*  
	*
	*  
	*
	* Parameters:  
	*  None
	*
	* Return: 
	*  None
	*  
	*******************************************************************************/
void onWifiConnect(const WiFiEventStationModeGotIP &event)
{
	Serial.println("Connected to Wi-Fi.");
	Connect_To_Mqtt();
}

void onWifiDisconnect(const WiFiEventStationModeDisconnected &event)
{
	Serial.println("Disconnected from Wi-Fi.");
	mqttReconnectTimer.detach(); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
	wifiReconnectTimer.once(2, Connect_To_Wifi);
}

/****************************************************************************** 
	* Function Name: On_Mqtt_Connect
	*******************************************************************************
	*
	* Summary:
	*  This function calls when mqtt connects to server
	*  
	*
	*  
	*
	* Parameters:  
	*  None
	*
	* Return: 
	*  None
	*  
	*******************************************************************************/
void On_Mqtt_Connect(bool sessionPresent)
{
	uint16_t ui16LoopCounter = 0;

	Serial.println("Connected to MQTT.");

	/* print session */
	Serial.print("Session present: ");
	Serial.println(sessionPresent);

	/* publish temperature topic to server */
	snprintf(buff, sizeof(buff), "%f", fTemperature);
	ui16TempPacketId = mqttClient.publish(sMqttTemperatureTopic.c_str(), MQTT_QOS, MQTT_RETAIN, buff);
	Serial.printf("Publishing at QoS %d, packetId: %d\n", MQTT_QOS, ui16TempPacketId);

	/* publish hall sensor topic to server */
	snprintf(buff, sizeof(buff), "%f", fHumidity);
	ui16HumidityPacketId = mqttClient.publish(sMqttHumidityTopic.c_str(), MQTT_QOS, MQTT_RETAIN, buff);
	Serial.printf("Publishing at QoS %d, packetId: %d\n", 2, ui16HumidityPacketId);

	/* publish will topic to server */
	mqttClient.publish(sMqttWillTopic.c_str(), MQTT_QOS, MQTT_RETAIN, "online");
	Serial.printf("Publishing will topic\n");

	/* subscribe to led control packet */
	uint16_t ui16LedPacketId = mqttClient.subscribe(sMqttLedTopic.c_str(), MQTT_QOS);
	Serial.printf("Subscribing to led topic QoS:%d\n", ui16LedPacketId);

	ui8MqttStatus = 1;
}

/****************************************************************************** 
	* Function Name: On_Mqtt_Disconnect
	*******************************************************************************
	*
	* Summary:
	*  This function calls when mqtt disconnects to server
	*  
	*
	*  
	*
	* Parameters:  
	*  None
	*
	* Return: 
	*  None
	*  
	*******************************************************************************/
void On_Mqtt_Disconnect(AsyncMqttClientDisconnectReason reason)
{
	Serial.println("Disconnected from MQTT.");

	/* if wifi is connected then reconnect to mqtt */
	if (WiFi.isConnected())
	{
		mqttReconnectTimer.once(2, Connect_To_Mqtt);
	}

	ui8MqttStatus = 0;
}

/****************************************************************************** 
	* Function Name: On_Mqtt_Subscribe
	*******************************************************************************
	*
	* Summary:
	*  This function calls when client subscribe to topic
	*  
	*
	*  
	*
	* Parameters:  
	*  None
	*
	* Return: 
	*  None
	*  
	*******************************************************************************/
void On_Mqtt_Subscribe(uint16_t packetId, uint8_t qos)
{
	uint16_t ui16LoopCounter = 0;

	Serial.print("Mqtt subscribe pkt: ");
	Serial.println(" ");

	Serial.println("Subscribe acknowledged.");
	Serial.print("packetId: ");
	Serial.println(packetId);
	Serial.print("qos: ");
	Serial.println(qos);
}

/****************************************************************************** 
	* Function Name: On_Mqtt_Unsubscribe
	*******************************************************************************
	*
	* Summary:
	*  This function calls when client unsubscribe to topic
	*  
	*
	*  
	*
	* Parameters:  
	*  None
	*
	* Return: 
	*  None
	*  
	*******************************************************************************/
void On_Mqtt_Unsubscribe(uint16_t packetId)
{
	uint16_t ui16LoopCounter = 0;

	Serial.println("Unsubscribe acknowledged.");
	Serial.print("packetId: ");
	Serial.println(packetId);
}

/****************************************************************************** 
	* Function Name: On_Mqtt_Message
	*******************************************************************************
	*
	* Summary:
	*  This function calls when client received any message from server
	*  
	*
	*  
	*
	* Parameters:  
	*  None
	*
	* Return: 
	*  None
	*  
	*******************************************************************************/
void On_Mqtt_Message(char *topic, char *payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total)
{

	Serial.println("Publish received.");
	Serial.print("  topic: ");
	Serial.println(topic);
	Serial.print("  qos: ");
	Serial.println(properties.qos);
	Serial.print("  dup: ");
	Serial.println(properties.dup);
	Serial.print("  retain: ");
	Serial.println(properties.retain);
	Serial.print("  len: ");
	Serial.println(len);
	Serial.print("  index: ");
	Serial.println(index);
	Serial.print("  total: ");
	Serial.println(total);
	Serial.print("  payload: ");
	Serial.println(payload);

	/* check for topic received */
	if (strcmp(topic, sMqttLedTopic.c_str()) == 0)
	{
		/* if payload is "on" then switch on led else switch off led */
		if(String(payload).indexOf("on")>= 0)
		{
			digitalWrite(LED_PIN, LOW);
		}
		else
		{
			digitalWrite(LED_PIN, HIGH);
		}
	}
}

/****************************************************************************** 
	* Function Name: On_Mqtt_Publish
	*******************************************************************************
	*
	* Summary:
	*  This function calls on sucess of mqtt publish 
	*  
	*
	*  
	*
	* Parameters:  
	*  None
	*
	* Return: 
	*  None
	*  
	*******************************************************************************/
void On_Mqtt_Publish(uint16_t packetId)
{
	uint16_t ui16LoopCounter = 0;

	Serial.print("Mqtt publish pkt: ");
	Serial.println(" ");

	Serial.println("Publish acknowledged.");
	Serial.print("  packetId: ");
	Serial.println(packetId);
}

/* function to read dht11 sensor */
uint8_t Read_DHT11(void)
{

	uint8_t ui8Response = 0;

	/* read humidity */
	fHumidity = dht.readHumidity();

	/* read temperature */
	fTemperature = dht.readTemperature();

	/* check if data is valid */
	if (isnan(fHumidity) || isnan(fTemperature))
	{

		/* display read error in serial */
		fHeatIndex = 0;
		ui8Response = 0;
		Serial.println("DHT11 reading failure");
	}
	else
	{

		/* if data is valid read heat index */
		fHeatIndex = dht.computeHeatIndex(fHumidity, fTemperature, false);

		ui8Response = 1;

		/* display these values */
		Serial.printf("Temperature:%f degC Humidity:%f % Heat index:%f\n", fTemperature, fHumidity, fHeatIndex);
	}

	return ui8Response;
}