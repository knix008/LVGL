#!/usr/bin/env python3
"""
Step-CA Manager - Main Entry Point
A GUI application for managing Step-CA Certificate Authority
"""

import gi
gi.require_version('Gtk', '3.0')
from gi.repository import Gtk
import sys
from pathlib import Path

# Add src directory to path
sys.path.insert(0, str(Path(__file__).parent))

from gui.main_window import MainWindow


def main():
    """Application entry point"""
    try:
        app = MainWindow()
        app.show_all()
        Gtk.main()
    except KeyboardInterrupt:
        print("\nApplication terminated by user")
        sys.exit(0)
    except Exception as e:
        print(f"Error starting application: {e}", file=sys.stderr)
        sys.exit(1)


if __name__ == "__main__":
    main()
