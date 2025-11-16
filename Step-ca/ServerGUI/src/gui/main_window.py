#!/usr/bin/env python3
"""
Main window for HTTPS Server GUI
"""

import gi
gi.require_version('Gtk', '3.0')
from gi.repository import Gtk, Gdk, GLib, Pango
import time

from core.server import HTTPSServer
from core.utils import (get_default_paths, validate_address, format_uptime,
                        save_server_config, load_server_config)


class MainWindow(Gtk.Window):
    """Main application window"""

    def __init__(self):
        super().__init__(title="HTTPS Server")
        self.set_default_size(900, 700)
        self.set_border_width(10)

        # Initialize server
        paths = get_default_paths()
        saved_config = load_server_config()

        if saved_config:
            self.server = HTTPSServer(
                server_path=saved_config.get("server_path", paths["server_path"]),
                cert_path=saved_config.get("cert_path", paths["cert_path"]),
                key_path=saved_config.get("key_path", paths["key_path"]),
                ca_path=saved_config.get("ca_path", paths["ca_path"]),
                address=saved_config.get("address", paths["default_address"])
            )
        else:
            self.server = HTTPSServer(
                server_path=paths["server_path"],
                cert_path=paths["cert_path"],
                key_path=paths["key_path"],
                ca_path=paths["ca_path"],
                address=paths["default_address"]
            )

        self.start_time = None
        self.log_lines = []

        # Setup log callback
        self.server.set_log_callback(self.on_log_line)

        # Create UI
        self.create_ui()

        # Start status updates
        GLib.timeout_add_seconds(1, self.update_status)
        GLib.timeout_add(100, self.poll_logs)

    def create_ui(self):
        """Create user interface"""
        main_box = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=10)
        self.add(main_box)

        # Header
        header_box = self.create_header()
        main_box.pack_start(header_box, False, False, 0)

        # Separator
        main_box.pack_start(Gtk.Separator(), False, False, 0)

        # Status section
        status_frame = self.create_status_section()
        main_box.pack_start(status_frame, False, False, 0)

        # Logs section
        logs_frame = self.create_logs_section()
        main_box.pack_start(logs_frame, True, True, 0)

        # Control buttons
        button_box = self.create_button_bar()
        main_box.pack_start(button_box, False, False, 0)

    def create_header(self):
        """Create header"""
        box = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL, spacing=10)

        # Title
        title = Gtk.Label()
        title.set_markup("<big><b>HTTPS Server Control Panel</b></big>")
        box.pack_start(title, False, False, 0)

        # Spacer
        box.pack_start(Gtk.Label(), True, True, 0)

        # Status indicator
        self.status_indicator = Gtk.Label()
        self.status_indicator.set_markup("<span color='gray'>● Stopped</span>")
        box.pack_start(self.status_indicator, False, False, 0)

        return box

    def create_status_section(self):
        """Create status display section"""
        frame = Gtk.Frame(label="Server Status")
        frame.set_border_width(5)

        box = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=10)
        box.set_border_width(10)
        frame.add(box)

        # Grid for status fields
        grid = Gtk.Grid()
        grid.set_row_spacing(8)
        grid.set_column_spacing(15)

        row = 0

        # Address
        label = Gtk.Label(label="Address:")
        label.set_xalign(0)
        label.set_markup("<b>Address:</b>")
        grid.attach(label, 0, row, 1, 1)

        self.address_label = Gtk.Label(label=self.server.address)
        self.address_label.set_xalign(0)
        self.address_label.set_selectable(True)
        grid.attach(self.address_label, 1, row, 1, 1)

        row += 1

        # State
        label = Gtk.Label(label="State:")
        label.set_xalign(0)
        label.set_markup("<b>State:</b>")
        grid.attach(label, 0, row, 1, 1)

        self.state_label = Gtk.Label(label="Stopped")
        self.state_label.set_xalign(0)
        grid.attach(self.state_label, 1, row, 1, 1)

        row += 1

        # Uptime
        label = Gtk.Label(label="Uptime:")
        label.set_xalign(0)
        label.set_markup("<b>Uptime:</b>")
        grid.attach(label, 0, row, 1, 1)

        self.uptime_label = Gtk.Label(label="N/A")
        self.uptime_label.set_xalign(0)
        grid.attach(self.uptime_label, 1, row, 1, 1)

        row += 1

        # PID
        label = Gtk.Label(label="PID:")
        label.set_xalign(0)
        label.set_markup("<b>PID:</b>")
        grid.attach(label, 0, row, 1, 1)

        self.pid_label = Gtk.Label(label="N/A")
        self.pid_label.set_xalign(0)
        grid.attach(self.pid_label, 1, row, 1, 1)

        row += 1

        # Memory
        label = Gtk.Label(label="Memory:")
        label.set_xalign(0)
        label.set_markup("<b>Memory:</b>")
        grid.attach(label, 0, row, 1, 1)

        self.memory_label = Gtk.Label(label="N/A")
        self.memory_label.set_xalign(0)
        grid.attach(self.memory_label, 1, row, 1, 1)

        row += 1

        # Certificates status
        label = Gtk.Label(label="Certificates:")
        label.set_xalign(0)
        label.set_markup("<b>Certificates:</b>")
        grid.attach(label, 0, row, 1, 1)

        self.certs_label = Gtk.Label(label="Checking...")
        self.certs_label.set_xalign(0)
        grid.attach(self.certs_label, 1, row, 1, 1)

        box.pack_start(grid, False, False, 0)

        return frame

    def create_logs_section(self):
        """Create logs display section"""
        frame = Gtk.Frame(label="Server Logs")
        frame.set_border_width(5)

        box = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=5)
        box.set_border_width(10)
        frame.add(box)

        # Toolbar for logs
        toolbar = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL, spacing=5)

        clear_button = Gtk.Button(label="Clear Logs")
        clear_button.connect("clicked", self.on_clear_logs_clicked)
        toolbar.pack_start(clear_button, False, False, 0)

        # Autoscroll checkbox
        self.autoscroll_check = Gtk.CheckButton(label="Auto-scroll")
        self.autoscroll_check.set_active(True)
        toolbar.pack_start(self.autoscroll_check, False, False, 0)

        # Spacer
        toolbar.pack_start(Gtk.Label(), True, True, 0)

        # Log count
        self.log_count_label = Gtk.Label(label="0 lines")
        toolbar.pack_start(self.log_count_label, False, False, 0)

        box.pack_start(toolbar, False, False, 0)

        # Scrolled window for logs
        scrolled = Gtk.ScrolledWindow()
        scrolled.set_policy(Gtk.PolicyType.AUTOMATIC, Gtk.PolicyType.AUTOMATIC)

        self.logs_text = Gtk.TextView()
        self.logs_text.set_editable(False)
        self.logs_text.set_wrap_mode(Gtk.WrapMode.WORD_CHAR)
        self.logs_buffer = self.logs_text.get_buffer()

        # Set monospace font
        font_desc = Pango.FontDescription("monospace 9")
        self.logs_text.modify_font(font_desc)

        scrolled.add(self.logs_text)
        box.pack_start(scrolled, True, True, 0)

        return frame

    def create_button_bar(self):
        """Create control button bar"""
        box = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL, spacing=10)

        # Start button
        self.start_button = Gtk.Button(label="Start Server")
        self.start_button.set_size_request(120, 40)
        self.start_button.connect("clicked", self.on_start_clicked)
        style_context = self.start_button.get_style_context()
        style_context.add_class("suggested-action")
        box.pack_start(self.start_button, False, False, 0)

        # Stop button
        self.stop_button = Gtk.Button(label="Stop Server")
        self.stop_button.set_size_request(120, 40)
        self.stop_button.set_sensitive(False)
        self.stop_button.connect("clicked", self.on_stop_clicked)
        style_context = self.stop_button.get_style_context()
        style_context.add_class("destructive-action")
        box.pack_start(self.stop_button, False, False, 0)

        # Restart button
        self.restart_button = Gtk.Button(label="Restart Server")
        self.restart_button.set_size_request(120, 40)
        self.restart_button.set_sensitive(False)
        self.restart_button.connect("clicked", self.on_restart_clicked)
        box.pack_start(self.restart_button, False, False, 0)

        # Spacer
        box.pack_start(Gtk.Label(), True, True, 0)

        # Settings button
        settings_button = Gtk.Button(label="Settings")
        settings_button.connect("clicked", self.on_settings_clicked)
        box.pack_start(settings_button, False, False, 0)

        return box

    def on_start_clicked(self, button):
        """Handle start button click"""
        self.add_log("Starting server...")

        success, message = self.server.start()

        if success:
            self.add_log(f"✓ {message}")
            self.start_time = time.time()
            self.update_button_states(running=True)
        else:
            self.add_log(f"✗ {message}")
            self.show_error("Start Failed", message)

    def on_stop_clicked(self, button):
        """Handle stop button click"""
        self.add_log("Stopping server...")

        success, message = self.server.stop()

        if success:
            self.add_log(f"✓ {message}")
            self.start_time = None
            self.update_button_states(running=False)
        else:
            self.add_log(f"✗ {message}")
            self.show_error("Stop Failed", message)

    def on_restart_clicked(self, button):
        """Handle restart button click"""
        self.add_log("Restarting server...")

        success, message = self.server.restart()

        if success:
            self.add_log(f"✓ {message}")
            self.start_time = time.time()
        else:
            self.add_log(f"✗ {message}")
            self.show_error("Restart Failed", message)

    def on_clear_logs_clicked(self, button):
        """Handle clear logs button click"""
        self.logs_buffer.set_text("")
        self.log_lines = []
        self.log_count_label.set_text("0 lines")

    def on_settings_clicked(self, button):
        """Handle settings button click"""
        dialog = SettingsDialog(self, self.server)
        response = dialog.run()

        if response == Gtk.ResponseType.OK:
            settings = dialog.get_settings()

            # Update server configuration
            self.server.server_path = settings["server_path"]
            self.server.cert_path = settings["cert_path"]
            self.server.key_path = settings["key_path"]
            self.server.ca_path = settings["ca_path"]
            self.server.address = settings["address"]

            # Save configuration
            save_server_config(settings)

            # Update UI
            self.address_label.set_text(self.server.address)
            self.add_log("Settings updated")

        dialog.destroy()

    def update_status(self):
        """Update status display"""
        status = self.server.get_status_summary()

        # Update state and indicator
        if status["running"]:
            self.status_indicator.set_markup("<span color='green'>● Running</span>")
            self.state_label.set_text("Running")

            # Update uptime
            if self.start_time:
                uptime = time.time() - self.start_time
                self.uptime_label.set_text(format_uptime(uptime))

            # Update PID and resource info
            if "pid" in status:
                self.pid_label.set_text(str(status["pid"]))

            if "memory_mb" in status:
                self.memory_label.set_text(f"{status['memory_mb']:.1f} MB")

        else:
            self.status_indicator.set_markup("<span color='gray'>● Stopped</span>")
            self.state_label.set_text("Stopped")
            self.uptime_label.set_text("N/A")
            self.pid_label.set_text("N/A")
            self.memory_label.set_text("N/A")

        # Update certificates status
        certs = status["certificates"]
        if all(certs.values()):
            self.certs_label.set_markup("<span color='green'>✓ Ready</span>")
        else:
            missing = [k for k, v in certs.items() if not v]
            self.certs_label.set_markup(f"<span color='red'>✗ Missing: {', '.join(missing)}</span>")

        # Update button states
        self.update_button_states(status["running"])

        return True  # Continue timeout

    def update_button_states(self, running):
        """Update button enabled states"""
        self.start_button.set_sensitive(not running)
        self.stop_button.set_sensitive(running)
        self.restart_button.set_sensitive(running)

    def poll_logs(self):
        """Poll for new log messages"""
        if self.server.is_running():
            line = self.server.read_log_line()
            # Log callback will handle adding to display

        return True  # Continue timeout

    def on_log_line(self, line):
        """Handle new log line"""
        GLib.idle_add(self.add_log, line.strip())

    def add_log(self, message):
        """Add log message to display"""
        timestamp = time.strftime("%H:%M:%S")
        log_line = f"[{timestamp}] {message}"

        self.log_lines.append(log_line)

        # Keep only last 1000 lines
        if len(self.log_lines) > 1000:
            self.log_lines = self.log_lines[-1000:]

        # Update display
        self.logs_buffer.set_text("\n".join(self.log_lines))

        # Update count
        self.log_count_label.set_text(f"{len(self.log_lines)} lines")

        # Auto-scroll if enabled
        if self.autoscroll_check.get_active():
            mark = self.logs_buffer.get_insert()
            self.logs_text.scroll_to_mark(mark, 0.0, True, 0.0, 1.0)

    def show_error(self, title, message):
        """Show error dialog"""
        dialog = Gtk.MessageDialog(
            parent=self,
            flags=0,
            message_type=Gtk.MessageType.ERROR,
            buttons=Gtk.ButtonsType.OK,
            text=title
        )
        dialog.format_secondary_text(message)
        dialog.run()
        dialog.destroy()


class SettingsDialog(Gtk.Dialog):
    """Settings dialog"""

    def __init__(self, parent, server):
        super().__init__(title="Server Settings", parent=parent, flags=0)
        self.add_buttons(
            Gtk.STOCK_CANCEL, Gtk.ResponseType.CANCEL,
            Gtk.STOCK_OK, Gtk.ResponseType.OK
        )

        self.server = server
        self.set_default_size(500, 450)

        box = self.get_content_area()
        box.set_spacing(10)
        box.set_border_width(10)

        # Server executable path
        label = Gtk.Label(label="Server Executable:")
        label.set_xalign(0)
        box.pack_start(label, False, False, 0)

        self.server_entry = Gtk.Entry()
        self.server_entry.set_text(server.server_path or "")
        box.pack_start(self.server_entry, False, False, 0)

        # Address
        label = Gtk.Label(label="Listening Address:")
        label.set_xalign(0)
        box.pack_start(label, False, False, 0)

        self.address_entry = Gtk.Entry()
        self.address_entry.set_text(server.address or "")
        self.address_entry.set_placeholder_text("https://0.0.0.0:8443")
        box.pack_start(self.address_entry, False, False, 0)

        # Certificate path
        label = Gtk.Label(label="Server Certificate:")
        label.set_xalign(0)
        box.pack_start(label, False, False, 0)

        self.cert_entry = Gtk.Entry()
        self.cert_entry.set_text(server.cert_path or "")
        box.pack_start(self.cert_entry, False, False, 0)

        # Key path
        label = Gtk.Label(label="Server Key:")
        label.set_xalign(0)
        box.pack_start(label, False, False, 0)

        self.key_entry = Gtk.Entry()
        self.key_entry.set_text(server.key_path or "")
        box.pack_start(self.key_entry, False, False, 0)

        # CA path
        label = Gtk.Label(label="Root CA Certificate:")
        label.set_xalign(0)
        box.pack_start(label, False, False, 0)

        self.ca_entry = Gtk.Entry()
        self.ca_entry.set_text(server.ca_path or "")
        box.pack_start(self.ca_entry, False, False, 0)

        self.show_all()

    def get_settings(self):
        """Get settings from dialog"""
        return {
            "server_path": self.server_entry.get_text(),
            "address": self.address_entry.get_text(),
            "cert_path": self.cert_entry.get_text(),
            "key_path": self.key_entry.get_text(),
            "ca_path": self.ca_entry.get_text()
        }
