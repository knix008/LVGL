#!/usr/bin/env python3

import socket
import json
import sys

def send_command(host, port, command):
    try:
        # Create socket
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.settimeout(10)
        
        # Connect to server
        print(f"Connecting to {host}:{port}...")
        s.connect((host, port))
        print("Connected!")
        
        # Send command
        command_json = json.dumps(command)
        print(f"Sending: {command_json}")
        s.send(command_json.encode())
        
        # Receive response
        response = s.recv(4096).decode()
        print(f"Received: {response}")
        
        # Parse and display response
        try:
            response_data = json.loads(response)
            print(f"Command: {response_data.get('command', 'unknown')}")
            print(f"Success: {response_data.get('success', False)}")
            print(f"Message: {response_data.get('message', '')}")
            print(f"Score: {response_data.get('score', 0.0)}")
        except json.JSONDecodeError:
            print("Invalid JSON response")
        
        s.close()
        
    except Exception as e:
        print(f"Error: {e}")

if __name__ == "__main__":
    # Test commands
    test_commands = [
        {"command": "status", "device": "", "parameters": ""},
        {"command": "test", "device": "network", "parameters": "all"},
        {"command": "test", "device": "cpu", "parameters": "all"},
        {"command": "test", "device": "serial", "parameters": "all"},
        {"command": "test", "device": "serial", "parameters": "device:/dev/ttyUSB0,baud:115200,test:init"},
        {"command": "test", "device": "serial", "parameters": "device:/dev/ttyUSB0,baud:9600,test:comm"},
        {"command": "shutdown", "device": "", "parameters": ""}
    ]
    
    host = "localhost"
    port = 8081
    
    if len(sys.argv) > 1:
        port = int(sys.argv[1])
    
    for i, cmd in enumerate(test_commands):
        print(f"\n=== Test {i+1}: {cmd['command']} ===")
        send_command(host, port, cmd)
        
        if cmd['command'] == 'shutdown':
            print("Sent shutdown command, server should stop.")
            break
