const express = require('express');
const http = require('http');
const WebSocket = require('ws');
const path = require('path');

// Initialize Express and Create HTTP Server
const app = express();
const server = http.createServer(app);
// Set Up WebSocket Server
const wss = new WebSocket.Server({ server });

app.use(express.static(path.join(__dirname, 'public')));

let esp32Connection = null;
let dashboardConnections = new Set();
let latestData = {
  distance: 0,
  mode: 'MANUAL'
};

// Error handling middleware
app.use((err, req, res, next) => {
  console.error(err.stack);
  res.status(500).send('Something broke!');
});

wss.on('connection', (ws, req) => {
  const isESP32 = req.headers['user-agent'] && req.headers['user-agent'].includes('ESP32');

  if (isESP32) {
    console.log('ESP32 connected');
    esp32Connection = ws;

    ws.on('message', (message) => {
      try {
        const data = JSON.parse(message);
        console.log('Received from ESP32:', data);

        // Update latest data with new information
        if (data.distance !== undefined) latestData.distance = data.distance;
        if (data.mode) latestData.mode = data.mode;

        broadcastData();
      } catch (error) {
        console.error('Error parsing message from ESP32:', error);
      }
    });

    ws.on('close', () => {
      console.log('ESP32 disconnected');
      esp32Connection = null;
      // Broadcast disconnection to dashboards
      broadcastData();
    });

    ws.on('open', () => {
      if (esp32Connection) {
        // Send the current mode to the ESP32
        ws.send(JSON.stringify({ command: latestData.mode }));
      }
    });
  } else {
    console.log('Dashboard connected');
    dashboardConnections.add(ws);

    // Send the latest data to the newly connected dashboard
    ws.send(JSON.stringify(latestData));

    ws.on('message', (message) => {
      try {
        const data = JSON.parse(message);
        console.log('Received from dashboard:', data);

        if (data.command) {
          // Update mode and forward command to ESP32
          latestData.mode = data.command === 'AUTONOMOUS' ? 'AUTONOMOUS' : 'MANUAL';

          if (esp32Connection && esp32Connection.readyState === WebSocket.OPEN) {
            esp32Connection.send(JSON.stringify({
              command: data.command,
              // Add any additional control parameters if needed
              timestamp: Date.now()
            }));
            console.log('Sent command to ESP32:', data.command);
          } else {
            console.log('ESP32 not connected or not ready');
          }

          broadcastData();
        }
      } catch (error) {
        console.error('Error parsing message from dashboard:', error);
      }
    });

    ws.on('close', () => {
      console.log('Dashboard disconnected');
      dashboardConnections.delete(ws);
    });
  }
});

function broadcastData() {
  // Add timestamp to help with sync and debugging
  const broadcastData = {
    distance: latestData.distance,  // Ultrasonic distance
    mode: latestData.mode,          // Control mode (AUTONOMOUS or MANUAL)
    timestamp: Date.now(),          // Current timestamp
    esp32Connected: esp32Connection !== null // ESP32 connection status
  };

  dashboardConnections.forEach((client) => {
    if (client.readyState === WebSocket.OPEN) {
      client.send(JSON.stringify(broadcastData));
    }
  });
}

const PORT = process.env.PORT || 3000;
server.listen(PORT, '0.0.0.0', () => {
  console.log(`Server is running on port ${PORT}`);
  console.log(`Accessible at http://localhost:${PORT}`);
});

// Periodic status check and data broadcast
setInterval(() => {
  console.log('Connection status:');
  console.log('ESP32 connected:', esp32Connection !== null);
  console.log('Number of dashboard connections:', dashboardConnections.size);
  console.log('Latest data:', latestData);

  // Broadcast data to keep connections alive
  broadcastData();
}, 5000);

// Heartbeat mechanism to detect disconnections
function noop() { }

function heartbeat() {
  this.isAlive = true;
}

wss.on('connection', (ws) => {
  ws.isAlive = true;
  ws.on('pong', heartbeat);
});

const interval = setInterval(() => {
  wss.clients.forEach((ws) => {
    if (ws.isAlive === false) return ws.terminate();

    ws.isAlive = false;
    ws.ping(noop);
  });
}, 30000);

wss.on('close', () => {
  clearInterval(interval);
});