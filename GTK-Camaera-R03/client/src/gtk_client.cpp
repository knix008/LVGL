#include "../include/gtk_client.h"
#include <iostream>
#include <sstream>
#include <iomanip>

GTKClient::GTKClient()
    : window(nullptr), status_label(nullptr), response_text(nullptr),
      initial_entry(nullptr), id_entry(nullptr),
      camera_on_button(nullptr), camera_off_button(nullptr),
      capture_button(nullptr), registering_button(nullptr),
      status_button(nullptr), clear_button(nullptr) {
    socket_client = std::make_unique<SocketClientLib>();
}

GTKClient::~GTKClient() {
    cleanup();
}

bool GTKClient::init() {
    try {
        gtk_init(nullptr, nullptr);
        create_ui();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to initialize client: " << e.what() << std::endl;
        return false;
    }
}

void GTKClient::create_ui() {
    // Create main window
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Face Recognition Server Control");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    gtk_window_set_resizable(GTK_WINDOW(window), TRUE);

    g_signal_connect(window, "destroy", G_CALLBACK(on_window_destroy), this);

    // Main container
    GtkWidget* main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(main_box), 10);
    gtk_container_add(GTK_CONTAINER(window), main_box);

    // Status label
    status_label = gtk_label_new("Ready");
    gtk_label_set_xalign(GTK_LABEL(status_label), 0.0);
    gtk_box_pack_start(GTK_BOX(main_box), status_label, FALSE, FALSE, 0);

    // Command buttons section
    GtkWidget* buttons_frame = gtk_frame_new("Commands");
    gtk_box_pack_start(GTK_BOX(main_box), buttons_frame, FALSE, FALSE, 0);

    GtkWidget* buttons_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(buttons_box), 10);
    gtk_container_add(GTK_CONTAINER(buttons_frame), buttons_box);

    // Row 1: Camera controls
    GtkWidget* camera_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_pack_start(GTK_BOX(buttons_box), camera_box, FALSE, FALSE, 0);

    camera_on_button = gtk_button_new_with_label("Camera On");
    gtk_widget_set_size_request(camera_on_button, 150, 40);
    g_signal_connect(camera_on_button, "clicked", G_CALLBACK(on_camera_on_clicked), this);
    gtk_box_pack_start(GTK_BOX(camera_box), camera_on_button, FALSE, FALSE, 0);

    camera_off_button = gtk_button_new_with_label("Camera Off");
    gtk_widget_set_size_request(camera_off_button, 150, 40);
    g_signal_connect(camera_off_button, "clicked", G_CALLBACK(on_camera_off_clicked), this);
    gtk_box_pack_start(GTK_BOX(camera_box), camera_off_button, FALSE, FALSE, 0);

    status_button = gtk_button_new_with_label("Get Status");
    gtk_widget_set_size_request(status_button, 150, 40);
    g_signal_connect(status_button, "clicked", G_CALLBACK(on_status_clicked), this);
    gtk_box_pack_start(GTK_BOX(camera_box), status_button, FALSE, FALSE, 0);

    // Row 2: Capture section
    GtkWidget* capture_frame = gtk_frame_new("Capture Person");
    gtk_box_pack_start(GTK_BOX(buttons_box), capture_frame, FALSE, FALSE, 0);

    GtkWidget* capture_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(capture_box), 10);
    gtk_container_add(GTK_CONTAINER(capture_frame), capture_box);

    // Initial entry
    GtkWidget* initial_label = gtk_label_new("Initial:");
    gtk_box_pack_start(GTK_BOX(capture_box), initial_label, FALSE, FALSE, 0);

    initial_entry = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(initial_entry), 1);
    gtk_entry_set_placeholder_text(GTK_ENTRY(initial_entry), "A");
    gtk_widget_set_size_request(initial_entry, 50, 35);
    gtk_box_pack_start(GTK_BOX(capture_box), initial_entry, FALSE, FALSE, 0);

    // ID entry
    GtkWidget* id_label = gtk_label_new("ID:");
    gtk_box_pack_start(GTK_BOX(capture_box), id_label, FALSE, FALSE, 0);

    id_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(id_entry), "1");
    gtk_widget_set_size_request(id_entry, 100, 35);
    gtk_box_pack_start(GTK_BOX(capture_box), id_entry, FALSE, FALSE, 0);

    // Capture button
    capture_button = gtk_button_new_with_label("Capture");
    gtk_widget_set_size_request(capture_button, 150, 40);
    g_signal_connect(capture_button, "clicked", G_CALLBACK(on_capture_clicked), this);
    gtk_box_pack_start(GTK_BOX(capture_box), capture_button, FALSE, FALSE, 0);

    // Row 3: Training
    GtkWidget* training_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_pack_start(GTK_BOX(buttons_box), training_box, FALSE, FALSE, 0);

    registering_button = gtk_button_new_with_label("Train Model (Registering)");
    gtk_widget_set_size_request(registering_button, 200, 40);
    g_signal_connect(registering_button, "clicked", G_CALLBACK(on_registering_clicked), this);
    gtk_box_pack_start(GTK_BOX(training_box), registering_button, FALSE, FALSE, 0);

    // Response section
    GtkWidget* response_frame = gtk_frame_new("Server Responses");
    gtk_box_pack_start(GTK_BOX(main_box), response_frame, TRUE, TRUE, 0);

    GtkWidget* response_scroll = gtk_scrolled_window_new(nullptr, nullptr);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(response_scroll),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(response_frame), response_scroll);

    response_text = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(response_text), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(response_text), GTK_WRAP_WORD);
    gtk_container_add(GTK_CONTAINER(response_scroll), response_text);

    // Bottom button bar
    GtkWidget* bottom_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_pack_end(GTK_BOX(main_box), bottom_box, FALSE, FALSE, 0);

    clear_button = gtk_button_new_with_label("Clear Responses");
    gtk_widget_set_size_request(clear_button, 150, 40);
    g_signal_connect(clear_button, "clicked", G_CALLBACK(on_clear_clicked), this);
    gtk_box_pack_end(GTK_BOX(bottom_box), clear_button, FALSE, FALSE, 0);

    gtk_widget_show_all(window);
}

void GTKClient::run() {
    gtk_main();
}

void GTKClient::cleanup() {
    if (window) {
        gtk_widget_destroy(window);
        window = nullptr;
    }
}

void GTKClient::append_response(const std::string& title,
                                const SocketClientLib::Response& response) {
    GtkTextBuffer* buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(response_text));
    GtkTextIter end;
    gtk_text_buffer_get_end_iter(buffer, &end);

    // Get current time
    auto now = std::time(nullptr);
    auto tm = *std::localtime(&now);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%H:%M:%S");
    std::string timestamp = oss.str();

    // Format response
    std::string formatted = "[" + timestamp + "] " + title + ": ";
    if (response.success) {
        formatted += "OK - " + response.message;
    } else {
        formatted += "ERROR - " + response.message;
    }
    formatted += "\n";

    gtk_text_buffer_insert(buffer, &end, formatted.c_str(), -1);

    // Scroll to end
    gtk_text_buffer_get_end_iter(buffer, &end);
    GtkTextMark* mark = gtk_text_buffer_create_mark(buffer, nullptr, &end, FALSE);
    gtk_text_view_scroll_to_mark(GTK_TEXT_VIEW(response_text), mark, 0, FALSE, 0, 0);
}

void GTKClient::set_status_label(const std::string& message, gboolean is_error) {
    gtk_label_set_text(GTK_LABEL(status_label), message.c_str());

    // Use CSS styling instead of deprecated gtk_widget_override_color
    GtkCssProvider* provider = gtk_css_provider_new();
    if (is_error) {
        gtk_css_provider_load_from_data(provider, "label { color: red; }", -1, nullptr);
    } else {
        gtk_css_provider_load_from_data(provider, "label { color: inherit; }", -1, nullptr);
    }

    GtkStyleContext* context = gtk_widget_get_style_context(status_label);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    g_object_unref(provider);
}

void GTKClient::handle_camera_on() {
    set_status_label("Sending camera_on...", FALSE);
    auto response = socket_client->camera_on();
    append_response("camera_on", response);
    set_status_label(response.message, !response.success);
}

void GTKClient::handle_camera_off() {
    set_status_label("Sending camera_off...", FALSE);
    auto response = socket_client->camera_off();
    append_response("camera_off", response);
    set_status_label(response.message, !response.success);
}

void GTKClient::handle_capture() {
    const char* initial = gtk_entry_get_text(GTK_ENTRY(initial_entry));
    const char* id = gtk_entry_get_text(GTK_ENTRY(id_entry));

    if (!initial || strlen(initial) == 0 || !id || strlen(id) == 0) {
        set_status_label("Please enter initial and ID", TRUE);
        return;
    }

    std::string message = std::string("Capturing ") + initial + id + "...";
    set_status_label(message, FALSE);

    auto response = socket_client->capture(initial, id);
    append_response("capture:" + std::string(initial) + ":" + std::string(id), response);
    set_status_label(response.message, !response.success);
}

void GTKClient::handle_registering() {
    set_status_label("Sending registering command...", FALSE);
    auto response = socket_client->registering();
    append_response("registering", response);
    set_status_label(response.message, !response.success);
}

void GTKClient::handle_status() {
    set_status_label("Getting server status...", FALSE);
    auto response = socket_client->status();
    append_response("status", response);
    set_status_label(response.message, !response.success);
}

void GTKClient::handle_clear() {
    GtkTextBuffer* buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(response_text));
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(buffer, &start, &end);
    gtk_text_buffer_delete(buffer, &start, &end);
    set_status_label("Responses cleared", FALSE);
}

// Static callback implementations
void GTKClient::on_camera_on_clicked(GtkWidget* /* widget */, gpointer user_data) {
    GTKClient* self = static_cast<GTKClient*>(user_data);
    self->handle_camera_on();
}

void GTKClient::on_camera_off_clicked(GtkWidget* /* widget */, gpointer user_data) {
    GTKClient* self = static_cast<GTKClient*>(user_data);
    self->handle_camera_off();
}

void GTKClient::on_capture_clicked(GtkWidget* /* widget */, gpointer user_data) {
    GTKClient* self = static_cast<GTKClient*>(user_data);
    self->handle_capture();
}

void GTKClient::on_registering_clicked(GtkWidget* /* widget */, gpointer user_data) {
    GTKClient* self = static_cast<GTKClient*>(user_data);
    self->handle_registering();
}

void GTKClient::on_status_clicked(GtkWidget* /* widget */, gpointer user_data) {
    GTKClient* self = static_cast<GTKClient*>(user_data);
    self->handle_status();
}

void GTKClient::on_clear_clicked(GtkWidget* /* widget */, gpointer user_data) {
    GTKClient* self = static_cast<GTKClient*>(user_data);
    self->handle_clear();
}

void GTKClient::on_window_destroy(GtkWidget* /* widget */, gpointer user_data) {
    GTKClient* self = static_cast<GTKClient*>(user_data);
    self->cleanup();
    gtk_main_quit();
}
