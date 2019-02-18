#include <Arduino.h>

#include "UIPEthernet.h"
#include "PubSubClient.h"

#define CLIENT_ID       "UnoMQTT"
#define INTERVAL        3000 // 3 sec delay between publishing
#define DHTPIN          3
#define DHTTYPE         DHT11

uint8_t mac[6] = {0x00,0x01,0x02,0x03,0x04,0x05};

EthernetClient ethClient;
PubSubClient mqttClient;

long previousMillis;

void setup() {
  // setup serial communication
  //Serial.begin(9600);
  // setup ethernet communication using DHCP
  if(Ethernet.begin(mac) == 0) {
    //Serial.println(F("Ethernet configuration using DHCP failed"));
    for(;;);
  }
  // setup mqtt client
  mqttClient.setClient(ethClient);
  mqttClient.setServer("192.168.0.2",1883);
  //mqttClient.setServer("192.168.1.xxx",1883); //for using local broker
  //mqttClient.setServer("broker.hivemq.com",1883);
  //Serial.println(F("MQTT client configured"));

  // setup DHT sensor
  previousMillis = millis();
}


void sendData() {
  if(mqttClient.connect(CLIENT_ID)) {
   mqttClient.publish("nano/millis", millis());
 }
}

void loop() {
  // check interval
  if(millis() - previousMillis > INTERVAL) {
    sendData();
    previousMillis = millis();
  }
  mqttClient.loop();
}