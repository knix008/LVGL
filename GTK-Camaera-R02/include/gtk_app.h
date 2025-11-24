#ifndef GTK_APP_H
#define GTK_APP_H

#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <opencv2/opencv.hpp>
#include <thread>
#include <atomic>
#include <mutex>
#include <map>
#include "camera.h"
#include "face_detector.h"
#include "deep_face_recognizer.h"
#include "face_database.h"

class GTKApp {
private:
    // GTK Widgets
    GtkWidget* window;
    GtkWidget* image_widget;
    GtkWidget* toggle_button;
    GtkWidget* train_button;
    GtkWidget* capture_button;
    GtkWidget* status_label;
    GtkWidget* fps_label;
    GtkWidget* face_info_label;
    GtkWidget* face_count_label;
    GtkWidget* error_rate_label;

    // Camera and Face Recognition
    Camera camera;
    FaceDetector face_detector;
    DeepFaceRecognizer face_recognizer;
    FaceDatabase face_database;

    guint refresh_timer;
    bool camera_running;
    bool face_recognition_enabled;
    std::atomic<bool> training_in_progress;
    bool capture_in_progress;
    bool cleanup_done;
    int frame_count;
    gint64 last_time;
    cv::Mat last_frame;
    int capture_count;
    gint64 last_recognition_time;

    // Cache for last recognition result (to display on stream)
    std::string last_recognized_name;
    double last_recognized_confidence;
    bool has_recognition_result;

    // Fixed bounding box size for stable display
    static constexpr int FIXED_BOX_WIDTH = 180;   // Fixed width for face box (reduced 10%)
    static constexpr int FIXED_BOX_HEIGHT = 180;  // Fixed height for face box (reduced 10%)

    // Training thread management
    std::thread training_thread;
    std::atomic<bool> training_success;

    // Face recognition mutex (ONNX Runtime is not thread-safe)
    std::mutex recognition_mutex;

    // Static callback wrappers
    static gboolean on_refresh_timer(gpointer user_data);
    static void on_toggle_button_clicked(GtkWidget* widget, gpointer user_data);
    static void on_train_button_clicked(GtkWidget* widget, gpointer user_data);
    static void on_capture_button_clicked(GtkWidget* widget, gpointer user_data);
    static void on_window_destroy(GtkWidget* widget, gpointer user_data);
    static gboolean on_training_complete(gpointer user_data);

    // Instance methods
    gboolean refresh_frame();
    void toggle_camera();
    void train_model();
    void train_model_async();
    void on_training_finished();
    void capture_photo();
    void update_ui();
    GdkPixbuf* mat_to_pixbuf(const cv::Mat& mat);
    void draw_faces_on_frame(cv::Mat& frame, const std::vector<Face>& faces);
    void load_face_recognizer();

public:
    GTKApp();
    ~GTKApp();

    bool init();
    void run();
    void cleanup();
};

#endif // GTK_APP_H
