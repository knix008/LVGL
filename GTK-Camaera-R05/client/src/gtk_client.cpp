#include "../include/gtk_client.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <unistd.h>
#include <thread>
#include <chrono>
#include <sys/socket.h>

GTKClient::GTKClient()
    : window(nullptr), status_label(nullptr), response_text(nullptr),
      initial_entry(nullptr), id_entry(nullptr),
      camera_on_button(nullptr), camera_off_button(nullptr),
      capture_button(nullptr), registering_button(nullptr),
      status_button(nullptr), clear_button(nullptr),
      stream_button(nullptr), stream_text(nullptr),
      streaming_active(false), stream_socket(-1) {
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

    // Streaming section
    GtkWidget* stream_frame = gtk_frame_new("Recognition Stream");
    gtk_box_pack_start(GTK_BOX(main_box), stream_frame, FALSE, FALSE, 0);

    GtkWidget* stream_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(stream_vbox), 10);
    gtk_container_add(GTK_CONTAINER(stream_frame), stream_vbox);

    stream_button = gtk_button_new_with_label("Start Recognition Stream");
    gtk_widget_set_size_request(stream_button, 200, 40);
    g_signal_connect(stream_button, "clicked", G_CALLBACK(on_stream_clicked), this);
    gtk_box_pack_start(GTK_BOX(stream_vbox), stream_button, FALSE, FALSE, 0);

    GtkWidget* stream_scroll = gtk_scrolled_window_new(nullptr, nullptr);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(stream_scroll),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_size_request(stream_scroll, -1, 100);
    gtk_box_pack_start(GTK_BOX(stream_vbox), stream_scroll, TRUE, TRUE, 0);

    stream_text = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(stream_text), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(stream_text), GTK_WRAP_WORD);
    gtk_container_add(GTK_CONTAINER(stream_scroll), stream_text);

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
    // Stop streaming if active
    if (streaming_active) {
        streaming_active = false;
        if (stream_socket >= 0) {
            // Shutdown socket to interrupt blocking read()
            shutdown(stream_socket, SHUT_RDWR);
            close(stream_socket);
            stream_socket = -1;
        }
        if (stream_thread.joinable()) {
            // Give thread a moment to exit, then detach if needed
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            if (stream_thread.joinable()) {
                stream_thread.detach();
            }
        }
    }

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
    
    // Run in separate thread to avoid blocking UI
    std::thread([this]() {
        auto response = socket_client->camera_on();
        
        // Update UI in main thread
        g_idle_add([](gpointer user_data) {
            auto* data = static_cast<std::pair<GTKClient*, SocketClientLib::Response>*>(user_data);
            GTKClient* self = data->first;
            auto& response = data->second;
            
            self->append_response("camera_on", response);
            self->set_status_label(response.message, !response.success);
            
            delete data;
            return FALSE;
        }, new std::pair<GTKClient*, SocketClientLib::Response>(this, response));
    }).detach();
}

void GTKClient::handle_camera_off() {
    set_status_label("Sending camera_off...", FALSE);
    
    // Run in separate thread to avoid blocking UI
    std::thread([this]() {
        auto response = socket_client->camera_off();
        
        // Update UI in main thread
        g_idle_add([](gpointer user_data) {
            auto* data = static_cast<std::pair<GTKClient*, SocketClientLib::Response>*>(user_data);
            GTKClient* self = data->first;
            auto& response = data->second;
            
            self->append_response("camera_off", response);
            self->set_status_label(response.message, !response.success);
            
            delete data;
            return FALSE;
        }, new std::pair<GTKClient*, SocketClientLib::Response>(this, response));
    }).detach();
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

    // Capture command data for thread
    std::string initial_str = initial;
    std::string id_str = id;
    
    // Run in separate thread to avoid blocking UI
    std::thread([this, initial_str, id_str]() {
        auto response = socket_client->capture(initial_str, id_str);
        std::string cmd = "capture:" + initial_str + ":" + id_str;
        
        // Update UI in main thread
        g_idle_add([](gpointer user_data) {
            auto* data = static_cast<std::tuple<GTKClient*, SocketClientLib::Response, std::string>*>(user_data);
            GTKClient* self = std::get<0>(*data);
            auto& response = std::get<1>(*data);
            auto& cmd = std::get<2>(*data);
            
            self->append_response(cmd, response);
            self->set_status_label(response.message, !response.success);
            
            delete data;
            return FALSE;
        }, new std::tuple<GTKClient*, SocketClientLib::Response, std::string>(this, response, cmd));
    }).detach();
}

void GTKClient::handle_registering() {
    set_status_label("Sending registering command...", FALSE);
    
    // Run in separate thread to avoid blocking UI
    std::thread([this]() {
        auto response = socket_client->registering();
        
        // Update UI in main thread
        g_idle_add([](gpointer user_data) {
            auto* data = static_cast<std::pair<GTKClient*, SocketClientLib::Response>*>(user_data);
            GTKClient* self = data->first;
            auto& response = data->second;
            
            self->append_response("registering", response);
            self->set_status_label(response.message, !response.success);
            
            delete data;
            return FALSE;
        }, new std::pair<GTKClient*, SocketClientLib::Response>(this, response));
    }).detach();
}

void GTKClient::handle_status() {
    set_status_label("Getting server status...", FALSE);
    
    // Run in separate thread to avoid blocking UI
    std::thread([this]() {
        auto response = socket_client->status();
        
        // Update UI in main thread
        g_idle_add([](gpointer user_data) {
            auto* data = static_cast<std::pair<GTKClient*, SocketClientLib::Response>*>(user_data);
            GTKClient* self = data->first;
            auto& response = data->second;
            
            self->append_response("status", response);
            self->set_status_label(response.message, !response.success);
            
            delete data;
            return FALSE;
        }, new std::pair<GTKClient*, SocketClientLib::Response>(this, response));
    }).detach();
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

void GTKClient::on_stream_clicked(GtkWidget* /* widget */, gpointer user_data) {
    GTKClient* self = static_cast<GTKClient*>(user_data);
    self->handle_stream_toggle();
}

void GTKClient::handle_stream_toggle() {
    if (streaming_active) {
        // Stop streaming
        streaming_active = false;
        
        // Close socket if open
        if (stream_socket >= 0) {
            close(stream_socket);
            stream_socket = -1;
        }
        
        // Wait for thread to finish if it's running
        if (stream_thread.joinable()) {
            stream_thread.join();
        }
        
        gtk_button_set_label(GTK_BUTTON(stream_button), "Start Recognition Stream");
        set_status_label("Recognition stream stopped", FALSE);
    } else {
        // Start streaming
        
        // Make sure previous thread is cleaned up before starting new one
        if (stream_thread.joinable()) {
            // Previous thread might still be finishing - wait for it
            stream_thread.join();
        }
        
        streaming_active = true;
        set_status_label("Starting recognition stream...", FALSE);
        stream_thread = std::thread(&GTKClient::stream_recognition_loop, this);
    }
}

void GTKClient::stream_recognition_loop() {
    // Connect to server and start streaming
    stream_socket = socket_client->stream_recognition();
    if (stream_socket < 0) {
        // Connection failed - update UI and reset state
        g_idle_add([](gpointer user_data) {
            GTKClient* self = static_cast<GTKClient*>(user_data);
            self->streaming_active = false;
            self->stream_socket = -1;  // Ensure socket is marked as closed
            gtk_button_set_label(GTK_BUTTON(self->stream_button), "Start Recognition Stream");
            self->set_status_label("Failed to connect to server stream", TRUE);
            return FALSE;
        }, this);
        // Thread will exit here, allowing it to be joined
        return;
    }

    // Successfully connected - update button label in main thread
    g_idle_add([](gpointer user_data) {
        GTKClient* self = static_cast<GTKClient*>(user_data);
        if (self->streaming_active) {  // Only update if still active
            gtk_button_set_label(GTK_BUTTON(self->stream_button), "Stop Recognition Stream");
            self->set_status_label("Recognition stream active", FALSE);
        }
        return FALSE;
    }, this);

    // Read stream data
    char buffer[1024] = {0};
    std::string line_buffer;

    while (streaming_active && stream_socket >= 0) {
        ssize_t bytes_read = read(stream_socket, buffer, sizeof(buffer) - 1);
        if (bytes_read <= 0) {
            break;  // Connection closed or error
        }

        buffer[bytes_read] = '\0';  // Null terminate
        
        // Parse lines from buffer
        line_buffer.append(buffer, bytes_read);

        // Process complete lines
        size_t pos = 0;
        while ((pos = line_buffer.find('\n')) != std::string::npos) {
            std::string line = line_buffer.substr(0, pos);
            line_buffer.erase(0, pos + 1);

            // Parse recognition data
            if (line.empty()) continue;

            // Format: FACE:name:confidence:timestamp or NO_FACE:timestamp
            if (line.find("FACE:") == 0) {
                // Extract name and confidence
                size_t first_colon = line.find(':');
                size_t second_colon = line.find(':', first_colon + 1);
                size_t third_colon = line.find(':', second_colon + 1);

                if (first_colon != std::string::npos && second_colon != std::string::npos) {
                    std::string name = line.substr(first_colon + 1, second_colon - first_colon - 1);
                    std::string confidence_str = line.substr(second_colon + 1, third_colon - second_colon - 1);

                    // Create a struct to pass both the client and the text data
                    struct StreamData {
                        GTKClient* client;
                        std::string name;
                        std::string confidence;
                    };
                    
                    StreamData* data = new StreamData{this, name, confidence_str};
                    
                    // Update stream display in main thread
                    g_idle_add([](gpointer user_data) {
                        StreamData* data = static_cast<StreamData*>(user_data);
                        GTKClient* self = data->client;
                        
                        GtkTextBuffer* buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(self->stream_text));
                        GtkTextIter end;
                        gtk_text_buffer_get_end_iter(buffer, &end);

                        // Limit lines to 50 for performance
                        GtkTextIter start;
                        gint line_count = gtk_text_buffer_get_line_count(buffer);
                        if (line_count > 50) {
                            gtk_text_buffer_get_start_iter(buffer, &start);
                            GtkTextIter line_end = start;
                            gtk_text_iter_forward_line(&line_end);
                            gtk_text_buffer_delete(buffer, &start, &line_end);
                            gtk_text_buffer_get_end_iter(buffer, &end);
                        }

                        // Get current time
                        auto now = std::time(nullptr);
                        auto tm = *std::localtime(&now);
                        std::ostringstream oss;
                        oss << std::put_time(&tm, "%H:%M:%S");
                        std::string timestamp = oss.str();

                        // Format and insert with name and confidence
                        std::string formatted = "[" + timestamp + "] ✓ " + data->name + 
                                              " (Confidence: " + data->confidence + "%)\n";

                        gtk_text_buffer_insert(buffer, &end, formatted.c_str(), -1);

                        // Scroll to end
                        gtk_text_buffer_get_end_iter(buffer, &end);
                        GtkTextMark* mark = gtk_text_buffer_create_mark(buffer, nullptr, &end, FALSE);
                        gtk_text_view_scroll_to_mark(GTK_TEXT_VIEW(self->stream_text), mark, 0, FALSE, 0, 0);

                        delete data;
                        return FALSE;
                    }, data);
                }
            } else if (line.find("NO_FACE:") == 0) {
                g_idle_add([](gpointer user_data) {
                    GTKClient* self = static_cast<GTKClient*>(user_data);
                    GtkTextBuffer* buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(self->stream_text));
                    GtkTextIter end;
                    gtk_text_buffer_get_end_iter(buffer, &end);

                    // Limit lines to 50 for performance
                    gint line_count = gtk_text_buffer_get_line_count(buffer);
                    if (line_count > 50) {
                        GtkTextIter start;
                        gtk_text_buffer_get_start_iter(buffer, &start);
                        GtkTextIter line_end = start;
                        gtk_text_iter_forward_line(&line_end);
                        gtk_text_buffer_delete(buffer, &start, &line_end);
                        gtk_text_buffer_get_end_iter(buffer, &end);
                    }

                    auto now = std::time(nullptr);
                    auto tm = *std::localtime(&now);
                    std::ostringstream oss;
                    oss << std::put_time(&tm, "%H:%M:%S");
                    std::string timestamp = oss.str();

                    std::string formatted = "[" + timestamp + "] - No face detected\n";
                    gtk_text_buffer_insert(buffer, &end, formatted.c_str(), -1);

                    // Scroll to end
                    gtk_text_buffer_get_end_iter(buffer, &end);
                    GtkTextMark* mark = gtk_text_buffer_create_mark(buffer, nullptr, &end, FALSE);
                    gtk_text_view_scroll_to_mark(GTK_TEXT_VIEW(self->stream_text), mark, 0, FALSE, 0, 0);

                    return FALSE;
                }, this);
            }
        }
    }

    // Connection closed or streaming stopped
    if (stream_socket >= 0) {
        close(stream_socket);
        stream_socket = -1;
    }

    // Update UI to reflect disconnection (only if we were streaming)
    g_idle_add([](gpointer user_data) {
        GTKClient* self = static_cast<GTKClient*>(user_data);
        if (self->streaming_active) {
            // Stream was active but disconnected
            self->streaming_active = false;
            gtk_button_set_label(GTK_BUTTON(self->stream_button), "Start Recognition Stream");
            self->set_status_label("Recognition stream disconnected", TRUE);
        }
        // If not active, user already stopped it - no need to update UI
        return FALSE;
    }, this);
}

void GTKClient::on_window_destroy(GtkWidget* /* widget */, gpointer user_data) {
    GTKClient* self = static_cast<GTKClient*>(user_data);
    self->cleanup();
    gtk_main_quit();
}
