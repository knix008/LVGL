#!/usr/bin/env python3
"""
HTTPS Client GUI - Main Entry Point
"""

import gi
gi.require_version('Gtk', '3.0')
from gi.repository import Gtk
import sys
import os

# Add src directory to path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

from gui.main_window import MainWindow


def main():
    """Main function"""
    window = MainWindow()
    window.connect("destroy", Gtk.main_quit)
    window.show_all()
    Gtk.main()


if __name__ == "__main__":
    main()
