#!/usr/bin/env python3
"""
Dashboard Tab - CA Status and Information
"""

import gi
gi.require_version('Gtk', '3.0')
from gi.repository import Gtk, Pango


class DashboardTab(Gtk.Box):
    """Dashboard tab showing CA status and information"""

    def __init__(self, stepca):
        super().__init__(orientation=Gtk.Orientation.VERTICAL, spacing=10)
        self.stepca = stepca
        self.set_margin_top(10)
        self.set_margin_bottom(10)
        self.set_margin_start(10)
        self.set_margin_end(10)

        # Create title
        title = Gtk.Label()
        title.set_markup("<big><b>Certificate Authority Dashboard</b></big>")
        title.set_halign(Gtk.Align.START)
        self.pack_start(title, False, False, 0)

        # Create status section
        self.create_status_section()

        # Create info section
        self.create_info_section()

        # Create CA config section
        self.create_config_section()

    def create_status_section(self):
        """Create CA status section"""
        frame = Gtk.Frame(label="CA Status")
        frame.set_margin_top(10)
        self.pack_start(frame, False, False, 0)

        grid = Gtk.Grid()
        grid.set_margin_top(10)
        grid.set_margin_bottom(10)
        grid.set_margin_start(10)
        grid.set_margin_end(10)
        grid.set_row_spacing(10)
        grid.set_column_spacing(20)
        frame.add(grid)

        # Step CLI status
        label = Gtk.Label(label="Step CLI:")
        label.set_halign(Gtk.Align.END)
        grid.attach(label, 0, 0, 1, 1)

        self.step_cli_status = Gtk.Label()
        self.step_cli_status.set_halign(Gtk.Align.START)
        grid.attach(self.step_cli_status, 1, 0, 1, 1)

        # Step-CA status
        label = Gtk.Label(label="Step-CA:")
        label.set_halign(Gtk.Align.END)
        grid.attach(label, 0, 1, 1, 1)

        self.step_ca_status = Gtk.Label()
        self.step_ca_status.set_halign(Gtk.Align.START)
        grid.attach(self.step_ca_status, 1, 1, 1, 1)

        # CA Service status
        label = Gtk.Label(label="CA Service:")
        label.set_halign(Gtk.Align.END)
        grid.attach(label, 0, 2, 1, 1)

        self.ca_service_status = Gtk.Label()
        self.ca_service_status.set_halign(Gtk.Align.START)
        grid.attach(self.ca_service_status, 1, 2, 1, 1)

    def create_info_section(self):
        """Create CA info section"""
        frame = Gtk.Frame(label="CA Information")
        frame.set_margin_top(10)
        self.pack_start(frame, False, False, 0)

        grid = Gtk.Grid()
        grid.set_margin_top(10)
        grid.set_margin_bottom(10)
        grid.set_margin_start(10)
        grid.set_margin_end(10)
        grid.set_row_spacing(10)
        grid.set_column_spacing(20)
        frame.add(grid)

        # CA URL
        label = Gtk.Label(label="CA URL:")
        label.set_halign(Gtk.Align.END)
        grid.attach(label, 0, 0, 1, 1)

        self.ca_url_label = Gtk.Label(label=self.stepca.ca_url or "Not configured")
        self.ca_url_label.set_halign(Gtk.Align.START)
        self.ca_url_label.set_selectable(True)
        grid.attach(self.ca_url_label, 1, 0, 1, 1)

        # CA Fingerprint
        label = Gtk.Label(label="Fingerprint:")
        label.set_halign(Gtk.Align.END)
        grid.attach(label, 0, 1, 1, 1)

        self.fingerprint_label = Gtk.Label()
        self.fingerprint_label.set_halign(Gtk.Align.START)
        self.fingerprint_label.set_selectable(True)
        self.fingerprint_label.set_line_wrap(True)
        grid.attach(self.fingerprint_label, 1, 1, 1, 1)

    def create_config_section(self):
        """Create CA configuration section"""
        frame = Gtk.Frame(label="CA Configuration")
        frame.set_margin_top(10)
        self.pack_start(frame, True, True, 0)

        scrolled = Gtk.ScrolledWindow()
        scrolled.set_policy(Gtk.PolicyType.AUTOMATIC, Gtk.PolicyType.AUTOMATIC)
        frame.add(scrolled)

        self.config_view = Gtk.TextView()
        self.config_view.set_editable(False)
        self.config_view.set_cursor_visible(False)
        self.config_view.set_wrap_mode(Gtk.WrapMode.WORD)
        self.config_view.set_margin_top(10)
        self.config_view.set_margin_bottom(10)
        self.config_view.set_margin_start(10)
        self.config_view.set_margin_end(10)

        # Use monospace font for config
        font_desc = Pango.FontDescription.from_string("Monospace 10")
        self.config_view.modify_font(font_desc)

        scrolled.add(self.config_view)

    def refresh(self):
        """Refresh dashboard data"""
        # Update Step CLI status
        if self.stepca.check_step_installed():
            version = self.stepca.get_step_version()
            self.step_cli_status.set_markup(f"<span color='green'>✓ Installed ({version})</span>")
        else:
            self.step_cli_status.set_markup("<span color='red'>✗ Not found</span>")

        # Update Step-CA status
        if self.stepca.check_step_ca_installed():
            version = self.stepca.get_step_ca_version()
            self.step_ca_status.set_markup(f"<span color='green'>✓ Installed ({version})</span>")
        else:
            self.step_ca_status.set_markup("<span color='red'>✗ Not found</span>")

        # Update CA service status
        if self.stepca.is_ca_running():
            self.ca_service_status.set_markup("<span color='green'>✓ Running</span>")
        else:
            self.ca_service_status.set_markup("<span color='orange'>✗ Stopped</span>")

        # Update fingerprint
        fingerprint = self.stepca.get_ca_fingerprint()
        if fingerprint:
            self.fingerprint_label.set_text(fingerprint)
        else:
            self.fingerprint_label.set_markup("<span color='gray'>Not available</span>")

        # Update CA URL
        self.ca_url_label.set_text(self.stepca.ca_url or "Not configured")

        # Update CA config
        config = self.stepca.get_ca_config()
        if config:
            import json
            config_text = json.dumps(config, indent=2)
            self.config_view.get_buffer().set_text(config_text)
        else:
            self.config_view.get_buffer().set_text("CA not initialized or config not found")
