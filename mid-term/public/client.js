/* 
This is the websocket library which will allow us to send messages
back to the web server 
*/
var socket = io();

socket.on('event', (data) => {
	console.log('received data from webserver: ' + data);
});

socket.on('buttonPressed', (data) => {
	console.log('received button pressed event from webserver: ' + data);
	displayTheImage();
});

socket.on('buttonReleased', (data) => {
	console.log('received button released from webserver: ' + data);
	hideTheImage();
});

function sendAlert() {
  socket.emit('sendAlert');
  console.log("Button clicked on browser")
}

