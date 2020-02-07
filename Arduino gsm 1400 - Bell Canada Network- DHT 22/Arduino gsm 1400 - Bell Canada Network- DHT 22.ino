/*

 This uses an MKR GSM 1400 with a DHT22 to send temperature and humidity data to a thingsBoard server via MQTT.
 Warning:  Basic examples so use at your own risk
 Tip:  Run throught the gsm examples and tests provided by the arduino mkrgsm library

Circuit:
* MKR GSM 1400 board
* Antenna
* SIM card with a data plan
* DHT22

 adapted 12 Jan 2020
 by Andre Roussel
*/
#include <MKRGSM.h>

#include "GSMConnector.h"

#include "MQTTConnector.h";

#include "DHT.h"

#define DHTPIN 2
#define DHTTYPE DHT22  


// Please enter your sensitive data in the GSMConnector.h  and the MQTTConnector.h
// PIN Number
const char PINNUMBER[]     = SECRET_PINNUMBER;
// APN data
const char GPRS_APN[]      = SECRET_GPRS_APN;
const char GPRS_LOGIN[]    = SECRET_GPRS_LOGIN;
const char GPRS_PASSWORD[] = SECRET_GPRS_PASSWORD;

// initialize the library instance
GSMClient GSMclient;
GPRS gprs;
GSM gsmAccess;

MQTTClient mqqtClient;  // Create a MQTT client to handle messagin between object and broker
DHT dht(DHTPIN, DHTTYPE); //Define and create the correct DHT sensor instance
unsigned long lastMillis = 0;  //Used instead of delay to send data every 10 seconds

void setup() {
   // Initialize serial and wait for port to open:
   Serial.begin(9600);
   

  connect();

  dht.begin(); //Start communication with DHT22 sensor
 
 
}

void connect() {
  
  InitGSMConnection(); // Establish the WIFI connection (Infrastructure)
  InitMQTTConnection(); // Establish the MQTT connection (Messaging)

}

void InitMQTTConnection() {
  
  mqqtClient.begin(mqttServer, mqttServerPort, GSMclient);
  Serial.print("Connecting to broker...");
  
  while (!mqqtClient.connect(device, key, secret)) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\nConnected to MQTT!\n");

}

void InitGSMConnection() {

  Serial.print("Connecting to GSM...");
  
  // connection state
 bool connected = false;

 // After starting the modem with GSM.begin()
 // attach the shield to the GPRS network with the APN, login and password
 while (!connected) {
   if ((gsmAccess.begin(PINNUMBER) == GSM_READY) &&
       (gprs.attachGPRS(GPRS_APN, GPRS_LOGIN, GPRS_PASSWORD) == GPRS_READY)) {
     connected = true;
   } else {
     Serial.println("Not connected");
     delay(1000);
   }
 }
  
  Serial.println("\nConnected to GSM!\n");

}

void loop() {
 mqqtClient.loop(); //allow the client to process incoming messages and maintain its connection to the server.

  if (millis() - lastMillis > 1000) {
    lastMillis = millis();

    float  pumpHouse_Hum = dht.readHumidity();  // Get Humidity
    float pumpHouse_Temp = dht.readTemperature(); // Get Temperature
    
    String payload = prepareJSONstring(pumpHouse_Temp,pumpHouse_Hum); // Prepare message
    
    SendMessage(payload); // Send the message to broker
  
    lastMillis = millis();
  }
}

// Prepare a JSON payload string
String prepareJSONstring(float pumpHouse_Temp,float pumpHouse_Hum){


String payload = "{";
  payload += "\"pumpHouse_Temp\":"; 
  payload += pumpHouse_Temp; 
  payload += ",";
  payload += "\"pumpHouse_Hum\":"; 
  payload += pumpHouse_Hum;
  payload += "}";

 return payload;
}

void SendMessage(String payload){
  // Send payload
  char attributes[100];
  payload.toCharArray(attributes, 100);
  mqqtClient.publish("v1/devices/me/telemetry", attributes);
  Serial.println(attributes);
}
