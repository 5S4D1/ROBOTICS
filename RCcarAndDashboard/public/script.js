// WebSocket connection
const socket = new WebSocket('ws://' + window.location.hostname + ':3000');

// DOM elements
const distanceValue = document.getElementById('distanceValue');
const progressFill = document.querySelector('.progress-fill');
const timestamp = document.getElementById('timestamp');
const warningAlert = document.getElementById('warningAlert');
const autonomousToggle = document.getElementById('autonomousToggle');
const statusIndicator = document.querySelector('.status-indicator');
const connectionStatus = document.querySelector('.connection-status span');

socket.addEventListener('open', (event) => {
    console.log('Connected to WebSocket server');
    statusIndicator.style.backgroundColor = '#22c55e';
    connectionStatus.textContent = 'Connected';
});

socket.addEventListener('message', (event) => {
    try {
        const data = JSON.parse(event.data);
        console.log('Received data:', data);
        
        // Check if the distance is available and update the dashboard
        if (data.distance !== undefined) {
            updateDashboard(data.distance);  // This will update the displayed distance
        }
    } catch (error) {
        console.error('Error parsing message:', error);
    }
});

function updateDashboard(distance) {
    distanceValue.textContent = distance;  // Update the distance on your dashboard
    const percentage = Math.min(100, (distance / 400) * 100);
    progressFill.style.width = `${percentage}%`;
    
    const now = new Date();
    timestamp.textContent = `Last Updated: ${now.toLocaleTimeString()}`;
    
    if (distance < 50) {
        warningAlert.style.display = 'block';
        progressFill.style.backgroundColor = '#dc3545';
    }
    else if (distance > 49 && distance < 130) {
        warningAlert.style.display = 'none';
        progressFill.style.backgroundColor = '#355F2E';
    } else {
        warningAlert.style.display = 'none';
        progressFill.style.backgroundColor = '#007bff';
    }
}


autonomousToggle.addEventListener('change', (event) => {
    const mode = event.target.checked ? 'AUTONOMOUS' : 'MANUAL';
    socket.send(JSON.stringify({ command: mode }));
});

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
    updateDashboard(999); // Initial distance value
});