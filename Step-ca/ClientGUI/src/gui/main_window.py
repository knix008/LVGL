#!/usr/bin/env python3
"""
Main window for HTTPS Client GUI
"""

import gi
gi.require_version('Gtk', '3.0')
from gi.repository import Gtk, Gdk, GLib
import threading
import json

from core.client import HTTPSClient
from core.utils import get_default_paths, validate_url, validate_json, save_request_history, load_request_history


class MainWindow(Gtk.Window):
    """Main application window"""

    def __init__(self):
        super().__init__(title="HTTPS Client")
        self.set_default_size(900, 700)
        self.set_border_width(10)

        # Initialize client
        paths = get_default_paths()
        self.client = HTTPSClient(
            client_path=paths["client_path"],
            cert_path=paths["cert_path"],
            key_path=paths["key_path"],
            ca_path=paths["ca_path"]
        )

        # Create UI
        self.create_ui()

        # Load request history
        self.load_history()

        # Check client status
        GLib.timeout_add_seconds(1, self.update_status)

    def create_ui(self):
        """Create user interface"""
        main_box = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=10)
        self.add(main_box)

        # Header with status
        header_box = self.create_header()
        main_box.pack_start(header_box, False, False, 0)

        # Separator
        main_box.pack_start(Gtk.Separator(), False, False, 0)

        # Request section
        request_frame = self.create_request_section()
        main_box.pack_start(request_frame, False, False, 0)

        # Response section
        response_frame = self.create_response_section()
        main_box.pack_start(response_frame, True, True, 0)

        # Bottom button bar
        button_box = self.create_button_bar()
        main_box.pack_start(button_box, False, False, 0)

    def create_header(self):
        """Create header with status"""
        box = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL, spacing=10)

        # Title
        title = Gtk.Label()
        title.set_markup("<big><b>HTTPS Client</b></big>")
        box.pack_start(title, False, False, 0)

        # Spacer
        box.pack_start(Gtk.Label(), True, True, 0)

        # Status indicator
        self.status_label = Gtk.Label()
        self.status_label.set_markup("<span color='gray'>● Not Ready</span>")
        box.pack_start(self.status_label, False, False, 0)

        return box

    def create_request_section(self):
        """Create request configuration section"""
        frame = Gtk.Frame(label="Request")
        frame.set_border_width(5)

        box = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=10)
        box.set_border_width(10)
        frame.add(box)

        # URL row
        url_box = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL, spacing=10)
        url_label = Gtk.Label(label="URL:")
        url_label.set_size_request(80, -1)
        url_label.set_xalign(0)
        url_box.pack_start(url_label, False, False, 0)

        self.url_entry = Gtk.Entry()
        self.url_entry.set_placeholder_text("https://localhost:8443/")
        self.url_entry.set_text("https://localhost:8443/")
        url_box.pack_start(self.url_entry, True, True, 0)

        box.pack_start(url_box, False, False, 0)

        # Method and timeout row
        controls_box = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL, spacing=10)

        # Method
        method_label = Gtk.Label(label="Method:")
        method_label.set_size_request(80, -1)
        method_label.set_xalign(0)
        controls_box.pack_start(method_label, False, False, 0)

        self.method_combo = Gtk.ComboBoxText()
        for method in ["GET", "POST", "PUT", "DELETE"]:
            self.method_combo.append_text(method)
        self.method_combo.set_active(0)
        self.method_combo.connect("changed", self.on_method_changed)
        controls_box.pack_start(self.method_combo, False, False, 0)

        # Spacer
        controls_box.pack_start(Gtk.Label(), True, True, 0)

        # Timeout
        timeout_label = Gtk.Label(label="Timeout (ms):")
        controls_box.pack_start(timeout_label, False, False, 0)

        self.timeout_spin = Gtk.SpinButton()
        self.timeout_spin.set_range(1000, 60000)
        self.timeout_spin.set_increments(1000, 5000)
        self.timeout_spin.set_value(10000)
        controls_box.pack_start(self.timeout_spin, False, False, 0)

        box.pack_start(controls_box, False, False, 0)

        # Data section (for POST/PUT)
        self.data_box = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=5)

        data_label = Gtk.Label(label="Request Body (JSON):")
        data_label.set_xalign(0)
        self.data_box.pack_start(data_label, False, False, 0)

        # Scrolled window for data
        scrolled = Gtk.ScrolledWindow()
        scrolled.set_min_content_height(100)
        scrolled.set_policy(Gtk.PolicyType.AUTOMATIC, Gtk.PolicyType.AUTOMATIC)

        self.data_text = Gtk.TextView()
        self.data_text.set_wrap_mode(Gtk.WrapMode.WORD_CHAR)
        self.data_buffer = self.data_text.get_buffer()
        scrolled.add(self.data_text)

        self.data_box.pack_start(scrolled, True, True, 0)
        self.data_box.set_no_show_all(True)
        self.data_box.hide()

        box.pack_start(self.data_box, False, False, 0)

        return frame

    def create_response_section(self):
        """Create response display section"""
        frame = Gtk.Frame(label="Response")
        frame.set_border_width(5)

        box = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=10)
        box.set_border_width(10)
        frame.add(box)

        # Status line
        self.response_status_label = Gtk.Label(label="No response yet")
        self.response_status_label.set_xalign(0)
        box.pack_start(self.response_status_label, False, False, 0)

        # Notebook for tabs
        notebook = Gtk.Notebook()

        # Body tab
        body_scroll = Gtk.ScrolledWindow()
        body_scroll.set_policy(Gtk.PolicyType.AUTOMATIC, Gtk.PolicyType.AUTOMATIC)

        self.response_body_text = Gtk.TextView()
        self.response_body_text.set_editable(False)
        self.response_body_text.set_wrap_mode(Gtk.WrapMode.WORD_CHAR)
        self.response_body_buffer = self.response_body_text.get_buffer()

        # Set monospace font
        font_desc = Pango.FontDescription("monospace 10")
        self.response_body_text.modify_font(font_desc)

        body_scroll.add(self.response_body_text)
        notebook.append_page(body_scroll, Gtk.Label(label="Body"))

        # Headers tab
        headers_scroll = Gtk.ScrolledWindow()
        headers_scroll.set_policy(Gtk.PolicyType.AUTOMATIC, Gtk.PolicyType.AUTOMATIC)

        self.response_headers_text = Gtk.TextView()
        self.response_headers_text.set_editable(False)
        self.response_headers_text.set_wrap_mode(Gtk.WrapMode.WORD_CHAR)
        self.response_headers_buffer = self.response_headers_text.get_buffer()
        self.response_headers_text.modify_font(font_desc)

        headers_scroll.add(self.response_headers_text)
        notebook.append_page(headers_scroll, Gtk.Label(label="Headers"))

        # Raw tab
        raw_scroll = Gtk.ScrolledWindow()
        raw_scroll.set_policy(Gtk.PolicyType.AUTOMATIC, Gtk.PolicyType.AUTOMATIC)

        self.response_raw_text = Gtk.TextView()
        self.response_raw_text.set_editable(False)
        self.response_raw_text.set_wrap_mode(Gtk.WrapMode.WORD_CHAR)
        self.response_raw_buffer = self.response_raw_text.get_buffer()
        self.response_raw_text.modify_font(font_desc)

        raw_scroll.add(self.response_raw_text)
        notebook.append_page(raw_scroll, Gtk.Label(label="Raw"))

        box.pack_start(notebook, True, True, 0)

        return frame

    def create_button_bar(self):
        """Create bottom button bar"""
        box = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL, spacing=10)

        # Send button (primary)
        self.send_button = Gtk.Button(label="Send Request")
        self.send_button.set_size_request(150, 40)
        self.send_button.connect("clicked", self.on_send_clicked)

        # Style the button
        style_context = self.send_button.get_style_context()
        style_context.add_class("suggested-action")

        box.pack_start(self.send_button, False, False, 0)

        # Spacer
        box.pack_start(Gtk.Label(), True, True, 0)

        # Clear button
        clear_button = Gtk.Button(label="Clear Response")
        clear_button.connect("clicked", self.on_clear_clicked)
        box.pack_start(clear_button, False, False, 0)

        # Settings button
        settings_button = Gtk.Button(label="Settings")
        settings_button.connect("clicked", self.on_settings_clicked)
        box.pack_start(settings_button, False, False, 0)

        return box

    def on_method_changed(self, combo):
        """Handle method change"""
        method = combo.get_active_text()
        if method in ["POST", "PUT"]:
            self.data_box.show()
        else:
            self.data_box.hide()

    def on_send_clicked(self, button):
        """Handle send button click"""
        url = self.url_entry.get_text().strip()
        method = self.method_combo.get_active_text()
        timeout = int(self.timeout_spin.get_value())

        # Validate URL
        if not validate_url(url):
            self.show_error("Invalid URL", "Please enter a valid HTTPS URL")
            return

        # Get data if POST/PUT
        data = None
        if method in ["POST", "PUT"]:
            start_iter = self.data_buffer.get_start_iter()
            end_iter = self.data_buffer.get_end_iter()
            data = self.data_buffer.get_text(start_iter, end_iter, False).strip()

            if data and not validate_json(data):
                self.show_error("Invalid JSON", "Request body must be valid JSON")
                return

        # Disable button and show loading
        self.send_button.set_sensitive(False)
        self.send_button.set_label("Sending...")
        self.response_status_label.set_text("Sending request...")

        # Make request in background thread
        thread = threading.Thread(target=self.send_request, args=(url, method, data, timeout))
        thread.daemon = True
        thread.start()

    def send_request(self, url, method, data, timeout):
        """Send request in background thread"""
        returncode, stdout, stderr = self.client.make_request(url, method, data, timeout)

        # Update UI in main thread
        GLib.idle_add(self.update_response, returncode, stdout, stderr, url, method)

    def update_response(self, returncode, stdout, stderr, url, method):
        """Update response display"""
        # Re-enable button
        self.send_button.set_sensitive(True)
        self.send_button.set_label("Send Request")

        if returncode == 0:
            # Success
            self.response_status_label.set_markup(f"<span color='green'>✓ Success ({method} {url})</span>")

            # Parse response
            response = self.client.parse_response(stdout)

            # Update body
            self.response_body_buffer.set_text(response.get("body", ""))

            # Update headers
            headers_text = ""
            for key, value in response.get("headers", {}).items():
                headers_text += f"{key}: {value}\n"
            self.response_headers_buffer.set_text(headers_text)

            # Update raw
            self.response_raw_buffer.set_text(stdout)

            # Save to history
            save_request_history(url, method)

        else:
            # Error
            self.response_status_label.set_markup(f"<span color='red'>✗ Error ({method} {url})</span>")
            error_msg = stderr if stderr else "Request failed"
            self.response_body_buffer.set_text(error_msg)
            self.response_headers_buffer.set_text("")
            self.response_raw_buffer.set_text(stdout + "\n\n" + stderr)

    def on_clear_clicked(self, button):
        """Handle clear button click"""
        self.response_status_label.set_text("No response yet")
        self.response_body_buffer.set_text("")
        self.response_headers_buffer.set_text("")
        self.response_raw_buffer.set_text("")

    def on_settings_clicked(self, button):
        """Handle settings button click"""
        dialog = SettingsDialog(self, self.client)
        response = dialog.run()
        dialog.destroy()

        if response == Gtk.ResponseType.OK:
            # Update status
            self.update_status()

    def update_status(self):
        """Update status indicator"""
        status = self.client.get_status_summary()

        if status["client_available"] and status["all_certs_ready"]:
            self.status_label.set_markup("<span color='green'>● Ready</span>")
        elif status["client_available"]:
            self.status_label.set_markup("<span color='orange'>● Certificates Missing</span>")
        else:
            self.status_label.set_markup("<span color='red'>● Client Not Found</span>")

        return True  # Continue timeout

    def load_history(self):
        """Load request history"""
        # Could populate a history dropdown or sidebar
        pass

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

    def __init__(self, parent, client):
        super().__init__(title="Client Settings", parent=parent, flags=0)
        self.add_buttons(
            Gtk.STOCK_CANCEL, Gtk.ResponseType.CANCEL,
            Gtk.STOCK_OK, Gtk.ResponseType.OK
        )

        self.client = client
        self.set_default_size(500, 400)

        box = self.get_content_area()
        box.set_spacing(10)
        box.set_border_width(10)

        # Client path
        label = Gtk.Label(label="Client Executable:")
        label.set_xalign(0)
        box.pack_start(label, False, False, 0)

        self.client_entry = Gtk.Entry()
        self.client_entry.set_text(client.client_path or "")
        box.pack_start(self.client_entry, False, False, 0)

        # Certificate path
        label = Gtk.Label(label="Client Certificate:")
        label.set_xalign(0)
        box.pack_start(label, False, False, 0)

        self.cert_entry = Gtk.Entry()
        self.cert_entry.set_text(client.cert_path or "")
        box.pack_start(self.cert_entry, False, False, 0)

        # Key path
        label = Gtk.Label(label="Client Key:")
        label.set_xalign(0)
        box.pack_start(label, False, False, 0)

        self.key_entry = Gtk.Entry()
        self.key_entry.set_text(client.key_path or "")
        box.pack_start(self.key_entry, False, False, 0)

        # CA path
        label = Gtk.Label(label="Root CA Certificate:")
        label.set_xalign(0)
        box.pack_start(label, False, False, 0)

        self.ca_entry = Gtk.Entry()
        self.ca_entry.set_text(client.ca_path or "")
        box.pack_start(self.ca_entry, False, False, 0)

        self.show_all()

    def get_settings(self):
        """Get settings from dialog"""
        return {
            "client_path": self.client_entry.get_text(),
            "cert_path": self.cert_entry.get_text(),
            "key_path": self.key_entry.get_text(),
            "ca_path": self.ca_entry.get_text()
        }


# Import Pango for font
from gi.repository import Pango
