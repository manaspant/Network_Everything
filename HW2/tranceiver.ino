/*

  WiFi UDP Send Byte on Button Press
  Based on UDP Send and Receive String

  created 3 February 2019
  by Michael Shiloh
*/

#include <SPI.h>
#include <WiFiNINA.h>
#include <WiFiUdp.h>

int status = WL_IDLE_STATUS;
#include "arduino_secrets.h"
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;            // your network key Index number (needed only for WEP)



//unsigned int localPort = 5000;      // local port to listen on
unsigned int localPort = 2390;      // local port to listen on

// IP address of the receiving device
IPAddress receivingDeviceAddress(192, 168, 1, 16);
unsigned int receivingDevicePort = 2390;

char packetBuffer[255]; //buffer to hold incoming packet

WiFiUDP Udp;

const int LED_PIN1 = 4;
const int LED_PIN2 = 5;
const int LED_PIN3 = 0;

const int led1 = 1;
const int led2 = 2;
const int led3 = 3;

// remember the button state so we only send
// when the state changes
boolean buttonState1;
boolean lastButtonState1 = LOW; // arbitrary
boolean buttonState2;
boolean lastButtonState2 = LOW; // arbitrary
boolean buttonState3;
boolean lastButtonState3 = LOW; // arbitrary



void setup() {

//Initialize serial and wait for port to open :
  Serial.begin(9600);

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < "1.0.0") {
    Serial.println("Please upgrade the firmware");
  }


  //  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);


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
}

void loop() {



  buttonState1 = digitalRead(LED_PIN1);
  buttonState2 = digitalRead(LED_PIN2);
  buttonState3 = digitalRead(LED_PIN3);

  if (buttonState1 != lastButtonState1 || buttonState2 != lastButtonState2 || buttonState3 != lastButtonState3) {

    Serial.println("button state changed; sending new state");
    Udp.beginPacket(receivingDeviceAddress, receivingDevicePort);
    Udp.write(buttonState1);
    Udp.write(buttonState2);
    Udp.write(buttonState3);
    Udp.endPacket();

    lastButtonState1 = buttonState1;
    lastButtonState2 = buttonState2;
    lastButtonState3 = buttonState3;
  }

  // once we send a packet to the server, it might
  // respond, so read it

  // if there's data available, read a packet
  int packetSize = Udp.parsePacket();
  if (packetSize)
  {
    Serial.print("Received packet of size ");
    Serial.println(packetSize);
    Serial.print("From ");
    IPAddress remoteIp = Udp.remoteIP();
    Serial.print(remoteIp);
    Serial.print(", port ");
    Serial.println(Udp.remotePort());

    // read the packet into packetBufffer
    int len = Udp.read(packetBuffer, 255);
    if (len > 0) packetBuffer[len] = 0;
    Serial.println("Contents:");
    Serial.println(packetBuffer);


  }

  //Receiver

  // if nobody sent us anything, packetSize
  // would be zero, so a non-zero packetSize
  // means we have something to work on
  if (packetSize) {
    Serial.print("Received packet of size ");
    Serial.println(packetSize);
    Serial.print("From ");
    IPAddress remoteIp = Udp.remoteIP();
    Serial.print(remoteIp);
    Serial.print(", port ");
    Serial.println(Udp.remotePort());

    // read the packet into packetBufffer
    int len = Udp.read(packetBuffer, 255);

    // We are expecting exactly one byte,
    // and that byte tells us whether to turn
    // the LED on or off. There is no need to
    // inspect the byte, simply send it directly
    // to digitalWrite()
    int blueSwitchState = packetBuffer[0];
    int redSwitchState = packetBuffer[1];
    int greenSwitchState = packetBuffer[2];


    if (blueSwitchState) {
      digitalWrite(led1, HIGH);
      delay(300);
      digitalWrite(led2, HIGH);
      delay(300);
      digitalWrite(led3, HIGH);
      delay(300);
      digitalWrite(led1, LOW);
      delay(300);
      digitalWrite(led2, LOW);
      delay(300);
      digitalWrite(led3, LOW);


    }

    if (greenSwitchState) {
      digitalWrite(led3, HIGH);
      delay(300);
      digitalWrite(led2, HIGH);
      delay(300);
      digitalWrite(led1, HIGH);
      delay(300);
      digitalWrite(led3, LOW);
      delay(300);
      digitalWrite(led2, LOW);
      delay(300);
      digitalWrite(led1, LOW);


    }


    if (redSwitchState) {
      for (int i = 0; i < 3; i++) {

        digitalWrite(led3, HIGH);
        digitalWrite(led2, HIGH);
        digitalWrite(led1, HIGH);
        delay(300);
        digitalWrite(led3, LOW);
        digitalWrite(led2, LOW);
        digitalWrite(led1, LOW);
        delay(300);


      }

    }



  }




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
