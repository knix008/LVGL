#!/usr/bin/env python3
"""
Provisioners Tab - Provisioner Management
"""

import gi
gi.require_version('Gtk', '3.0')
from gi.repository import Gtk


class ProvisionersTab(Gtk.Box):
    """Provisioners tab for managing provisioners"""

    def __init__(self, stepca):
        super().__init__(orientation=Gtk.Orientation.VERTICAL, spacing=10)
        self.stepca = stepca
        self.set_margin_top(10)
        self.set_margin_bottom(10)
        self.set_margin_start(10)
        self.set_margin_end(10)

        # Create title
        title = Gtk.Label()
        title.set_markup("<big><b>Provisioner Management</b></big>")
        title.set_halign(Gtk.Align.START)
        self.pack_start(title, False, False, 0)

        # Create provisioner list
        self.create_provisioner_list()

    def create_provisioner_list(self):
        """Create provisioner list section"""
        frame = Gtk.Frame(label="Provisioners")
        frame.set_margin_top(10)
        self.pack_start(frame, True, True, 0)

        box = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=10)
        box.set_margin_top(10)
        box.set_margin_bottom(10)
        box.set_margin_start(10)
        box.set_margin_end(10)
        frame.add(box)

        # Create scrolled window
        scrolled = Gtk.ScrolledWindow()
        scrolled.set_policy(Gtk.PolicyType.AUTOMATIC, Gtk.PolicyType.AUTOMATIC)
        box.pack_start(scrolled, True, True, 0)

        # Create list store
        self.provisioner_liststore = Gtk.ListStore(str, str)

        # Create tree view
        self.provisioner_treeview = Gtk.TreeView(model=self.provisioner_liststore)
        self.provisioner_treeview.set_headers_visible(True)

        # Add columns
        renderer = Gtk.CellRendererText()

        column = Gtk.TreeViewColumn("Name", renderer, text=0)
        column.set_expand(True)
        self.provisioner_treeview.append_column(column)

        column = Gtk.TreeViewColumn("Type", renderer, text=1)
        self.provisioner_treeview.append_column(column)

        scrolled.add(self.provisioner_treeview)

        # Info label
        info_label = Gtk.Label()
        info_label.set_markup(
            "<i>Provisioners are used to authenticate certificate requests.\n"
            "Manage provisioners using the step CLI or by editing ca.json.</i>"
        )
        info_label.set_halign(Gtk.Align.START)
        box.pack_start(info_label, False, False, 0)

    def refresh(self):
        """Refresh provisioner list"""
        self.provisioner_liststore.clear()

        provisioners = self.stepca.get_provisioners()

        for prov in provisioners:
            self.provisioner_liststore.append([
                prov.get('name', 'Unknown'),
                prov.get('type', 'Unknown')
            ])
