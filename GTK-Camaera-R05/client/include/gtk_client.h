#ifndef GTK_CLIENT_H
#define GTK_CLIENT_H

#include <gtk/gtk.h>
#include <memory>
#include <thread>
#include <atomic>
#include "socket_client_lib.h"

/**
 * @brief GTK Client Application for Face Recognition Server Control
 *
 * Provides a GUI interface to control the face recognition server
 * via Unix domain socket commands.
 */
class GTKClient {
private:
    // Main window and widgets
    GtkWidget* window;
    GtkWidget* status_label;
    GtkWidget* response_text;
    GtkWidget* initial_entry;
    GtkWidget* id_entry;

    // Buttons
    GtkWidget* camera_on_button;
    GtkWidget* camera_off_button;
    GtkWidget* capture_button;
    GtkWidget* registering_button;
    GtkWidget* status_button;
    GtkWidget* clear_button;
    GtkWidget* stream_button;
    GtkWidget* stream_text;

    // Socket client
    std::unique_ptr<SocketClientLib> socket_client;

    // Streaming thread
    std::thread stream_thread;
    std::atomic<bool> streaming_active;
    int stream_socket;

    // Static callback wrappers
    static void on_camera_on_clicked(GtkWidget* widget, gpointer user_data);
    static void on_camera_off_clicked(GtkWidget* widget, gpointer user_data);
    static void on_capture_clicked(GtkWidget* widget, gpointer user_data);
    static void on_registering_clicked(GtkWidget* widget, gpointer user_data);
    static void on_status_clicked(GtkWidget* widget, gpointer user_data);
    static void on_clear_clicked(GtkWidget* widget, gpointer user_data);
    static void on_stream_clicked(GtkWidget* widget, gpointer user_data);
    static void on_window_destroy(GtkWidget* widget, gpointer user_data);

    // Instance methods for handlers
    void handle_camera_on();
    void handle_camera_off();
    void handle_capture();
    void handle_registering();
    void handle_status();
    void handle_clear();
    void handle_stream_toggle();

    // Streaming helper
    void stream_recognition_loop();
    static gboolean update_stream_display(gpointer user_data);

    // UI helper methods
    void append_response(const std::string& title, const SocketClientLib::Response& response);
    void set_status_label(const std::string& message, gboolean is_error);
    void create_ui();

public:
    GTKClient();
    ~GTKClient();

    /**
     * @brief Initialize the client application
     * @return true if successful
     */
    bool init();

    /**
     * @brief Run the GTK main loop
     */
    void run();

    /**
     * @brief Clean up resources
     */
    void cleanup();
};

#endif // GTK_CLIENT_H
