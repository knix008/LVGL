#ifndef GTK_APP_H
#define GTK_APP_H

#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <opencv2/opencv.hpp>
#include "camera.h"
#include "face_detector.h"
#include "face_recognizer.h"
#include "face_database.h"

class GTKApp {
private:
    // GTK Widgets
    GtkWidget* window;
    GtkWidget* image_widget;
    GtkWidget* toggle_button;
    GtkWidget* status_label;
    GtkWidget* fps_label;
    GtkWidget* face_info_label;
    GtkWidget* face_count_label;

    // Camera and Face Recognition
    Camera camera;
    FaceDetector face_detector;
    FaceRecognizer face_recognizer;
    FaceDatabase face_database;

    guint refresh_timer;
    bool camera_running;
    bool face_recognition_enabled;
    int frame_count;
    gint64 last_time;

    // Static callback wrappers
    static gboolean on_refresh_timer(gpointer user_data);
    static void on_toggle_button_clicked(GtkWidget* widget, gpointer user_data);
    static void on_window_destroy(GtkWidget* widget, gpointer user_data);

    // Instance methods
    gboolean refresh_frame();
    void toggle_camera();
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
