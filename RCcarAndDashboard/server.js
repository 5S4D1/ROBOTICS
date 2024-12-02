const express = require('express');
const http = require('http');
const WebSocket = require('ws');
const path = require('path');

const app = express();
const server = http.createServer(app);
const wss = new WebSocket.Server({ server });

// Serve static files from the 'public' directory
app.use(express.static(path.join(__dirname, 'public')));

let distance = 0; // Make sure this is declared globally

wss.on('connection', (ws) => {
    console.log('New WebSocket connection');

    ws.on('message', (message) => {
        try {
            const data = JSON.parse(message);
            console.log('Received:', data);

            // Update distance if ESP32 sends sensor data
            if (data.distance !== undefined) {
                distance = data.distance; // Update the distance with the correct sensor value
                console.log(`Updated distance from ESP32: ${distance}`);
            }

            // Handle commands from the client if necessary
            if (data.command) {
                console.log('Received command:', data.command);
            }
        } catch (error) {
            console.error('Error parsing message:', error);
        }
    });

    // Simulate sending the correct sensor data every second
    const simulateData = () => {
        console.log('Sending distance:', distance);  // Log the current distance before sending it
        if (distance === 0) {
            console.log('Warning: Distance is still 0. Check if the sensor data is updating correctly.');
        }
        ws.send(JSON.stringify({ distance }));
    };

    const dataInterval = setInterval(simulateData, 1000);

    ws.on('close', () => {
        clearInterval(dataInterval);
        console.log('WebSocket connection closed');
    });
});

const PORT = process.env.PORT || 3000;
server.listen(PORT, () => {
    console.log(`Server is running on port ${PORT}`);
});