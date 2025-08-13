let ws = null;
let reconnectAttempts = 0;
const maxReconnectAttempts = 5;

function log(message) {
    const logDiv = document.getElementById('log');
    const timestamp = new Date().toLocaleTimeString();
    logDiv.innerHTML += `[${timestamp}] ${message}\n`;
    logDiv.scrollTop = logDiv.scrollHeight;
}

function connect() {
    const protocol = window.location.protocol === 'https:' ? 'wss:' : 'ws:';
    const wsUrl = protocol + '//' + window.location.host + '/ws';
    
    log(`Connecting to ${wsUrl}...`);
    ws = new WebSocket(wsUrl);
    
    ws.onopen = function(event) {
        document.getElementById('status').className = 'status online';
        document.getElementById('status').textContent = 'Connected';
        document.getElementById('protocol').textContent = window.location.protocol;
        document.getElementById('port').textContent = window.location.port || (window.location.protocol === 'https:' ? '443' : '80');
        reconnectAttempts = 0;
        log('WebSocket connection established successfully');
        log('WebSocket readyState: ' + ws.readyState);
        
        // Send initial status request
        setTimeout(function() {
            sendCommand('system', 'status');
        }, 100);
    };
    
    ws.onclose = function() {
        document.getElementById('status').className = 'status offline';
        document.getElementById('status').textContent = 'Disconnected';
        log('WebSocket connection closed');
        
        if (reconnectAttempts < maxReconnectAttempts) {
            reconnectAttempts++;
            log(`Attempting to reconnect... (${reconnectAttempts}/${maxReconnectAttempts})`);
            setTimeout(connect, 2000);
        } else {
            log('Max reconnection attempts reached');
        }
    };
    
    ws.onerror = function(error) {
        log('WebSocket error: ' + error);
    };
    
    ws.onmessage = function(event) {
        try {
            const data = JSON.parse(event.data);
            log(`Received: ${event.data}`);
            
            if (data.type === 'ui_state') {
                document.getElementById('uiState').textContent = JSON.stringify(data.state, null, 2);
            } else if (data.type === 'response') {
                log(`Command response: ${data.message}`);
            } else if (data.type === 'error') {
                log(`Error: ${data.message}`);
            }
        } catch (e) {
            log('Failed to parse message: ' + event.data);
        }
    };
}

function sendCommand(type, value) {
    if (!ws) {
        log('WebSocket not initialized');
        return;
    }
    
    if (ws.readyState === WebSocket.OPEN) {
        const command = {type: type, value: value};
        const jsonCommand = JSON.stringify(command);
        ws.send(jsonCommand);
        log(`Sent command: ${jsonCommand}`);
        log(`WebSocket readyState: ${ws.readyState}`);
    } else {
        log(`WebSocket not connected. ReadyState: ${ws.readyState}`);
        log('Attempting to reconnect...');
        connect();
    }
}

function sendCustomCommand() {
    const type = document.getElementById('commandType').value;
    const value = document.getElementById('commandValue').value;
    if (value.trim()) {
        sendCommand(type, value);
        document.getElementById('commandValue').value = '';
    }
}

// Handle Enter key in command input
document.addEventListener('DOMContentLoaded', function() {
    document.getElementById('commandValue').addEventListener('keypress', function(e) {
        if (e.key === 'Enter') {
            sendCustomCommand();
        }
    });
    
    connect();
});
