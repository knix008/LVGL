#!/usr/bin/env python3
"""
Main Window for Step-CA Manager
"""

import gi
gi.require_version('Gtk', '3.0')
from gi.repository import Gtk, GLib
import sys
from pathlib import Path

# Add parent directory to path for imports
sys.path.insert(0, str(Path(__file__).parent.parent))

from core.stepca import StepCA
from core.utils import get_default_step_paths
from gui.dashboard import DashboardTab
from gui.certificates import CertificatesTab
from gui.provisioners import ProvisionersTab
from gui.settings import SettingsTab


class MainWindow(Gtk.Window):
    """Main application window"""

    def __init__(self):
        super().__init__(title="Step-CA Manager")

        # Initialize Step-CA interface
        paths = get_default_step_paths()
        self.stepca = StepCA(
            step_path=paths['step'],
            step_ca_path=paths['step_ca']
        )

        # Set window properties
        self.set_default_size(900, 600)
        self.set_position(Gtk.WindowPosition.CENTER)

        # Create main container
        main_box = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=0)
        self.add(main_box)

        # Create header bar
        header_bar = self.create_header_bar()
        self.set_titlebar(header_bar)

        # Create notebook (tabs)
        self.notebook = Gtk.Notebook()
        self.notebook.set_tab_pos(Gtk.PositionType.TOP)
        main_box.pack_start(self.notebook, True, True, 0)

        # Create tabs
        self.dashboard_tab = DashboardTab(self.stepca)
        self.certificates_tab = CertificatesTab(self.stepca)
        self.provisioners_tab = ProvisionersTab(self.stepca)
        self.settings_tab = SettingsTab(self.stepca, self.on_settings_changed)

        # Add tabs to notebook
        self.notebook.append_page(
            self.dashboard_tab,
            Gtk.Label(label="Dashboard")
        )
        self.notebook.append_page(
            self.certificates_tab,
            Gtk.Label(label="Certificates")
        )
        self.notebook.append_page(
            self.provisioners_tab,
            Gtk.Label(label="Provisioners")
        )
        self.notebook.append_page(
            self.settings_tab,
            Gtk.Label(label="Settings")
        )

        # Create status bar
        self.statusbar = Gtk.Statusbar()
        self.statusbar_context = self.statusbar.get_context_id("main")
        main_box.pack_end(self.statusbar, False, False, 0)

        # Connect signals
        self.connect("delete-event", self.on_quit)
        self.notebook.connect("switch-page", self.on_tab_switched)

        # Start periodic updates
        GLib.timeout_add_seconds(5, self.update_status)
        self.update_status()

    def create_header_bar(self):
        """Create the header bar"""
        header_bar = Gtk.HeaderBar()
        header_bar.set_show_close_button(True)
        header_bar.props.title = "Step-CA Manager"
        header_bar.props.subtitle = "Certificate Authority Management"

        # Add refresh button
        refresh_button = Gtk.Button()
        refresh_icon = Gtk.Image.new_from_icon_name(
            "view-refresh-symbolic",
            Gtk.IconSize.BUTTON
        )
        refresh_button.add(refresh_icon)
        refresh_button.set_tooltip_text("Refresh")
        refresh_button.connect("clicked", self.on_refresh_clicked)
        header_bar.pack_start(refresh_button)

        return header_bar

    def update_status(self):
        """Update status bar with CA status"""
        if self.stepca.is_ca_running():
            self.statusbar.push(
                self.statusbar_context,
                "✓ Step-CA is running"
            )
        else:
            self.statusbar.push(
                self.statusbar_context,
                "✗ Step-CA is not running"
            )

        # Continue periodic updates
        return True

    def on_refresh_clicked(self, button):
        """Handle refresh button click"""
        # Refresh current tab
        current_page = self.notebook.get_current_page()

        if current_page == 0:
            self.dashboard_tab.refresh()
        elif current_page == 1:
            self.certificates_tab.refresh()
        elif current_page == 2:
            self.provisioners_tab.refresh()

        self.update_status()

    def on_tab_switched(self, notebook, page, page_num):
        """Handle tab switch"""
        # Refresh the newly active tab
        if page_num == 0:
            self.dashboard_tab.refresh()
        elif page_num == 1:
            self.certificates_tab.refresh()
        elif page_num == 2:
            self.provisioners_tab.refresh()

    def on_settings_changed(self):
        """Handle settings changes"""
        # Refresh all tabs
        self.dashboard_tab.refresh()
        self.certificates_tab.refresh()
        self.provisioners_tab.refresh()
        self.update_status()

    def on_quit(self, widget, event):
        """Handle application quit"""
        Gtk.main_quit()


def main():
    """Application entry point"""
    app = MainWindow()
    app.show_all()
    Gtk.main()


if __name__ == "__main__":
    main()
