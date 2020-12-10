#include <Ethernet.h>
#include <PZEM004Tv30.h>
#include <PubSubClient.h>

#define MQTT_PORT 1883

IPAddress server(192, 168, 1, 102);
uint8_t mac[6] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05};
char myName[] = "TestArduinoClient";

EthernetClient ethClient;
PubSubClient client(ethClient);
PZEM004Tv30 pzem(5, 6);

char res[10] = "";

char* float_to_char(float num) {
  float div = 1000.0;
  bool flag_first = true;
  uint8_t i = 0;
  while (div > 0.0001) {
    float dig = num / div;
    int dig_int = dig;
    if (dig_int != 0 || div == 1) flag_first = false;

    if (!flag_first) {
      res[i] = dig_int + '0';
      ++i;
    }
    if (div == 1) {
      res[i] = '.';
      ++i;
    }
    num -= dig_int * div;
    div /= 10.0;
  }
  return res;
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    // Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(myName)) {
      // Serial.println("connected");

    } else {
      // Serial.print("failed, rc=");
      // Serial.print(client.state());
      // Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(9600);

  if (!Ethernet.begin(mac)) {
    // Serial.println("Ethernet failed.");
    return;
  } else {
    // Serial.print("localIP: ");
    // Serial.println(Ethernet.localIP());
    // Serial.print("subnetMask: ");
    // Serial.println(Ethernet.subnetMask());
    // Serial.print("gatewayIP: ");
    // Serial.println(Ethernet.gatewayIP());
    // Serial.print("dnsServerIP: ");
    // Serial.println(Ethernet.dnsServerIP());
  }

  client.setServer(server, MQTT_PORT);
  delay(1500);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }

  float current = pzem.current();
  if (current != 0) {
    float voltage = pzem.voltage();
    char data[32] = "{";

    strcat(data, "\"current\":");
    strcat(data, float_to_char(current));
    strcat(data, ",\"voltage\":");
    strcat(data, float_to_char(voltage));
    strcat(data, "}");
    Serial.println(data);
    client.publish("sendData", data);
  }
  client.loop();
  delay(5000);
}
