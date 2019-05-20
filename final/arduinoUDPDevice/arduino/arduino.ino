/*
  HC-SR04 Ping distance sensor]
  VCC to arduino 5v GND to arduino GND
  Echo to Arduino pin 13 Trig to Arduino pin 12
  Red POS to Arduino pin 11
  Green POS to Arduino pin 10
  560 ohm resistor to both LED NEG and GRD power rail
  More info at: http://goo.gl/kJ8Gl
  Original code improvements to the Ping sketch sourced from Trollmaker.com
  Some code and wiring inspired by http://en.wikiversity.org/wiki/User:Dstaub/robotcar
*/

#define trigPin 6
#define echoPin 7
#define led 1
#define led2 0

#include <SPI.h>
#include <WiFiNINA.h>
#include <WiFiUdp.h>


int status = WL_IDLE_STATUS;
#include "arduino_secrets.h"
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;            // your network key Index number (needed only for WEP)

unsigned int localPort = 5000;      // local port to listen on

char packetBuffer[255]; //buffer to hold incoming packet

WiFiUDP Udp;

#define trigPin 6
#define echoPin 7

#define led 1
#define led2 0

int ultraState = 1;
String ultrastate = "hi";

void setup() {

  Serial.begin (9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(led, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(2, OUTPUT); // Set buzzer - pin 9 as an output


  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }

  // attempt to connect to WiFi network:
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
  Serial.println("Connected to wifi");
  printWiFiStatus();

  Serial.print("Initializing WiFiUDP library and listening on port ");
  Serial.println(localPort);
  Udp.begin(localPort);

  digitalWrite(2, LOW);
}

void loop() {

  // if there's data available, read a packet
  int packetSize = Udp.parsePacket();
  Serial.print(packetSize);
  if (packetSize)
  {
    Serial.print("Received packet of size ");
    Serial.print(packetSize);
    Serial.print(" from address ");
    IPAddress remoteIp = Udp.remoteIP();
    Serial.print(remoteIp);
    Serial.print(", port ");
    Serial.println(Udp.remotePort());

    // read the packet into packetBufffer
    int len = Udp.read(packetBuffer, 255);

    // Activate the actuators as requested
    digitalWrite(
      (int)packetBuffer[0],  // first byte is actuator number
      (int)packetBuffer[1]);            // second byte is value
  }

  // IP address of the receiving device
  //  IPAddress receivingDeviceAddress(192, 168, 1, 21);
  IPAddress receivingDeviceAddress(192, 168, 1, 11);
  unsigned int receivingDevicePort = 2390;

  long duration, distance;
  digitalWrite(trigPin, LOW);  // Added this line
  delayMicroseconds(2); // Added this line
  digitalWrite(trigPin, HIGH);
  //  delayMicroseconds(1000); - Removed this line
  delayMicroseconds(10); // Added this line
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration / 2) / 29.1;
  if (distance < 4 && ultraState == 1) {  // This is where the LED On/Off happens

    ultraState = 2;
    digitalWrite(led, HIGH); // When the Red condition is met, the Green LED should turn off
    digitalWrite(led2, LOW);


    Serial.println("CLOSE ENOUGH!");
    Udp.beginPacket(receivingDeviceAddress, receivingDevicePort);
    Udp.write("hello");
    Udp.endPacket();
  }

  else {
    digitalWrite(led, LOW);
    digitalWrite(led2, HIGH);
    ultraState = 1;
  }

  if (distance >= 200 || distance <= 0) {
    Serial.println("Out of range");
  }
  else {
    Serial.print(distance);
    Serial.println(" cm");
  }
  delay(500);
  // once we send a packet to the server, it might
  // respond, so read it

  
//
//    digitalWrite(2, HIGH); // Send 1KHz sound signal...
//  delay(1000);        // ...for 1 sec
//  digitalWrite(2, LOW);    // Stop sound...
//  delay(1000);        // ...for 1sec

}

void printWiFiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("My IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
