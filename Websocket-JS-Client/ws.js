$(document).ready(function () {
    // TODO: Update address with IP assigned to your ESP32 in your local Wifi
    let socket = new WebSocket("ws://192.168.xxx.xxx/ws");

    console.log("Starting...");

    // Socket connection to ESP32 established
    socket.onopen = (e) => {
        addMessage("[open] Connection established");
    };

    // Message from ESP32 received
    socket.onmessage = (e) => {
        let message = e.data;
        addMessage(message);
    };

    // Message to ESP32 closed
    socket.onclose = (e) => {
        if (e.wasClean) {
            addMessage(`[close] Connection closed cleanly, code=${e.code} reason=${e.reason}`);
        } else {
            // e.g., server process was killed or network is down
            // event.code is usually 1006 in this case
            addMessage('[close] Connection died');
        }
    };

    // Error handler
    socket.onerror = (e) => {
        addMessage(`[error] ${e.message}`);
    };

    // Add message text to the HTML page
    function addMessage(text) {
        let messageElem = document.createElement('div');
        messageElem.textContent = text;
        document.getElementById('messages').prepend(messageElem);
    }

    // Input form submitted on HTML page -> send to ESP32
    $("#textform").submit((e) => {
        // Get the user entered text
        const enteredText = $("#usertext").val();

        // Send the message to the ESP32
        socket.send(enteredText);
        addMessage(`[sent] ${enteredText}`);

        // Prevent page reload
        e.preventDefault();
    })

})