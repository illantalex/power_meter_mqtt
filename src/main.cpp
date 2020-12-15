#include <Ethernet.h>      // A library to work with Ethernet shield or ENC28J60
#include <PZEM004Tv30.h>   // A library to work with PZEM004T power meter
#include <PubSubClient.h>  // MQTT client for Adruino

#define MQTT_PORT 1883  // A port where MQTT broker works in LAN

IPAddress server(192, 168, 1, 102);                     // An address of MQTT broker in local network
uint8_t mac[6] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05};  // Manually set MAC address of Ethernet
char myName[] = "TestArduinoClient";                    // Name of device from which we send data

EthernetClient ethClient;
PubSubClient client(ethClient);  // Inintialized instance of MQTT client
PZEM004Tv30 pzem(5, 6);          // Initialized RX and TX pins for power meter

char res[10] = "";  // Temporary variable for result of floatToChar() function

// A function to convert float to char* types to send through Ethernet
char* floatToChar(float num) {
  float divider = 1000.0;         // A divider which becomes 10.0 times smaller every iteration to detach every digit
  bool flagFirst = true;          // A flag to check the first non-zero digit of the number or 0 if it is 0.[other digits]
  uint8_t i = 0;                  // Iteration index
  while (divider > 0.0001) {      // The number will be converted up to 3 digits after point
    float digit = num / divider;  // A digit we got after dividing
    int digitInt = digit;
    if (digitInt != 0 || divider == 1) flagFirst = false;  // Check if it's the first non-zero digit

    if (!flagFirst) {
      res[i] = digitInt + '0';  // Fill an array by the digits
      ++i;
    }
    if (divider == 1) {
      res[i] = '.';  // Add a point if it's the first number before point
      ++i;
    }
    num -= digitInt * divider;  // Substract a digit from a number
    divider /= 10.0;            // Divide the divider by 10.0
  }
  return res;
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(myName)) {
      // Serial.println("connected");

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
  Serial.begin(9600);  // Serial communication for debugging

  while (!Ethernet.begin(mac)) {  // Start DHCP Ethernet with MAC address
    Serial.println("Ethernet failed, waiting 5 seconds to reconnect.");
    delay(5000);
  }
  Serial.print("localIP: ");
  Serial.println(Ethernet.localIP());
  Serial.print("subnetMask: ");
  Serial.println(Ethernet.subnetMask());
  Serial.print("gatewayIP: ");
  Serial.println(Ethernet.gatewayIP());
  Serial.print("dnsServerIP: ");
  Serial.println(Ethernet.dnsServerIP());

  // Set an address of the MQTT server where we'll send data and wait 1.5 seconds to initialize
  client.setServer(server, MQTT_PORT);
  delay(1500);
}

void loop() {
  if (!client.connected()) {  // reconnect if not connected
    reconnect();
  }

  float current = pzem.current();
  if (current > 0) {                 // if current is higher than zero, then we send it to server
    float voltage = pzem.voltage();  // read voltage from power meter
    char data[32] = "{";             // JSON which we'll send to server
    // Continue building JSON with data
    strcat(data, "\"current\":");
    strcat(data, floatToChar(current));
    strcat(data, ",\"voltage\":");
    strcat(data, floatToChar(voltage));
    strcat(data, "}");
    Serial.println(data);
    client.publish("sendData", data);  // send JSON to server to the topic "sendData"
  }
  client.loop();  // call this method to stay connected
  delay(5000);    // delay 5 seconds between measurements
}
