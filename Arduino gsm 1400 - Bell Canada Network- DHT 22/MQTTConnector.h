//Uses mqtt library by Joel Geahwiler

#include <MQTT.h>


const char mqttServer[] = ""; // IP adress of IDO ThingsBoard Server (Broker) in the classroom
const int mqttServerPort = 1883; // broker mqtt port
const char key[] = "xxxxxxx"; // broker device key
const char secret[] = "xxxxxxx"; // broker secret (May not be needed depending on your server and it's setup so leave blank)
const char device[] = "xxxxxxxxxx"; // broker device identifier
