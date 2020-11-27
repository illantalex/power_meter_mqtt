#include <Arduino.h>
#include <ArduinoJson.h>
#include <Ethernet.h>
#include <PZEM004Tv30.h>
#include <PubSubClient.h>
#include <SPI.h>

IPAddress server(192, 168, 1, 102);
uint8_t mac[6] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05};
char myName[] = "TestArduinoClient";
#define MQTT_PORT 1883

// char payloadGetTime[] = "";
// uint8_t payloadGetTimeLength = 0;
// uint32_t timeInLinuxFormat = 0;

EthernetClient ethClient;
PubSubClient client(ethClient);
PZEM004Tv30 pzem(5, 6);

// uint32_t getTime() {
//   return timeInLinuxFormat + millis() / 1000;
// }

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (uint8_t i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    // payloadGetTime[i] = (char)payload[i];
  }
  // Serial.println();
  // Serial.println(payloadGetTime);
  // payloadGetTimeLength = length;
}

// void getTimeFromServer() {
//   client.subscribe("getTime");
//   while (!*payloadGetTime) {
//     client.loop();
//   }
//   // Serial.println(payloadGetTime);
//   Serial.print("Payload: ");
//   for (uint8_t i = 0; i < payloadGetTimeLength; i++) {
//     Serial.print(payloadGetTime[i]);
//     timeInLinuxFormat *= 10;
//     timeInLinuxFormat += payloadGetTime[i] - '0';
//   }
//   Serial.print("time in linux format: ");
//   Serial.println(timeInLinuxFormat);
//   client.unsubscribe("getTime");
// }

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(myName)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      // ... and resubscribe
      // client.publish("test", "message");
      // client.subscribe("getTime");
      client.publish("sendData", "{name:\"name\"}");

    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(9600);

  if (!Ethernet.begin(mac)) {
    Serial.println("Ethernet failed.");
    return;
  } else {
    Serial.print("localIP: ");
    Serial.println(Ethernet.localIP());
    Serial.print("subnetMask: ");
    Serial.println(Ethernet.subnetMask());
    Serial.print("gatewayIP: ");
    Serial.println(Ethernet.gatewayIP());
    Serial.print("dnsServerIP: ");
    Serial.println(Ethernet.dnsServerIP());
  }

  // client.setServer(broadcast(Ethernet.localIP(), Ethernet.subnetMask()), 1883);
  client.setServer(server, MQTT_PORT);

  client.setCallback(callback);

  delay(1500);
  // reconnect();
  // client.loop();
  // int mill1 = millis();
  // getTimeFromServer();
  // int mill2 = millis();
  // timeInLinuxFormat -= (mill1 + mill2) / 2000;
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  // Serial.println(getTime());
  // client.loop();
  // DynamicJsonDocument data(32);
  // data["name"] = myName;
  // data["time"] = getTime();
  // data["data"]["voltage"] = pzem.voltage();
  // data["data"]["current"] = pzem.current();
  // char output[32];
  // serializeJson(data, output);
  client.connect(myName);
  client.publish("sendData", "{name:\"name\"}");
  // client.loop();
  delay(5000);
}
