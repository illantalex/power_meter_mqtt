#include <Arduino.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <UIPEthernet.h>

EthernetClient ethClient;
PubSubClient client(ethClient);
signed long next;

IPAddress server(192, 168, 1, 103);
uint8_t mac[6] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05};

IPAddress broadcast(IPAddress ip, IPAddress maska) {
  IPAddress broad;
  for (int i = 0; i < 4; ++i) broad[i] = (ip[i] & maska[i]) + ~maska[i];
  return broad;
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("arduinoClient")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("test", "hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
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
  client.setServer(server, 1883);

  client.setCallback(callback);

  next = 0;
  delay(1500);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }

  client.loop();
}
