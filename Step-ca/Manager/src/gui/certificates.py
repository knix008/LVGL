#!/usr/bin/env python3
"""
Certificates Tab - Certificate Management
"""

import gi
gi.require_version('Gtk', '3.0')
from gi.repository import Gtk, Pango, GLib
import os
import threading
from pathlib import Path


class CertificatesTab(Gtk.Box):
    """Certificates tab for managing certificates"""

    def __init__(self, stepca):
        super().__init__(orientation=Gtk.Orientation.VERTICAL, spacing=10)
        self.stepca = stepca
        self.set_margin_top(10)
        self.set_margin_bottom(10)
        self.set_margin_start(10)
        self.set_margin_end(10)

        # Certificate storage path
        self.cert_dir = Path(__file__).parent.parent.parent / "certs"
        self.cert_dir.mkdir(exist_ok=True)

        # Create paned layout (split view)
        paned = Gtk.Paned(orientation=Gtk.Orientation.HORIZONTAL)
        self.pack_start(paned, True, True, 0)

        # Left side: Certificate list and actions
        left_box = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=10)
        left_box.set_size_request(400, -1)
        paned.add1(left_box)

        # Add title
        title = Gtk.Label()
        title.set_markup("<big><b>Certificate Management</b></big>")
        title.set_halign(Gtk.Align.START)
        left_box.pack_start(title, False, False, 0)

        # Create request certificate section
        self.create_request_section(left_box)

        # Create certificate list section
        self.create_list_section(left_box)

        # Right side: Certificate viewer
        right_box = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=10)
        paned.add2(right_box)

        viewer_title = Gtk.Label()
        viewer_title.set_markup("<b>Certificate Details</b>")
        viewer_title.set_halign(Gtk.Align.START)
        right_box.pack_start(viewer_title, False, False, 0)

        self.create_certificate_viewer(right_box)

    def create_request_section(self, parent):
        """Create request certificate section"""
        frame = Gtk.Frame(label="Request New Certificate")
        frame.set_margin_top(10)
        parent.pack_start(frame, False, False, 0)

        grid = Gtk.Grid()
        grid.set_margin_top(10)
        grid.set_margin_bottom(10)
        grid.set_margin_start(10)
        grid.set_margin_end(10)
        grid.set_row_spacing(10)
        grid.set_column_spacing(10)
        frame.add(grid)

        # Common Name entry
        label = Gtk.Label(label="Common Name:")
        label.set_halign(Gtk.Align.END)
        grid.attach(label, 0, 0, 1, 1)

        self.cn_entry = Gtk.Entry()
        self.cn_entry.set_placeholder_text("e.g., myserver.local")
        self.cn_entry.set_hexpand(True)
        grid.attach(self.cn_entry, 1, 0, 1, 1)

        # Request button
        request_button = Gtk.Button(label="Request Certificate")
        request_button.connect("clicked", self.on_request_certificate)
        grid.attach(request_button, 1, 1, 1, 1)

    def create_list_section(self, parent):
        """Create certificate list section"""
        frame = Gtk.Frame(label="Existing Certificates")
        frame.set_margin_top(10)
        parent.pack_start(frame, True, True, 0)

        box = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=10)
        box.set_margin_top(10)
        box.set_margin_bottom(10)
        box.set_margin_start(10)
        box.set_margin_end(10)
        frame.add(box)

        # Create scrolled window for list
        scrolled = Gtk.ScrolledWindow()
        scrolled.set_policy(Gtk.PolicyType.AUTOMATIC, Gtk.PolicyType.AUTOMATIC)
        scrolled.set_min_content_height(200)
        box.pack_start(scrolled, True, True, 0)

        # Create list store
        self.cert_liststore = Gtk.ListStore(str, str, str)

        # Create tree view
        self.cert_treeview = Gtk.TreeView(model=self.cert_liststore)
        self.cert_treeview.set_headers_visible(True)

        # Add columns
        renderer = Gtk.CellRendererText()
        column = Gtk.TreeViewColumn("Name", renderer, text=0)
        column.set_expand(True)
        self.cert_treeview.append_column(column)

        column = Gtk.TreeViewColumn("Status", renderer, text=1)
        self.cert_treeview.append_column(column)

        # Connect selection changed
        self.cert_selection = self.cert_treeview.get_selection()
        self.cert_selection.connect("changed", self.on_cert_selected)

        scrolled.add(self.cert_treeview)

        # Action buttons
        button_box = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL, spacing=5)
        button_box.set_halign(Gtk.Align.END)
        box.pack_start(button_box, False, False, 0)

        inspect_button = Gtk.Button(label="Inspect")
        inspect_button.connect("clicked", self.on_inspect_certificate)
        button_box.pack_start(inspect_button, False, False, 0)

        renew_button = Gtk.Button(label="Renew")
        renew_button.connect("clicked", self.on_renew_certificate)
        button_box.pack_start(renew_button, False, False, 0)

        revoke_button = Gtk.Button(label="Revoke")
        revoke_button.connect("clicked", self.on_revoke_certificate)
        button_box.pack_start(revoke_button, False, False, 0)

    def create_certificate_viewer(self, parent):
        """Create certificate details viewer"""
        scrolled = Gtk.ScrolledWindow()
        scrolled.set_policy(Gtk.PolicyType.AUTOMATIC, Gtk.PolicyType.AUTOMATIC)
        parent.pack_start(scrolled, True, True, 0)

        self.cert_viewer = Gtk.TextView()
        self.cert_viewer.set_editable(False)
        self.cert_viewer.set_cursor_visible(False)
        self.cert_viewer.set_wrap_mode(Gtk.WrapMode.WORD)
        self.cert_viewer.set_margin_top(10)
        self.cert_viewer.set_margin_bottom(10)
        self.cert_viewer.set_margin_start(10)
        self.cert_viewer.set_margin_end(10)

        # Use monospace font
        font_desc = Pango.FontDescription.from_string("Monospace 10")
        self.cert_viewer.modify_font(font_desc)

        scrolled.add(self.cert_viewer)

    def refresh(self):
        """Refresh certificate list"""
        self.cert_liststore.clear()

        # Scan cert directory for certificates
        if self.cert_dir.exists():
            for cert_file in self.cert_dir.glob("*.crt"):
                name = cert_file.stem

                # Get certificate validity
                from core.utils import get_cert_validity
                validity = get_cert_validity(str(cert_file))

                if validity:
                    if validity['is_valid']:
                        status = f"✓ Valid ({validity['days_remaining']} days)"
                    else:
                        status = "✗ Expired"
                else:
                    status = "Unknown"

                self.cert_liststore.append([name, status, str(cert_file)])

    def on_request_certificate(self, button):
        """Handle certificate request"""
        cn = self.cn_entry.get_text().strip()

        if not cn:
            self.show_error("Please enter a common name")
            return

        # Get provisioner password
        password = self.show_password_dialog("Enter Provisioner Password",
                                            "Enter the provisioner password to request a certificate:")
        if password is None:  # User cancelled
            return

        # Disable button and show progress
        button.set_sensitive(False)
        button.set_label("Requesting...")

        # Generate paths
        cert_path = self.cert_dir / f"{cn}.crt"
        key_path = self.cert_dir / f"{cn}.key"

        # Run in background thread
        def request_cert_thread():
            success, message = self.stepca.request_certificate(
                cn,
                str(cert_path),
                str(key_path),
                provisioner_password=password
            )

            # Schedule UI update on main thread
            GLib.idle_add(self._on_request_certificate_done, button, cn, success, message)

        thread = threading.Thread(target=request_cert_thread, daemon=True)
        thread.start()

    def _on_request_certificate_done(self, button, cn, success, message):
        """Called when certificate request completes"""
        button.set_sensitive(True)
        button.set_label("Request Certificate")

        if success:
            self.show_info(f"Certificate requested successfully for {cn}")
            self.cn_entry.set_text("")
            self.refresh()
        else:
            self.show_error(f"Failed to request certificate:\n{message}")

        return False  # Don't call again

    def on_cert_selected(self, selection):
        """Handle certificate selection"""
        model, treeiter = selection.get_selected()
        if treeiter:
            cert_path = model[treeiter][2]
            self.display_certificate(cert_path)

    def display_certificate(self, cert_path):
        """Display certificate details"""
        cert_info = self.stepca.inspect_certificate(cert_path)

        if cert_info:
            import json
            text = json.dumps(cert_info, indent=2)
        else:
            # Fallback: display raw certificate
            try:
                with open(cert_path, 'r') as f:
                    text = f.read()
            except Exception as e:
                text = f"Error reading certificate: {e}"

        self.cert_viewer.get_buffer().set_text(text)

    def on_inspect_certificate(self, button):
        """Handle inspect certificate button"""
        model, treeiter = self.cert_selection.get_selected()
        if treeiter:
            cert_path = model[treeiter][2]
            self.display_certificate(cert_path)

    def on_renew_certificate(self, button):
        """Handle renew certificate button"""
        model, treeiter = self.cert_selection.get_selected()
        if not treeiter:
            self.show_error("Please select a certificate")
            return

        name = model[treeiter][0]
        cert_path = model[treeiter][2]
        key_path = cert_path.replace('.crt', '.key')

        # Confirm renewal
        dialog = Gtk.MessageDialog(
            transient_for=self.get_toplevel(),
            flags=0,
            message_type=Gtk.MessageType.QUESTION,
            buttons=Gtk.ButtonsType.YES_NO,
            text=f"Renew certificate for {name}?"
        )
        response = dialog.run()
        dialog.destroy()

        if response == Gtk.ResponseType.YES:
            # Get provisioner password
            password = self.show_password_dialog("Enter Provisioner Password",
                                                "Enter the provisioner password to renew the certificate:")
            if password is None:  # User cancelled
                return

            # Run in background thread
            def renew_cert_thread():
                success, message = self.stepca.renew_certificate(cert_path, key_path,
                                                                provisioner_password=password)
                GLib.idle_add(self._on_renew_certificate_done, name, success, message)

            thread = threading.Thread(target=renew_cert_thread, daemon=True)
            thread.start()

    def _on_renew_certificate_done(self, name, success, message):
        """Called when certificate renewal completes"""
        if success:
            self.show_info(f"Certificate renewed successfully")
            self.refresh()
        else:
            self.show_error(f"Failed to renew certificate:\n{message}")

        return False

    def on_revoke_certificate(self, button):
        """Handle revoke certificate button"""
        model, treeiter = self.cert_selection.get_selected()
        if not treeiter:
            self.show_error("Please select a certificate")
            return

        name = model[treeiter][0]
        cert_path = model[treeiter][2]

        # Confirm revocation
        dialog = Gtk.MessageDialog(
            transient_for=self.get_toplevel(),
            flags=0,
            message_type=Gtk.MessageType.WARNING,
            buttons=Gtk.ButtonsType.YES_NO,
            text=f"Revoke certificate for {name}?"
        )
        dialog.format_secondary_text("This action cannot be undone!")
        response = dialog.run()
        dialog.destroy()

        if response == Gtk.ResponseType.YES:
            # Run in background thread
            def revoke_cert_thread():
                success, message = self.stepca.revoke_certificate(cert_path)
                GLib.idle_add(self._on_revoke_certificate_done, name, success, message)

            thread = threading.Thread(target=revoke_cert_thread, daemon=True)
            thread.start()

    def _on_revoke_certificate_done(self, name, success, message):
        """Called when certificate revocation completes"""
        if success:
            self.show_info(f"Certificate revoked successfully")
            self.refresh()
        else:
            self.show_error(f"Failed to revoke certificate:\n{message}")

        return False

    def show_password_dialog(self, title, message):
        """Show password input dialog

        Returns:
            The password string or None if cancelled
        """
        dialog = Gtk.Dialog(
            title=title,
            transient_for=self.get_toplevel(),
            flags=0
        )
        dialog.add_buttons(Gtk.STOCK_CANCEL, Gtk.ResponseType.CANCEL,
                          Gtk.STOCK_OK, Gtk.ResponseType.OK)

        box = dialog.get_content_area()
        box.set_margin_top(10)
        box.set_margin_bottom(10)
        box.set_margin_start(10)
        box.set_margin_end(10)

        # Add label
        label = Gtk.Label(label=message)
        label.set_halign(Gtk.Align.START)
        box.pack_start(label, False, False, 0)

        # Add password entry
        password_entry = Gtk.Entry()
        password_entry.set_visibility(False)
        password_entry.set_placeholder_text("Enter password")
        box.pack_start(password_entry, False, False, 10)

        box.show_all()

        response = dialog.run()
        password = password_entry.get_text() if response == Gtk.ResponseType.OK else None
        dialog.destroy()

        return password

    def show_error(self, message):
        """Show error dialog"""
        dialog = Gtk.MessageDialog(
            transient_for=self.get_toplevel(),
            flags=0,
            message_type=Gtk.MessageType.ERROR,
            buttons=Gtk.ButtonsType.OK,
            text="Error"
        )
        dialog.format_secondary_text(message)
        dialog.run()
        dialog.destroy()

    def show_info(self, message):
        """Show info dialog"""
        dialog = Gtk.MessageDialog(
            transient_for=self.get_toplevel(),
            flags=0,
            message_type=Gtk.MessageType.INFO,
            buttons=Gtk.ButtonsType.OK,
            text="Success"
        )
        dialog.format_secondary_text(message)
        dialog.run()
        dialog.destroy()
