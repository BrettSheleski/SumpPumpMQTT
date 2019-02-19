#include <Arduino.h>
#include <UIPEthernet.h> // Used for Ethernet
#include <PubSubClient.h>
#include <Bounce2.h>

#define UPDATE_FREQUENCY          1000

#define MQTT_CLIENT_ID            "5d4d1db9-b6ef-4def-998d-d16e6f6ec103"
#define MQTT_TOPIC                "sumppump"
#define MQTT_SERVER               "192.168.0.2"
#define MQTT_PORT                 1883
#define MQTT_TELE_FREQUENCY       3000

#define FLOAT_SWITCH_PIN          2
#define FLOAT_SWTICH_OPEN_STATE   HIGH
#define FLOAT_SWTICH_CLOSE_STATE  LOW



// **** ETHERNET SETTING ****
byte mac[] = { 0x90, 0xA2, 0xDA, 0x0D, 0x78, 0xEE  };                                       



Bounce debouncer = Bounce();
EthernetClient ethClient;
PubSubClient mqttClient;

//IPAddress ip(192, 168, 1, 115);                        
EthernetServer server(80);

unsigned long lastUpdate;
unsigned long lastFloatSwitchState;

void setup() {
  Serial.begin(115200);
  Serial.println("Starting Up!");

  Serial.println("Setting up Ethernet...");
  // start the Ethernet connection and the server:
  Ethernet.begin(mac);
  server.begin();

  Serial.print("IP Address: ");
  Serial.println(Ethernet.localIP());

  mqttClient.setClient(ethClient);
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  
  //pinMode(FLOAT_SWITCH_PIN, INPUT_PULLUP);
  debouncer.attach(FLOAT_SWITCH_PIN, INPUT_PULLUP);
  debouncer.interval(25);

  lastFloatSwitchState = digitalRead(FLOAT_SWITCH_PIN);
  lastUpdate = millis();

  Serial.println("Done starting up.");
}

void publishMqttFloatSwitch(){
  publishMqttFloatSwitch(currentPinState == FLOAT_SWTICH_CLOSE_STATE);
}

void publishMqttFloatSwitch(bool isClosed){
  if(mqttClient.connect(MQTT_CLIENT_ID)) {
    bool retain = true;
    mqttClient.publish("tele/sumppump/switch", isClosed ? "CLOSED" : "OPEN", retain);
  }
}

void loop() {
  unsigned long now = millis(),
    currentPinState = digitalRead(FLOAT_SWITCH_PIN);

  debouncer.update();

  if (debouncer.fell()){
    lastFloatSwitchState = LOW;
    publishMqttFloatSwitch();
  }
  else if (debouncer.rose()){
    lastFloatSwitchState = HIGH;
    publishMqttFloatSwitch();
  }
  
  if ((now - lastUpdate) > UPDATE_FREQUENCY){
    lastUpdate = now;

    publishMqttFloatSwitch(lastFloatSwitchState == FLOAT_SWTICH_CLOSE_STATE);
  }
}