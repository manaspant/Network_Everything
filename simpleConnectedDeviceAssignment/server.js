const http = require('http')
const fs = require('fs');
const express = require('express'); // web server application
const app = express();        // instantiate express server
const server = http.Server(app);  // connects http library to server
const hostname = "127.0.0.1";


const PORT=8080;

app.use(express.static('public'));  // find pages in public directory

server.listen(PORT, () => {
  console.log(`Server running at http://${hostname}:${PORT}/`);
})