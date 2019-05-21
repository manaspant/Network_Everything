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

// variables to save pins for different button options

int buttonPin_World = 6;
int buttonPin_US = 7;
int buttonPin_NY = 8;
int buttonPin_GameStart = 9;
int buttonPin_Other = 10;

// game condition variables

bool winCondition = false;
bool readyGame = false;
bool GameStart = false;

// save response from server

int packetResponse;

// LED pins

int pin_win = 5;
int pin_lose = 4;
int pin_play = 3;

// User options buttons state

int buttonState_World = 0;
int buttonState_US = 0;
int buttonState_NY = 0;
int buttonState_GameStart = 0;
int buttonState_Other = 0;

#include <SPI.h>
#include <WiFiNINA.h>
#include <WiFiUdp.h>

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

int status = WL_IDLE_STATUS;

#include "arduino_secrets.h"

//please enter your sensitive data in the Secret tab/arduino_secrets.h

char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;            // your network key Index number (needed only for WEP)

unsigned int localPort = 5000;      // local port to listen on

char packetBuffer[255]; //buffer to hold incoming packet

WiFiUDP Udp;

void setup() {

  pinMode(pin_win, OUTPUT);
  pinMode(pin_lose, OUTPUT);
  pinMode(pin_play, OUTPUT);

  digitalWrite(pin_win, LOW);
  digitalWrite(pin_lose, LOW);
  digitalWrite(pin_play, LOW);

  pinMode(buttonPin_World, INPUT);
  pinMode(buttonPin_US, INPUT);
  pinMode(buttonPin_NY, INPUT);
  pinMode(buttonPin_GameStart, INPUT);
  pinMode(buttonPin_Other, INPUT);

  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  //  while (!Serial) {
  //    ; // wait for serial port to connect. Needed for native USB port only
  //  }

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

  // if there's data available, read a packet
  int packetSize = Udp.parsePacket();
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

    packetResponse = (int)packetBuffer[0];  // value of server message

    if (packetResponse == 1) {  // Checking what category the article falls under

      Serial.println("The article is about World");
      readyGame = true;
      digitalWrite(pin_play, HIGH);

    }

    if (packetResponse == 2) {  // Checking what category the article falls under

      Serial.println("The article is about US");
      readyGame = true;
      digitalWrite(pin_play, HIGH);

    }

    if (packetResponse == 3) {  // Checking what category the article falls under

      Serial.println("The article is about opinion");
      readyGame = true;
      digitalWrite(pin_play, HIGH);

    }

    if (packetResponse == 4) {  // Checking whether the start new game button has been pressed on the webpage. Will reset variables if TRUE

      Serial.println("Reset game!!!!");
      winCondition = false;
      readyGame = false;
      GameStart = false;

      digitalWrite(pin_win, LOW);
      digitalWrite(pin_lose, LOW);
      digitalWrite(pin_play, LOW);

    }

    if (packetResponse == 5) {  // Checking what category the article falls under

      Serial.println("The article is about other stuff");
      readyGame = true;
      digitalWrite(pin_play, HIGH);

    }

  }


  // IP address of the receiving device
  IPAddress receivingDeviceAddress(10, 225, 161, 132);
  unsigned int receivingDevicePort = 2390;

  buttonState_GameStart = digitalRead(buttonPin_GameStart);

  // Inform server.js that start new game has been pressed

  if (buttonState_GameStart == 1 && GameStart == false) {

    Serial.println("Starting Game");
    GameStart = true;

    Udp.beginPacket(receivingDeviceAddress, receivingDevicePort);
    Udp.write(1);
    Udp.endPacket();
  }

  // reading buttons

  buttonState_World = digitalRead(buttonPin_World);
  buttonState_US = digitalRead(buttonPin_US);
  buttonState_Other = digitalRead(buttonPin_Other);
  buttonState_NY = digitalRead(buttonPin_NY);

  // Check if the answer is correct if the packetresponse corresponds to the right button press

  if (buttonState_World == HIGH && readyGame == true && winCondition == false) {  // This is where the LED On/Off happens

    if (packetResponse == 1) {
      digitalWrite(pin_play, LOW);
      Serial.println("YOU GOT IT!");
      winCondition = true;
      digitalWrite(pin_win, HIGH);

      // switch on Green light since answer correct
    }

    if (packetResponse != 1) {
      digitalWrite(pin_play, LOW);
      Serial.println("YOU LOST IT!");
      winCondition = true;
      digitalWrite(pin_lose, HIGH);

      // switch on Red light since answer incorrect
    }

    readyGame = false;


    Serial.println("Telling server about your result");
    Udp.beginPacket(receivingDeviceAddress, receivingDevicePort);
    Udp.write(2);
    Udp.endPacket();
    winCondition = false;


    // inform server.js that the user has answered
  }

  // Check if the answer is correct if the packetresponse corresponds to the right button press

  if (buttonState_US == HIGH && readyGame == true && winCondition == false) {  // This is where the LED On/Off happens

    if (packetResponse == 2) {
      digitalWrite(pin_play, LOW);
      Serial.println("YOU GOT IT!");
      winCondition = true;
      digitalWrite(pin_win, HIGH);

      // switch on Green light since answer correct
    }

    if (packetResponse != 2) {
      digitalWrite(pin_play, LOW);
      Serial.println("YOU LOST IT!");
      winCondition = true;
      digitalWrite(pin_lose, HIGH);

      // switch on Red light since answer incorrect
    }

    readyGame = false;

    Serial.println("Telling server about your result");
    Udp.beginPacket(receivingDeviceAddress, receivingDevicePort);
    Udp.write(2);
    Udp.endPacket();
    winCondition = false;

    // inform server.js that the user has answered

  }

  // Check if the answer is correct if the packetresponse corresponds to the right button press

  if (buttonState_NY == HIGH && readyGame == true && winCondition == false) {  // This is where the LED On/Off happens

    Serial.println("Reached here at least");

    if (packetResponse == 3) {
      digitalWrite(pin_play, LOW);
      Serial.println("YOU GOT IT!");
      winCondition = true;
      digitalWrite(pin_win, HIGH);

      // switch on Green light since answer correct
    }
    if (packetResponse != 3) {
      digitalWrite(pin_play, LOW);
      Serial.println("YOU LOST IT!");
      winCondition = true;
      digitalWrite(pin_lose, HIGH);

      // switch on Red light since answer incorrect
    }

    readyGame = false;

    Serial.println("Telling server about your result");
    Udp.beginPacket(receivingDeviceAddress, receivingDevicePort);
    Udp.write(2);
    Udp.endPacket();
    winCondition = false;

    // inform server.js that the user has answered
  }

  // Check if the answer is correct if the packetresponse corresponds to the right button press

  if (buttonState_Other == HIGH && readyGame == true && winCondition == false) {  // This is where the LED On/Off happens

    if (packetResponse == 5) {
      digitalWrite(pin_play, LOW);
      Serial.println("YOU GOT IT!");
      winCondition = true;
      digitalWrite(pin_win, HIGH);

      // switch on Green light since answer correct
    }
    if (packetResponse != 5) {
      digitalWrite(pin_play, LOW);
      Serial.println("YOU LOST IT!");
      winCondition = true;
      digitalWrite(pin_lose, HIGH);

      // switch on Red light since answer incorrect
    }

    readyGame = false;

    Serial.println("Telling server about your result");
    Udp.beginPacket(receivingDeviceAddress, receivingDevicePort);
    Udp.write(2);
    Udp.endPacket();
    winCondition = false;

    // inform server.js that the user has answered

  }

}
