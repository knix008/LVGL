#include "face_database.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>
#include <ctime>

namespace fs = std::filesystem;

FaceDatabase::FaceDatabase(const std::string& db_path)
    : db_path(db_path) {
}

FaceDatabase::~FaceDatabase() {
}

bool FaceDatabase::initialize() {
    // Create database directory if it doesn't exist
    if (!fs::exists(db_path)) {
        try {
            fs::create_directories(db_path);
            std::cout << "Created database directory: " << db_path << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Error creating database directory: " << e.what() << std::endl;
            return false;
        }
    }

    // Load existing database
    load();

    std::cout << "Face database initialized at: " << db_path << std::endl;
    return true;
}

bool FaceDatabase::register_person(const std::string& person_id,
                                  const std::string& person_name,
                                  const cv::Mat& face_image,
                                  const std::vector<float>& embedding) {
    if (person_id.empty() || person_name.empty() || face_image.empty() || embedding.empty()) {
        std::cerr << "Invalid person data for registration" << std::endl;
        return false;
    }

    try {
        // Create person directory
        std::string person_dir = get_person_dir(person_id);
        if (!fs::exists(person_dir)) {
            fs::create_directories(person_dir);
        }

        // Save face image
        time_t now = time(nullptr);
        std::string image_filename = person_dir + "/face_" + std::to_string(now) + ".png";
        cv::Mat bgr_image;
        cv::cvtColor(face_image, bgr_image, cv::COLOR_RGB2BGR);
        cv::imwrite(image_filename, bgr_image);

        // Save embedding
        if (!save_embedding(person_id, embedding)) {
            std::cerr << "Failed to save embedding for person: " << person_id << std::endl;
            return false;
        }

        // Update person name
        person_names[person_id] = person_name;

        std::cout << "Person registered successfully: " << person_id
                  << " (" << person_name << ")" << std::endl;

        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error registering person: " << e.what() << std::endl;
        return false;
    }
}

bool FaceDatabase::get_person_info(const std::string& person_id,
                                  std::string& person_name) const {
    auto it = person_names.find(person_id);
    if (it != person_names.end()) {
        person_name = it->second;
        return true;
    }
    return false;
}

std::vector<std::string> FaceDatabase::get_all_persons() const {
    std::vector<std::string> persons;
    for (const auto& pair : person_names) {
        persons.push_back(pair.first);
    }
    return persons;
}

bool FaceDatabase::is_person_registered(const std::string& person_id) const {
    return person_names.find(person_id) != person_names.end();
}

std::vector<std::vector<float>> FaceDatabase::get_person_embeddings(const std::string& person_id) const {
    auto it = embeddings.find(person_id);
    if (it != embeddings.end()) {
        return it->second;
    }
    return std::vector<std::vector<float>>();
}

bool FaceDatabase::delete_person(const std::string& person_id) {
    try {
        // Remove from maps
        person_names.erase(person_id);
        embeddings.erase(person_id);

        // Remove person directory
        std::string person_dir = get_person_dir(person_id);
        if (fs::exists(person_dir)) {
            fs::remove_all(person_dir);
        }

        std::cout << "Person deleted: " << person_id << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error deleting person: " << e.what() << std::endl;
        return false;
    }
}

bool FaceDatabase::save() {
    try {
        // Save person names to CSV
        std::string person_list_file = db_path + "/person_list.csv";
        std::ofstream file(person_list_file);

        if (!file.is_open()) {
            std::cerr << "Failed to open person list file: " << person_list_file << std::endl;
            return false;
        }

        file << "person_id,person_name\n";
        for (const auto& pair : person_names) {
            file << pair.first << "," << pair.second << "\n";
        }

        file.close();
        std::cout << "Database saved successfully" << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error saving database: " << e.what() << std::endl;
        return false;
    }
}

bool FaceDatabase::load() {
    try {
        person_names.clear();
        embeddings.clear();

        // Load person list from CSV
        std::string person_list_file = db_path + "/person_list.csv";
        if (!fs::exists(person_list_file)) {
            std::cout << "No existing person list found" << std::endl;
            return true;  // Not an error if file doesn't exist
        }

        std::ifstream file(person_list_file);
        if (!file.is_open()) {
            std::cerr << "Failed to open person list file: " << person_list_file << std::endl;
            return false;
        }

        std::string line;
        bool first_line = true;

        while (std::getline(file, line)) {
            if (first_line) {
                first_line = false;
                continue;  // Skip header
            }

            if (line.empty()) continue;

            size_t comma_pos = line.find(',');
            if (comma_pos != std::string::npos) {
                std::string person_id = line.substr(0, comma_pos);
                std::string person_name = line.substr(comma_pos + 1);
                person_names[person_id] = person_name;

                // Load embeddings for this person
                load_embeddings_for_person(person_id);
            }
        }

        file.close();
        std::cout << "Database loaded successfully. Found " << person_names.size()
                  << " registered persons" << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error loading database: " << e.what() << std::endl;
        return false;
    }
}

std::string FaceDatabase::get_person_dir(const std::string& person_id) const {
    return db_path + "/" + person_id;
}

bool FaceDatabase::save_embedding(const std::string& person_id,
                                 const std::vector<float>& embedding) {
    try {
        std::string embedding_dir = get_person_dir(person_id);
        std::string embedding_file = embedding_dir + "/embeddings.txt";

        std::ofstream file(embedding_file, std::ios::app);
        if (!file.is_open()) {
            std::cerr << "Failed to open embedding file: " << embedding_file << std::endl;
            return false;
        }

        // Save embedding as space-separated values
        for (size_t i = 0; i < embedding.size(); ++i) {
            file << embedding[i];
            if (i < embedding.size() - 1) {
                file << " ";
            }
        }
        file << "\n";

        file.close();

        // Update in-memory embeddings
        embeddings[person_id].push_back(embedding);

        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error saving embedding: " << e.what() << std::endl;
        return false;
    }
}

bool FaceDatabase::load_embeddings_for_person(const std::string& person_id) {
    try {
        std::string embedding_file = get_person_dir(person_id) + "/embeddings.txt";

        if (!fs::exists(embedding_file)) {
            return true;  // No embeddings file yet
        }

        std::ifstream file(embedding_file);
        if (!file.is_open()) {
            std::cerr << "Failed to open embedding file: " << embedding_file << std::endl;
            return false;
        }

        std::string line;
        while (std::getline(file, line)) {
            if (line.empty()) continue;

            std::vector<float> embedding;
            std::istringstream iss(line);
            float value;

            while (iss >> value) {
                embedding.push_back(value);
            }

            if (!embedding.empty()) {
                embeddings[person_id].push_back(embedding);
            }
        }

        file.close();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error loading embeddings: " << e.what() << std::endl;
        return false;
    }
}
