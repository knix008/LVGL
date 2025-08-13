#!/usr/bin/env node

/**
 * WebSocket Test Client for LVGL WebServer
 * 
 * This script can be used in two ways:
 * 1. In Node.js: node websocket_test.js
 * 2. In browser: Include this file in an HTML page
 */

// Configuration
const CONFIG = {
    host: 'localhost',
    port: 8443,
    path: '/ws',
    secure: true,
    reconnectAttempts: 5,
    reconnectDelay: 2000,
    testInterval: 1000
};

// Test results storage
let testResults = {
    connection: false,
    messages: [],
    errors: [],
    commands: []
};

// WebSocket instance
let ws = null;
let reconnectCount = 0;
let isRunning = false;

// Test commands to send
const TEST_COMMANDS = [
    { type: 'system', value: 'status' },
    { type: 'tab', value: 'main' },
    { type: 'tab', value: 'calendar' },
    { type: 'tab', value: 'clock' },
    { type: 'system', value: 'refresh' },
    { type: 'database', value: 'query' },
    { type: 'video', value: 'play' },
    { type: 'custom', value: 'test_message' }
];

/**
 * Logging function
 */
function log(message, type = 'info') {
    const timestamp = new Date().toISOString();
    const logEntry = `[${timestamp}] [${type.toUpperCase()}] ${message}`;
    
    if (typeof window !== 'undefined') {
        // Browser environment
        console.log(logEntry);
        if (window.logToElement) {
            window.logToElement(logEntry);
        }
    } else {
        // Node.js environment
        console.log(logEntry);
    }
    
    testResults.messages.push({ timestamp, type, message });
}

/**
 * Create WebSocket URL
 */
function createWebSocketUrl() {
    const protocol = CONFIG.secure ? 'wss' : 'ws';
    return `${protocol}://${CONFIG.host}:${CONFIG.port}${CONFIG.path}`;
}

/**
 * Connect to WebSocket
 */
function connect() {
    if (ws && ws.readyState === WebSocket.OPEN) {
        log('WebSocket already connected', 'warn');
        return;
    }
    
    const url = createWebSocketUrl();
    log(`Connecting to ${url}...`);
    
    try {
        ws = new WebSocket(url);
        
        ws.onopen = function(event) {
            log('WebSocket connection established successfully');
            testResults.connection = true;
            reconnectCount = 0;
            
            // Start automated tests if running
            if (isRunning) {
                setTimeout(runAutomatedTests, 1000);
            }
        };
        
        ws.onclose = function(event) {
            log(`WebSocket connection closed. Code: ${event.code}, Reason: ${event.reason}`);
            testResults.connection = false;
            
            if (isRunning && reconnectCount < CONFIG.reconnectAttempts) {
                reconnectCount++;
                log(`Attempting to reconnect... (${reconnectCount}/${CONFIG.reconnectAttempts})`);
                setTimeout(connect, CONFIG.reconnectDelay);
            } else if (reconnectCount >= CONFIG.reconnectAttempts) {
                log('Max reconnection attempts reached', 'error');
            }
        };
        
        ws.onerror = function(error) {
            log(`WebSocket error: ${error.message || 'Unknown error'}`, 'error');
            testResults.errors.push({ timestamp: new Date().toISOString(), error: error.message || 'Unknown error' });
        };
        
        ws.onmessage = function(event) {
            try {
                const data = JSON.parse(event.data);
                log(`Received: ${JSON.stringify(data, null, 2)}`);
                
                // Store command response
                testResults.commands.push({
                    timestamp: new Date().toISOString(),
                    received: data
                });
                
            } catch (e) {
                log(`Failed to parse message: ${event.data}`, 'error');
            }
        };
        
    } catch (error) {
        log(`Failed to create WebSocket: ${error.message}`, 'error');
    }
}

/**
 * Send command to WebSocket
 */
function sendCommand(type, value) {
    if (!ws || ws.readyState !== WebSocket.OPEN) {
        log('WebSocket not connected', 'error');
        return false;
    }
    
    const command = { type, value };
    const jsonCommand = JSON.stringify(command);
    
    try {
        ws.send(jsonCommand);
        log(`Sent command: ${jsonCommand}`);
        
        // Store sent command
        testResults.commands.push({
            timestamp: new Date().toISOString(),
            sent: command
        });
        
        return true;
    } catch (error) {
        log(`Failed to send command: ${error.message}`, 'error');
        return false;
    }
}

/**
 * Run automated tests
 */
function runAutomatedTests() {
    if (!isRunning) return;
    
    log('Starting automated tests...');
    
    let commandIndex = 0;
    
    const sendNextCommand = () => {
        if (!isRunning || commandIndex >= TEST_COMMANDS.length) {
            log('Automated tests completed');
            isRunning = false;
            return;
        }
        
        const command = TEST_COMMANDS[commandIndex];
        sendCommand(command.type, command.value);
        commandIndex++;
        
        setTimeout(sendNextCommand, CONFIG.testInterval);
    };
    
    sendNextCommand();
}

/**
 * Start automated testing
 */
function startAutomatedTests() {
    isRunning = true;
    log('Starting automated WebSocket tests...');
    
    if (!testResults.connection) {
        connect();
    } else {
        runAutomatedTests();
    }
}

/**
 * Stop automated testing
 */
function stopAutomatedTests() {
    isRunning = false;
    log('Stopping automated tests...');
}

/**
 * Disconnect WebSocket
 */
function disconnect() {
    if (ws) {
        ws.close();
        ws = null;
    }
    testResults.connection = false;
    log('WebSocket disconnected');
}

/**
 * Get test results
 */
function getTestResults() {
    return {
        ...testResults,
        summary: {
            totalCommands: testResults.commands.length,
            totalErrors: testResults.errors.length,
            connectionStatus: testResults.connection ? 'connected' : 'disconnected',
            reconnectAttempts: reconnectCount
        }
    };
}

/**
 * Clear test results
 */
function clearTestResults() {
    testResults = {
        connection: false,
        messages: [],
        errors: [],
        commands: []
    };
    reconnectCount = 0;
    log('Test results cleared');
}

/**
 * Manual test functions
 */
function testConnection() {
    log('Testing WebSocket connection...');
    connect();
}

function testBasicCommands() {
    log('Testing basic commands...');
    sendCommand('system', 'status');
    setTimeout(() => sendCommand('tab', 'main'), 500);
    setTimeout(() => sendCommand('system', 'refresh'), 1000);
}

function testCustomMessage() {
    log('Testing custom message...');
    sendCommand('custom', 'Hello from JavaScript test client!');
}

// Export functions for browser use
if (typeof window !== 'undefined') {
    window.WebSocketTest = {
        connect,
        disconnect,
        sendCommand,
        startAutomatedTests,
        stopAutomatedTests,
        testConnection,
        testBasicCommands,
        testCustomMessage,
        getTestResults,
        clearTestResults,
        CONFIG
    };
}

// Node.js CLI interface
if (typeof module !== 'undefined' && module.exports) {
    const readline = require('readline');
    
    const rl = readline.createInterface({
        input: process.stdin,
        output: process.stdout
    });
    
    function showMenu() {
        console.log('\n=== WebSocket Test Client ===');
        console.log('1. Connect');
        console.log('2. Disconnect');
        console.log('3. Send custom command');
        console.log('4. Test basic commands');
        console.log('5. Start automated tests');
        console.log('6. Stop automated tests');
        console.log('7. Show test results');
        console.log('8. Clear test results');
        console.log('9. Exit');
        console.log('============================');
    }
    
    function handleInput(input) {
        switch (input.trim()) {
            case '1':
                connect();
                break;
            case '2':
                disconnect();
                break;
            case '3':
                rl.question('Enter command type: ', (type) => {
                    rl.question('Enter command value: ', (value) => {
                        sendCommand(type, value);
                        showMenu();
                    });
                });
                return;
            case '4':
                testBasicCommands();
                break;
            case '5':
                startAutomatedTests();
                break;
            case '6':
                stopAutomatedTests();
                break;
            case '7':
                console.log(JSON.stringify(getTestResults(), null, 2));
                break;
            case '8':
                clearTestResults();
                break;
            case '9':
                disconnect();
                rl.close();
                process.exit(0);
                break;
            default:
                console.log('Invalid option');
        }
        showMenu();
    }
    
    // Start CLI
    showMenu();
    rl.on('line', handleInput);
    
    // Handle Ctrl+C
    process.on('SIGINT', () => {
        disconnect();
        rl.close();
        process.exit(0);
    });
}

// Auto-connect if running in Node.js
if (typeof module !== 'undefined' && module.exports) {
    log('WebSocket test client started');
    log(`Target: ${createWebSocketUrl()}`);
    log('Use the menu to interact with the WebSocket');
}
