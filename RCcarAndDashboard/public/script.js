let socket;
let reconnectAttempts = 0;
const MAX_RECONNECT_ATTEMPTS = 5;

// DOM Element References
const elements = {
    distanceValue: document.getElementById('distanceValue'),
    progressFill: document.querySelector('.progress-fill'),
    timestamp: document.getElementById('timestamp'),
    warningAlert: document.getElementById('warningAlert'),
    autonomousToggle: document.getElementById('autonomousToggle'),
    statusIndicator: document.querySelector('.status-indicator'),
    connectionStatus: document.querySelector('.connection-status span')
};

function connectWebSocket() {
    socket = new WebSocket('ws://' + window.location.hostname + ':3000');

    socket.addEventListener('open', (event) => {
        console.log('Connected to WebSocket server');
        updateConnectionStatus(true);
        reconnectAttempts = 0;
    });

    socket.addEventListener('message', (event) => {
        try {
            const data = JSON.parse(event.data);
            console.log('Received data:', data);
            updateDashboard(data);
        } catch (error) {
            console.error('Error parsing message:', error);
        }
    });

    socket.addEventListener('close', (event) => {
        console.log('Disconnected from WebSocket server');
        updateConnectionStatus(false);
        
        // Implement reconnection with backoff
        if (reconnectAttempts < MAX_RECONNECT_ATTEMPTS) {
            const delay = Math.pow(2, reconnectAttempts) * 1000;
            reconnectAttempts++;
            
            setTimeout(connectWebSocket, delay);
        } else {
            console.error('Maximum reconnection attempts reached');
            alert('Connection lost. Please refresh the page.');
        }
    });
}

function updateConnectionStatus(isConnected) {
    elements.statusIndicator.style.backgroundColor = isConnected ? '#22c55e' : '#dc3545';
    elements.connectionStatus.textContent = isConnected ? 'Connected' : 'Disconnected';
}

function updateDashboard(data) {
    // Distance tracking
    elements.distanceValue.textContent = data.distance ? data.distance.toFixed(2) : 'N/A';
    const percentage = Math.min(100, (data.distance / 300) * 100);
    elements.progressFill.style.width = `${percentage}%`;
    
    // Timestamp
    const now = new Date();
    elements.timestamp.textContent = `Last Updated: ${now.toLocaleTimeString()}`;
    
    // Warning for close obstacles
    if (data.distance < 20) {
        elements.warningAlert.style.display = 'block';
        elements.progressFill.style.backgroundColor = '#dc3545';
    } else {
        elements.warningAlert.style.display = 'none';
        elements.progressFill.style.backgroundColor = '#007bff';
    }

    // Autonomous mode toggle
    elements.autonomousToggle.checked = data.mode === 'AUTONOMOUS';

    // Additional telemetry (if available)
    elements.batteryLevel.textContent = data.battery ? `${data.battery}%` : 'N/A';
    elements.speedValue.textContent = data.speed ? `${data.speed} km/h` : 'N/A';
    elements.temperatureValue.textContent = data.temperature ? `${data.temperature}°C` : 'N/A';
}

// Event Listeners
elements.autonomousToggle.addEventListener('change', (event) => {
    const command = event.target.checked ? 'AUTONOMOUS' : 'MANUAL';
    sendCommand(command);
});

// Keyboard control (only active in manual mode)
document.addEventListener('keydown', (event) => {
    if (!elements.autonomousToggle.checked) {
        let command = '';
        switch (event.key) {
            case 'ArrowUp': command = 'FORWARD'; break;
            case 'ArrowDown': command = 'BACKWARD'; break;
            case 'ArrowLeft': command = 'LEFT'; break;
            case 'ArrowRight': command = 'RIGHT'; break;
        }
        if (command) sendCommand(command);
    }
});

document.addEventListener('keyup', (event) => {
    if (!elements.autonomousToggle.checked && 
        ['ArrowUp', 'ArrowDown', 'ArrowLeft', 'ArrowRight'].includes(event.key)) {
        sendCommand('STOP');
    }
});

// Command sending with error handling
function sendCommand(command) {
    if (socket && socket.readyState === WebSocket.OPEN) {
        try {
            socket.send(JSON.stringify({ command }));
            console.log('Sent command:', command);
        } catch (error) {
            console.error('Error sending command:', error);
            alert('Failed to send command. Check connection.');
        }
    } else {
        console.error('WebSocket is not connected');
        alert('Connection lost. Attempting to reconnect...');
        connectWebSocket();
    }
}

// Camera loading function
function loadCamera() {
    const inputLink = document.getElementById("inputLink").value;
    const cameraFrame = document.getElementById("cameraFrame");
    const noVideoMessage = document.getElementById("noVideoMessage");
    const cameraFeedContainer = document.querySelector(".camera-feed");

    if (inputLink) {
        cameraFrame.src = inputLink;
        noVideoMessage.style.display = "none";

        cameraFrame.onload = () => {
            noVideoMessage.style.display = "none";
            cameraFeedContainer.style.backgroundColor = "transparent";
        };

        cameraFrame.onerror = () => {
            cameraFeedContainer.style.backgroundColor = "#000";
            noVideoMessage.style.display = "block";
            noVideoMessage.innerHTML = "Failed to load video stream. Please check the IP address.";
        };
    } else {
        alert("Please enter a valid IP camera URL.");
        cameraFeedContainer.style.backgroundColor = "#000";
        noVideoMessage.style.display = "block";
        noVideoMessage.innerHTML = "No video stream available. Please load an IP camera.";
    }
}

// Initialize the dashboard
document.addEventListener('DOMContentLoaded', () => {
    connectWebSocket();
    
    // Initialize with default/placeholder values
    updateDashboard({
        distance: 0,
        mode: 'MANUAL'
    });
});