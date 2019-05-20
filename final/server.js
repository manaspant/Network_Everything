/*
	UDP And HTTP Server
	Context: node.js

	Serve a web page to a browser with a control panel
	Read control panel and send results to Arduino 

	Web server provided by modules "http" and "express"

	Communication with Arduino is via a UDP socket
	provided by module "dgram"

	Communication with the web client (web browser) 
	is via a UDP socket provided by webSockets.
	Websockets creates a socket on top of the HTTP protocol
	The webSockets module is "socket.io"

	created 7 March 2019
	by Michael Shiloh

	Change log:

	14 Mar 2019 - ms - better comments and cleaned up code
										 send bytes instead of buffers to Arduino
										 receive button events from Arduino and send to web page
*/

/* UDP server talks to Arduino */
var dgram = require('dgram');
var nodemailer = require('nodemailer');
var udpServer = dgram.createSocket('udp4')
var MY_PORT_FOR_ARDUINO = 2390;
var ARDUINO_PORT_FOR_ME= 5000;
// var ARDUINO_IP_ADDRESS = '192.168.1.4'; // NETGEAR55
var ARDUINO_IP_ADDRESS = '10.225.161.131'; // IMNetwork

/* HTTP server talks to browser */
const http = require('http')
const express = require('express'); // web server application
const app = express();        // instantiate express server
const httpServer = http.Server(app);  // connects http library to server
const HTTP_SERVER_PORT = 8090;


// Express creates the simple web page
// The argument says where to find pages and scripts
app.use(express.static('public'));  

// websockets so that webpage can talk back to server
const webSocket = require('socket.io')(httpServer);  

/* Arduino UDP server callback functions */

function ArduinoUDPServerIsListening() {
	console.log('Arduino UDP Server is listening');
}

function ArduinoUDPServerReceivedMessage(message, sender) {

	// print the message
	console.log(
		'Received message from: ' +
		sender.address + 
		':' + 
		sender.port);
	console.log(
		'Message length: ' +
		message.length + 
		' Message contents: ' +
		message);

// This is the message from arduino when the start new game is pressed.
//Based on this, a message is sent to the scripts.js that will trigger the answers to be hidden and further instructions to pop up
		if (message.readUInt8(0) == 1 ) {
		console.log( "received a bc");
		webSocket.emit('GameStartKaroBC', 19);
	}


// this is the chosen answer from the arduino. This triggers the anser to be revealed 

			if (message.readUInt8(0) == 2 ) {
		console.log( "received the results");
		webSocket.emit('buttonPressed', 19);
	}
}

/* Register the UDP callback functions */
udpServer.bind( MY_PORT_FOR_ARDUINO );
udpServer.on('listening', ArduinoUDPServerIsListening);
udpServer.on('message', ArduinoUDPServerReceivedMessage);

/* HTTP callback functions */

httpServer.listen(HTTP_SERVER_PORT, () => {
	console.log('web server: Listening at', httpServer.address());
});

httpServer.on('connection', () => {
  console.log("web server: An HTTP client has connected")
});


// Websocket event handler 
// for UDP messages coming from the browser

webSocket.on('connect', (socket) => {
	// array for the message
	// sound[0] = LED number 
	// sound[1] = LED state 
	const SIZEOF_LED_DATA = 1;
	var sound = new Uint8Array(SIZEOF_LED_DATA ); 
  
	console.log('Web server socket: Client connected');


// All these different options depend on what article we got from scripts.js
// based on that we are sending a message to arduino telling it what article is the correct answer
// each article category has been assigned a number 
// in the arduino, we check that number to determine what the cateogry is

    socket.on('worldArticle', function () {
      console.log('Web server socket: received worldArticle message from web client');
      // this is where we would send the message to Arduino
		sound[0] = 1;  // Red is LED number 0
    // Send the message to Arduino
    	udpServer.send(
			sound,
			0, // offset to the message start in the buffer
			SIZEOF_LED_DATA,
			ARDUINO_PORT_FOR_ME, 
			ARDUINO_IP_ADDRESS);

    	console.log("message sent to arduino to turn on sound");
    });

        socket.on('USArticle', function () {
      console.log('Web server socket: received USArticle message from web client');
      // this is where we would send the message to Arduino
		sound[0] = 2;  // Red is LED number 0
    // Send the message to Arduino
    	udpServer.send(
			sound,
			0, // offset to the message start in the buffer
			SIZEOF_LED_DATA,
			ARDUINO_PORT_FOR_ME, 
			ARDUINO_IP_ADDRESS);

    	console.log("message sent to arduino to turn on sound");
    });

// The NY Article is actually for the opinion editorial

        socket.on('NYArticle', function () {
      console.log('Web server socket: received otherArticle message from web client');
      // this is where we would send the message to Arduino
		sound[0] = 3;  // Red is LED number 0
    // Send the message to Arduino
    	udpServer.send(
			sound,
			0, // offset to the message start in the buffer
			SIZEOF_LED_DATA,
			ARDUINO_PORT_FOR_ME, 
			ARDUINO_IP_ADDRESS);

    	console.log("message sent to arduino to turn on sound");
    });

// this one will send a message to arduino to reset the new game 

        socket.on('NEWGAME', function () {
      console.log('Web server socket: received otherArticle message from web client');
      // this is where we would send the message to Arduino
		sound[0] = 4;  // Red is LED number 0
    // Send the message to Arduino
    	udpServer.send(
			sound,
			0, // offset to the message start in the buffer
			SIZEOF_LED_DATA,
			ARDUINO_PORT_FOR_ME, 
			ARDUINO_IP_ADDRESS);

    	console.log("Message to start NEW GAME");
    });

        socket.on('otherArticle', function () {
      console.log('Web server socket: received otherArticle message from web client');
      // this is where we would send the message to Arduino
		sound[0] = 5;  // Red is LED number 0
    // Send the message to Arduino
    	udpServer.send(
			sound,
			0, // offset to the message start in the buffer
			SIZEOF_LED_DATA,
			ARDUINO_PORT_FOR_ME, 
			ARDUINO_IP_ADDRESS);

    	console.log("message sent to arduino to turn on sound");
    });

  // if you get the 'disconnect' message, say the user disconnected
  socket.on('disconnect', () => {
    console.log('Web server socket: user disconnected');
  });
});
