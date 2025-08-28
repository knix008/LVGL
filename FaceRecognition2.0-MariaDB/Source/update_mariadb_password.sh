#!/bin/bash

# Script to update MariaDB password in the source code
echo "=== MariaDB Password Update Script ==="
echo ""
echo "This script will help you update the MariaDB password in the source code."
echo ""

# Prompt for password
read -s -p "Enter your MariaDB root password: " MARIADB_PASSWORD
echo ""

if [ -z "$MARIADB_PASSWORD" ]; then
    echo "Error: Password cannot be empty"
    exit 1
fi

# Escape special characters in password for sed
ESCAPED_PASSWORD=$(echo "$MARIADB_PASSWORD" | sed 's/[[\.*^$()+?{|]/\\&/g')

# Update the password in mariadb_test.c
echo "Updating password in mariadb_test.c..."
sed -i "s/#define MARIADB_PASSWORD \"your_password_here\"/#define MARIADB_PASSWORD \"$ESCAPED_PASSWORD\"/" src/mariadb_test.c

if [ $? -eq 0 ]; then
    echo "✅ Password updated successfully!"
    echo ""
    echo "You can now rebuild and run the application:"
    echo "  cd build"
    echo "  make"
    echo "  ./main"
else
    echo "❌ Failed to update password"
    exit 1
fi
