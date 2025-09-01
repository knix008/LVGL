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
        return response_data.get('success', False)
        
    except Exception as e:
        print(f"Error: {e}")
        return False

if __name__ == "__main__":
    host = "localhost"
    port = 8083
    
    if len(sys.argv) > 1:
        port = int(sys.argv[1])
    
    print("=== Serial Communication Test Examples ===\n")
    
    # Test serial commands with different parameters
    serial_commands = [
        {
            "name": "Basic serial test (default device)",
            "command": {"command": "test", "device": "serial", "parameters": "all"}
        },
        {
            "name": "Serial initialization test on /dev/ttyUSB0 at 115200 baud",
            "command": {"command": "test", "device": "serial", "parameters": "device:/dev/ttyUSB0,baud:115200,test:init"}
        },
        {
            "name": "Serial communication test on /dev/ttyUSB0 at 9600 baud",
            "command": {"command": "test", "device": "serial", "parameters": "device:/dev/ttyUSB0,baud:9600,test:comm"}
        },
        {
            "name": "Serial loopback test on /dev/ttyS0 at 38400 baud",
            "command": {"command": "test", "device": "serial", "parameters": "device:/dev/ttyS0,baud:38400,test:loopback"}
        },
        {
            "name": "Serial speed test on /dev/ttyUSB1 at 230400 baud",
            "command": {"command": "test", "device": "serial", "parameters": "device:/dev/ttyUSB1,baud:230400,test:speed"}
        },
        {
            "name": "Serial configuration test",
            "command": {"command": "test", "device": "serial", "parameters": "test:config"}
        },
        {
            "name": "All serial tests on custom device",
            "command": {"command": "test", "device": "serial", "parameters": "device:/dev/ttyACM0,baud:57600,test:all"}
        }
    ]
    
    for i, test in enumerate(serial_commands):
        print(f"=== Test {i+1}: {test['name']} ===")
        success = send_command(host, port, test['command'])
        print(f"Result: {'SUCCESS' if success else 'FAILED'}")
        print("-" * 60)
        
        if not success and "ttyUSB" in test['command']['parameters']:
            print("Note: This test may fail if the serial device is not connected")
            print("      or if you don't have permission to access the device.")
            print()
    
    print("\n=== Serial Command Format ===")
    print("Parameters format: device:<path>,baud:<rate>,test:<test_name>")
    print("Available tests: init, comm, loopback, speed, error, config, all")
    print("Default device: /dev/ttyUSB0")
    print("Default baud rate: 115200")
    print("Default test: all")
    print("\nExamples:")
    print('  {"command":"test","device":"serial","parameters":"all"}')
    print('  {"command":"test","device":"serial","parameters":"device:/dev/ttyUSB0,baud:9600,test:init"}')
    print('  {"command":"test","device":"serial","parameters":"baud:115200,test:comm"}')
    print('  {"command":"test","device":"serial","parameters":"device:/dev/ttyS0,test:loopback"}')
