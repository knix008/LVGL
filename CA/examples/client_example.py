#!/usr/bin/env python3
"""
Certificate Authority Client Example

This script demonstrates how to interact with the CA server API
to create, list, and manage certificates.
"""

import requests
import json
import sys
from datetime import datetime

class CAClient:
    def __init__(self, base_url="http://localhost:8080"):
        self.base_url = base_url
        self.api_url = f"{base_url}/api"
    
    def create_certificate(self, common_name, email="", organization="", 
                          country="KR", state="", city="", key_size=2048, 
                          validity_days=365):
        """Create a new certificate"""
        data = {
            "common_name": common_name,
            "email": email,
            "organization": organization,
            "country": country,
            "state": state,
            "city": city,
            "key_size": key_size,
            "validity_days": validity_days
        }
        
        try:
            response = requests.post(f"{self.api_url}/certificates", json=data)
            response.raise_for_status()
            return response.json()
        except requests.exceptions.RequestException as e:
            print(f"Error creating certificate: {e}")
            return None
    
    def list_certificates(self):
        """List all certificates"""
        try:
            response = requests.get(f"{self.api_url}/certificates")
            response.raise_for_status()
            return response.json()
        except requests.exceptions.RequestException as e:
            print(f"Error listing certificates: {e}")
            return None
    
    def get_certificate(self, serial_number):
        """Get certificate details"""
        try:
            response = requests.get(f"{self.api_url}/certificates/{serial_number}")
            response.raise_for_status()
            return response.json()
        except requests.exceptions.RequestException as e:
            print(f"Error getting certificate: {e}")
            return None
    
    def revoke_certificate(self, serial_number):
        """Revoke a certificate"""
        try:
            response = requests.delete(f"{self.api_url}/certificates/{serial_number}")
            response.raise_for_status()
            return response.json()
        except requests.exceptions.RequestException as e:
            print(f"Error revoking certificate: {e}")
            return None
    
    def download_ca_certificate(self, filename="ca.crt"):
        """Download CA certificate"""
        try:
            response = requests.get(f"{self.api_url}/ca")
            response.raise_for_status()
            with open(filename, 'wb') as f:
                f.write(response.content)
            print(f"CA certificate saved to {filename}")
            return True
        except requests.exceptions.RequestException as e:
            print(f"Error downloading CA certificate: {e}")
            return False
    
    def download_crl(self, filename="ca.crl"):
        """Download Certificate Revocation List"""
        try:
            response = requests.get(f"{self.api_url}/crl")
            response.raise_for_status()
            with open(filename, 'wb') as f:
                f.write(response.content)
            print(f"CRL saved to {filename}")
            return True
        except requests.exceptions.RequestException as e:
            print(f"Error downloading CRL: {e}")
            return False

def format_timestamp(timestamp):
    """Format Unix timestamp to readable date"""
    return datetime.fromtimestamp(timestamp).strftime('%Y-%m-%d %H:%M:%S')

def main():
    if len(sys.argv) < 2:
        print("Usage: python3 client_example.py <command> [options]")
        print("Commands:")
        print("  create <common_name> [email] [organization] - Create certificate")
        print("  list                                      - List certificates")
        print("  get <serial_number>                       - Get certificate details")
        print("  revoke <serial_number>                    - Revoke certificate")
        print("  download-ca                               - Download CA certificate")
        print("  download-crl                              - Download CRL")
        return
    
    client = CAClient()
    command = sys.argv[1]
    
    if command == "create":
        if len(sys.argv) < 3:
            print("Error: Common name required")
            return
        
        common_name = sys.argv[2]
        email = sys.argv[3] if len(sys.argv) > 3 else ""
        organization = sys.argv[4] if len(sys.argv) > 4 else ""
        
        print(f"Creating certificate for {common_name}...")
        result = client.create_certificate(common_name, email, organization)
        
        if result:
            print("Certificate created successfully!")
            print(f"Certificate path: {result.get('cert_path', 'N/A')}")
            print(f"Key path: {result.get('key_path', 'N/A')}")
        else:
            print("Failed to create certificate")
    
    elif command == "list":
        print("Fetching certificate list...")
        result = client.list_certificates()
        
        if result and 'certificates' in result:
            certificates = result['certificates']
            if not certificates:
                print("No certificates found")
            else:
                print(f"Found {len(certificates)} certificates:")
                print("-" * 80)
                for cert in certificates:
                    print(f"Serial: {cert['serial_number']}")
                    print(f"CN: {cert['common_name']}")
                    print(f"Email: {cert['email']}")
                    print(f"Status: {cert['status']}")
                    print(f"Created: {format_timestamp(cert['created_at'])}")
                    print(f"Expires: {format_timestamp(cert['expires_at'])}")
                    print("-" * 80)
        else:
            print("Failed to fetch certificates")
    
    elif command == "get":
        if len(sys.argv) < 3:
            print("Error: Serial number required")
            return
        
        serial_number = sys.argv[2]
        print(f"Fetching certificate {serial_number}...")
        result = client.get_certificate(serial_number)
        
        if result:
            print("Certificate details:")
            print(f"Serial: {result['serial_number']}")
            print(f"CN: {result['common_name']}")
            print(f"Email: {result['email']}")
            print(f"Status: {result['status']}")
            print(f"Created: {format_timestamp(result['created_at'])}")
            print(f"Expires: {format_timestamp(result['expires_at'])}")
        else:
            print("Certificate not found")
    
    elif command == "revoke":
        if len(sys.argv) < 3:
            print("Error: Serial number required")
            return
        
        serial_number = sys.argv[2]
        print(f"Revoking certificate {serial_number}...")
        result = client.revoke_certificate(serial_number)
        
        if result:
            print("Certificate revoked successfully!")
        else:
            print("Failed to revoke certificate")
    
    elif command == "download-ca":
        print("Downloading CA certificate...")
        client.download_ca_certificate()
    
    elif command == "download-crl":
        print("Downloading CRL...")
        client.download_crl()
    
    else:
        print(f"Unknown command: {command}")

if __name__ == "__main__":
    main()
