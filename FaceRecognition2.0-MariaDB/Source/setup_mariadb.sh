#!/bin/bash

# MariaDB Setup Script for FaceRecognition2.0
# This script helps set up MariaDB server for the project

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

print_status() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

echo "=== MariaDB Setup for FaceRecognition2.0 ==="
echo ""

# Check if MariaDB is already installed
if command -v mariadb &> /dev/null; then
    print_success "MariaDB is already installed"
    MARIADB_INSTALLED=true
else
    print_status "MariaDB is not installed. Installing..."
    MARIADB_INSTALLED=false
fi

# Install MariaDB if not already installed
if [ "$MARIADB_INSTALLED" = false ]; then
    print_status "Installing MariaDB server..."
    
    # Update package list
    sudo apt update
    
    # Install MariaDB server
    sudo apt install -y mariadb-server
    
    print_success "MariaDB server installed successfully"
fi

# Start MariaDB service
print_status "Starting MariaDB service..."
sudo systemctl start mariadb
sudo systemctl enable mariadb

# Check if MariaDB is running
if sudo systemctl is-active --quiet mariadb; then
    print_success "MariaDB service is running"
else
    print_error "Failed to start MariaDB service"
    exit 1
fi

# Secure the installation
print_status "Securing MariaDB installation..."
print_warning "You will be prompted to set a root password and answer security questions."
print_warning "For development, you can use an empty password by pressing Enter."
echo ""

# Run mysql_secure_installation
sudo mysql_secure_installation

# Create test database and user
print_status "Setting up test database..."

# Create a temporary SQL file
cat > /tmp/setup_mariadb.sql << EOF
-- Create test database
CREATE DATABASE IF NOT EXISTS test_db;

-- Create test user (optional, for better security)
-- CREATE USER IF NOT EXISTS 'testuser'@'localhost' IDENTIFIED BY 'testpass';
-- GRANT ALL PRIVILEGES ON test_db.* TO 'testuser'@'localhost';
-- FLUSH PRIVILEGES;

-- Use test database
USE test_db;

-- Create test table
CREATE TABLE IF NOT EXISTS users (
    id INT AUTO_INCREMENT PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    email VARCHAR(100)
);

-- Insert test data
INSERT INTO users (name, email) VALUES 
    ('John Doe', 'john@example.com'),
    ('Jane Smith', 'jane@example.com')
ON DUPLICATE KEY UPDATE name=name;

-- Show created data
SELECT * FROM users;
EOF

# Execute the SQL file
print_status "Creating test database and tables..."
sudo mysql < /tmp/setup_mariadb.sql

# Clean up
rm -f /tmp/setup_mariadb.sql

print_success "MariaDB setup completed successfully!"
echo ""
print_status "MariaDB server is now running and ready for use."
print_status "Default connection settings:"
echo "  - Host: localhost"
echo "  - Port: 3306"
echo "  - Database: test_db"
echo "  - User: root (or your configured user)"
echo "  - Password: (the one you set during setup)"
echo ""
print_status "You can now run the FaceRecognition2.0 application with:"
echo "  cd .."
echo "  ./run.sh"
echo ""
print_status "The application will automatically connect to the MariaDB server."
