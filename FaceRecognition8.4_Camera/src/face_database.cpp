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

    // Load existing data from CSV
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
        // Create person directory for image storage
        std::string person_dir = get_person_dir(person_id);
        if (!fs::exists(person_dir)) {
            fs::create_directories(person_dir);
        }

        // Save face image as PNG
        time_t now = time(nullptr);
        std::string image_filename = person_dir + "/face_" + std::to_string(now) + ".png";
        cv::Mat bgr_image;
        cv::cvtColor(face_image, bgr_image, cv::COLOR_RGB2BGR);
        cv::imwrite(image_filename, bgr_image);

        // Update in-memory data
        person_names[person_id] = person_name;
        embeddings[person_id].push_back(embedding);

        // Save to CSV
        if (!save_csv()) {
            std::cerr << "Failed to save CSV database" << std::endl;
            return false;
        }

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

        // Save updated CSV
        if (!save_csv()) {
            std::cerr << "Failed to save CSV database after deletion" << std::endl;
            return false;
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
        // All data is saved to CSV during registration
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

        // Load data from CSV
        if (!load_csv()) {
            std::cout << "No existing CSV database found, starting fresh" << std::endl;
        }

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

std::string FaceDatabase::get_csv_file_path() const {
    return db_path + "/face_database.csv";
}

std::string FaceDatabase::escape_csv_field(const std::string& field) const {
    // Check if field needs escaping
    if (field.find(',') != std::string::npos ||
        field.find('"') != std::string::npos ||
        field.find('\n') != std::string::npos) {

        std::string escaped = "\"";
        for (char c : field) {
            if (c == '"') {
                escaped += "\"\"";  // Escape quotes by doubling
            } else {
                escaped += c;
            }
        }
        escaped += "\"";
        return escaped;
    }
    return field;
}

std::string FaceDatabase::unescape_csv_field(const std::string& field) const {
    if (field.size() >= 2 && field.front() == '"' && field.back() == '"') {
        std::string unescaped = field.substr(1, field.size() - 2);
        std::string result;
        for (size_t i = 0; i < unescaped.size(); ++i) {
            if (unescaped[i] == '"' && i + 1 < unescaped.size() && unescaped[i + 1] == '"') {
                result += '"';
                ++i;  // Skip next quote
            } else {
                result += unescaped[i];
            }
        }
        return result;
    }
    return field;
}

bool FaceDatabase::save_csv() {
    try {
        std::string csv_file = get_csv_file_path();
        std::ofstream file(csv_file);

        if (!file.is_open()) {
            std::cerr << "Failed to open CSV file for writing: " << csv_file << std::endl;
            return false;
        }

        // Write CSV header
        file << "person_id,person_name,embedding_vector\n";

        // Write person and embedding data
        for (const auto& person_pair : person_names) {
            const std::string& person_id = person_pair.first;
            const std::string& person_name = person_pair.second;

            // Get embeddings for this person
            auto embeddings_it = embeddings.find(person_id);
            if (embeddings_it != embeddings.end()) {
                for (const auto& embedding : embeddings_it->second) {
                    // Convert embedding vector to space-separated string
                    std::string embedding_str;
                    for (size_t i = 0; i < embedding.size(); ++i) {
                        embedding_str += std::to_string(embedding[i]);
                        if (i < embedding.size() - 1) {
                            embedding_str += " ";
                        }
                    }

                    // Write CSV row with escaped fields
                    file << escape_csv_field(person_id) << ","
                         << escape_csv_field(person_name) << ","
                         << escape_csv_field(embedding_str) << "\n";
                }
            }
        }

        file.close();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error saving CSV: " << e.what() << std::endl;
        return false;
    }
}

bool FaceDatabase::load_csv() {
    try {
        std::string csv_file = get_csv_file_path();

        // If CSV file doesn't exist yet, that's okay
        if (!fs::exists(csv_file)) {
            return true;
        }

        std::ifstream file(csv_file);
        if (!file.is_open()) {
            std::cerr << "Failed to open CSV file for reading: " << csv_file << std::endl;
            return false;
        }

        std::string line;
        bool header_skipped = false;

        while (std::getline(file, line)) {
            // Skip header line
            if (!header_skipped) {
                header_skipped = true;
                continue;
            }

            if (line.empty()) continue;

            // Parse CSV line (handle quoted fields)
            std::vector<std::string> fields;
            std::string field;
            bool in_quotes = false;

            for (size_t i = 0; i < line.size(); ++i) {
                char c = line[i];

                if (c == '"') {
                    in_quotes = !in_quotes;
                    field += c;
                } else if (c == ',' && !in_quotes) {
                    fields.push_back(field);
                    field.clear();
                } else {
                    field += c;
                }
            }
            // Add the last field
            if (!field.empty() || line.back() == ',') {
                fields.push_back(field);
            }

            // Parse fields
            if (fields.size() >= 3) {
                std::string person_id = unescape_csv_field(fields[0]);
                std::string person_name = unescape_csv_field(fields[1]);
                std::string embedding_str = unescape_csv_field(fields[2]);

                // Parse embedding vector
                std::vector<float> embedding;
                std::istringstream iss(embedding_str);
                float value;

                while (iss >> value) {
                    embedding.push_back(value);
                }

                // Store in memory
                if (!person_id.empty() && !embedding.empty()) {
                    person_names[person_id] = person_name;
                    embeddings[person_id].push_back(embedding);
                }
            }
        }

        file.close();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error loading CSV: " << e.what() << std::endl;
        return false;
    }
}
