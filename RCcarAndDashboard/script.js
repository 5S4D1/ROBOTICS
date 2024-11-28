function loadCamera() {
    const inputLink = document.getElementById("inputLink").value;
    const cameraFrame = document.getElementById("cameraFrame");
    const noVideoMessage = document.getElementById("noVideoMessage");
    const cameraFeedContainer = document.querySelector(".camera-feed");

    if (inputLink) {
        cameraFrame.src = inputLink;
        noVideoMessage.style.display = "none"; // Hide message when video starts loading

        cameraFrame.onload = () => {
            noVideoMessage.style.display = "none"; // Hide the message on successful load
            cameraFeedContainer.style.backgroundColor = "transparent";
        };

        cameraFrame.onerror = () => {
            cameraFeedContainer.style.backgroundColor = "#000"; // Keep black background on error
            noVideoMessage.style.display = "block"; // Show message if video fails to load
            noVideoMessage.innerHTML = "Failed to load video stream. Please check the IP address.";
        };
    } else {
        alert("Please enter a valid IP camera URL.");
        cameraFeedContainer.style.backgroundColor = "#000";
        noVideoMessage.style.display = "block"; // Show message when no input is provided
        noVideoMessage.innerHTML = "No video stream available. Please load an IP camera.";
    }
}
