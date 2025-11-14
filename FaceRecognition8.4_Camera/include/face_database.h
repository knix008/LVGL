#ifndef FACE_DATABASE_H
#define FACE_DATABASE_H

#include <string>
#include <vector>
#include <map>
#include <opencv2/opencv.hpp>
#include "common.h"

class FaceDatabase {
public:
    FaceDatabase(const std::string& db_path = "./dataset");
    ~FaceDatabase();

    // Initialize database
    bool initialize();

    // Register new person with face image
    bool register_person(const std::string& person_id,
                        const std::string& person_name,
                        const cv::Mat& face_image,
                        const std::vector<float>& embedding);

    // Get person info by ID
    bool get_person_info(const std::string& person_id,
                        std::string& person_name) const;

    // Get all registered persons
    std::vector<std::string> get_all_persons() const;

    // Check if person is registered
    bool is_person_registered(const std::string& person_id) const;

    // Get all embeddings for a person
    std::vector<std::vector<float>> get_person_embeddings(const std::string& person_id) const;

    // Delete person from database
    bool delete_person(const std::string& person_id);

    // Save database to disk
    bool save();

    // Load database from disk
    bool load();

private:
    std::string db_path;
    std::map<std::string, std::string> person_names;  // person_id -> person_name
    std::map<std::string, std::vector<std::vector<float>>> embeddings;  // person_id -> embeddings

    // Helper functions
    std::string get_person_dir(const std::string& person_id) const;
    std::string get_csv_file_path() const;
    bool save_csv();
    bool load_csv();
    std::string escape_csv_field(const std::string& field) const;
    std::string unescape_csv_field(const std::string& field) const;
};

#endif // FACE_DATABASE_H
