
// Modules
const http = require('http');

// Config
const port = 8080;
const url = "http://localhost:" + 8080;

// Server Listener
const requestListener = function (req, res) {

    res.writeHead(200);
    res.end('Hello, World!\nNode.js will automatically stop when you close this window.\nNo need for Ctrl + D!');
}

// Create a web server using Nodejs
const server = http.createServer(requestListener);
server.listen(port);

// Run the WebUI example application to open a new window
const { spawn } = require("child_process");
const ls = spawn("window", [url]);
ls.on('error', (error) => {
    console.log('Failed to start the window');
    process.exit();
});
ls.on("close", code => {
    console.log('Bye.');
    process.exit();
});
