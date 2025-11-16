#!/usr/bin/env python3
"""
Settings Tab - Application Settings
"""

import gi
gi.require_version('Gtk', '3.0')
from gi.repository import Gtk


class SettingsTab(Gtk.Box):
    """Settings tab for application configuration"""

    def __init__(self, stepca, on_settings_changed):
        super().__init__(orientation=Gtk.Orientation.VERTICAL, spacing=10)
        self.stepca = stepca
        self.on_settings_changed = on_settings_changed
        self.set_margin_top(10)
        self.set_margin_bottom(10)
        self.set_margin_start(10)
        self.set_margin_end(10)

        # Create title
        title = Gtk.Label()
        title.set_markup("<big><b>Settings</b></big>")
        title.set_halign(Gtk.Align.START)
        self.pack_start(title, False, False, 0)

        # Create paths section
        self.create_paths_section()

        # Create CA settings section
        self.create_ca_settings_section()

        # Create about section
        self.create_about_section()

    def create_paths_section(self):
        """Create paths configuration section"""
        frame = Gtk.Frame(label="Binary Paths")
        frame.set_margin_top(10)
        self.pack_start(frame, False, False, 0)

        grid = Gtk.Grid()
        grid.set_margin_top(10)
        grid.set_margin_bottom(10)
        grid.set_margin_start(10)
        grid.set_margin_end(10)
        grid.set_row_spacing(10)
        grid.set_column_spacing(10)
        frame.add(grid)

        # Step CLI path
        label = Gtk.Label(label="Step CLI:")
        label.set_halign(Gtk.Align.END)
        grid.attach(label, 0, 0, 1, 1)

        self.step_path_entry = Gtk.Entry()
        self.step_path_entry.set_text(self.stepca.step_path)
        self.step_path_entry.set_hexpand(True)
        grid.attach(self.step_path_entry, 1, 0, 1, 1)

        browse_button = Gtk.Button(label="Browse...")
        browse_button.connect("clicked", self.on_browse_step)
        grid.attach(browse_button, 2, 0, 1, 1)

        # Step-CA path
        label = Gtk.Label(label="Step-CA:")
        label.set_halign(Gtk.Align.END)
        grid.attach(label, 0, 1, 1, 1)

        self.step_ca_path_entry = Gtk.Entry()
        self.step_ca_path_entry.set_text(self.stepca.step_ca_path)
        self.step_ca_path_entry.set_hexpand(True)
        grid.attach(self.step_ca_path_entry, 1, 1, 1, 1)

        browse_button = Gtk.Button(label="Browse...")
        browse_button.connect("clicked", self.on_browse_step_ca)
        grid.attach(browse_button, 2, 1, 1, 1)

    def create_ca_settings_section(self):
        """Create CA settings section"""
        frame = Gtk.Frame(label="CA Configuration")
        frame.set_margin_top(10)
        self.pack_start(frame, False, False, 0)

        grid = Gtk.Grid()
        grid.set_margin_top(10)
        grid.set_margin_bottom(10)
        grid.set_margin_start(10)
        grid.set_margin_end(10)
        grid.set_row_spacing(10)
        grid.set_column_spacing(10)
        frame.add(grid)

        # CA URL
        label = Gtk.Label(label="CA URL:")
        label.set_halign(Gtk.Align.END)
        grid.attach(label, 0, 0, 1, 1)

        self.ca_url_entry = Gtk.Entry()
        self.ca_url_entry.set_text(self.stepca.ca_url)
        self.ca_url_entry.set_hexpand(True)
        grid.attach(self.ca_url_entry, 1, 0, 1, 1)

        # Apply button
        apply_button = Gtk.Button(label="Apply Settings")
        apply_button.connect("clicked", self.on_apply_settings)
        grid.attach(apply_button, 1, 1, 1, 1)

    def create_about_section(self):
        """Create about section"""
        frame = Gtk.Frame(label="About")
        frame.set_margin_top(10)
        self.pack_start(frame, True, True, 0)

        box = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=10)
        box.set_margin_top(10)
        box.set_margin_bottom(10)
        box.set_margin_start(10)
        box.set_margin_end(10)
        frame.add(box)

        about_label = Gtk.Label()
        about_label.set_markup(
            "<b>Step-CA Manager</b>\n\n"
            "A graphical user interface for managing your Step-CA Certificate Authority.\n\n"
            "<b>Features:</b>\n"
            "• CA status monitoring\n"
            "• Certificate management (request, renew, revoke)\n"
            "• Certificate inspection\n"
            "• Provisioner management\n\n"
            "<small>Built with GTK+ 3 and Python</small>"
        )
        about_label.set_halign(Gtk.Align.START)
        box.pack_start(about_label, False, False, 0)

    def on_browse_step(self, button):
        """Browse for step CLI binary"""
        dialog = Gtk.FileChooserDialog(
            title="Select step CLI binary",
            parent=self.get_toplevel(),
            action=Gtk.FileChooserAction.OPEN
        )
        dialog.add_buttons(
            Gtk.STOCK_CANCEL, Gtk.ResponseType.CANCEL,
            Gtk.STOCK_OPEN, Gtk.ResponseType.OK
        )

        response = dialog.run()
        if response == Gtk.ResponseType.OK:
            self.step_path_entry.set_text(dialog.get_filename())

        dialog.destroy()

    def on_browse_step_ca(self, button):
        """Browse for step-ca binary"""
        dialog = Gtk.FileChooserDialog(
            title="Select step-ca binary",
            parent=self.get_toplevel(),
            action=Gtk.FileChooserAction.OPEN
        )
        dialog.add_buttons(
            Gtk.STOCK_CANCEL, Gtk.ResponseType.CANCEL,
            Gtk.STOCK_OPEN, Gtk.ResponseType.OK
        )

        response = dialog.run()
        if response == Gtk.ResponseType.OK:
            self.step_ca_path_entry.set_text(dialog.get_filename())

        dialog.destroy()

    def on_apply_settings(self, button):
        """Apply settings changes"""
        # Update StepCA instance
        self.stepca.step_path = self.step_path_entry.get_text()
        self.stepca.step_ca_path = self.step_ca_path_entry.get_text()
        self.stepca.ca_url = self.ca_url_entry.get_text()

        # Show confirmation
        dialog = Gtk.MessageDialog(
            transient_for=self.get_toplevel(),
            flags=0,
            message_type=Gtk.MessageType.INFO,
            buttons=Gtk.ButtonsType.OK,
            text="Settings Applied"
        )
        dialog.format_secondary_text("Settings have been updated successfully.")
        dialog.run()
        dialog.destroy()

        # Notify parent to refresh
        if self.on_settings_changed:
            self.on_settings_changed()
