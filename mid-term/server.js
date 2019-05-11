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
var ARDUINO_IP_ADDRESS = '192.168.1.39'; // IMNetwork

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

var transporter = nodemailer.createTransport({
 service: 'gmail',
 auth: {
        user: 'safety.device.iot@gmail.com',
        pass: 'internetofthings'
    }
});

const mailOptions = {
  from: 'sender@email.com', // sender address
  to: 'manaspant1997@gmail.com', // list of receivers
  subject: 'Stalker ALERT!!!!!!', // Subject line
  html: 'Thief near your project!! Send them an alert using this link: http://localhost:8090/'// plain text body
};

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

	udpServer.send(message, 0, message.length, sender.port, sender.address);
}

function sendEmail() {
	transporter.sendMail(mailOptions, function (err, info) {
	   if(err)
	     console.log(err);
	   else
	     console.log(info);
	});
}

/* Register the UDP callback functions */
udpServer.bind( MY_PORT_FOR_ARDUINO );
udpServer.on('listening', ArduinoUDPServerIsListening);
udpServer.on('message', ArduinoUDPServerReceivedMessage);
udpServer.on('message', sendEmail);

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
	const SIZEOF_LED_DATA = 2;
	var sound = new Uint8Array(SIZEOF_LED_DATA ); 
  
	console.log('Web server socket: Client connected');

    socket.on('sendAlert', function () {
      console.log('Web server socket: received alert message from web client');
      // this is where we would send the message to Arduino
		sound[0] = 2;  // Red is LED number 0
		sound[1] = 1; // turn off the LED
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
