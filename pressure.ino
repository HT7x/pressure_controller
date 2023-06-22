#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "secrets.h"
#include <ArduinoJson.h>


const int pin = A1;
char *DESTINATION = "steingart_lab/data/pressure/pressure/silicon_control";


WiFiClient espClient;
PubSubClient client(espClient); // lib required for mqtt;

void setup()
{
  // Setup Analog Output Pin
	Serial.begin(57600);
	pinMode(pin, OUTPUT);

	// Conncet to WiFi
	WiFi.begin(SECRET_WIFI, SECRET_PASS);
	while (WiFi.status() != WL_CONNECTED)
	{
		delay(500);
		Serial.print(".");
	}
	Serial.print("WiFi connected: ");
	Serial.println(WiFi.localIP());

	// Setup MQTT
	client.setServer(SECRET_MQTT, SECRET_PORT); // connecting to mqtt server
	client.setCallback(callback);
	reconnect();
	client.loop(); // check for incoming messages

	
}

void callback(char *topic, byte *payload, unsigned int length)
{
	for (int i = 0; i < length; i++)
	{
		Serial.print((char)payload[i]);
	}

	Serial.println();
}

void reconnect()
{
	while (!client.connected())
	{
		Serial.println("Connecting to MQTT broker...");
		if (client.connect(DESTINATION))
		{
			Serial.println("Connected to MQTT broker");
			client.subscribe(DESTINATION);
			Serial.print("Subscribed to topic: ");
		}
		else
		{
			Serial.print("Failed to connect to MQTT broker, rc=");
			Serial.print(client.state());
			Serial.println(" retrying in 5 seconds");
			delay(5000);
		}
	}
}

void loop()
{
	StaticJsonDocument<256> docout;

	if (!client.connected())
	{
		reconnect();
	}

  // Custome Code here
  for(int i = 0; i < 256; i+= 5) {
    analogWrite(pin,  i);
    delay(1000); 
    docout["bit"] = i;
    docout["current"] = 20 * (i / 255.0);

    // Sends data to MQTT
    char buffer[256];
    size_t n = serializeJson(docout, buffer);
    Serial.println(buffer);
    Serial.println(1);
    client.publish(DESTINATION, buffer, n);
  }

  
  for(int i = 255; i >= 0; i-= 5) {
    analogWrite(pin,  i);
    delay(1000); 
    docout["bit"] = i;
    docout["current"] = 20 * (i / 255.0);

    // Sends data to MQTT
    char buffer[256];
    size_t n = serializeJson(docout, buffer);
    Serial.println(buffer);
    Serial.println(1);
    client.publish(DESTINATION, buffer, n);
  }
	
}