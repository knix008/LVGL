#include "face_database.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>
#include <ctime>

namespace fs = std::filesystem;

FaceDatabase::FaceDatabase(const std::string& db_path)
    : db_path(db_path), db(nullptr) {
}

FaceDatabase::~FaceDatabase() {
    if (db) {
        sqlite3_close(db);
        db = nullptr;
    }
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

    // Open SQLite database
    std::string db_file = db_path + "/faces.db";
    int rc = sqlite3_open(db_file.c_str(), &db);

    if (rc != SQLITE_OK) {
        std::cerr << "Error opening database: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    // Create database schema
    if (!create_database_schema()) {
        return false;
    }

    // Load existing database
    load();

    std::cout << "Face database initialized at: " << db_path << std::endl;
    std::cout << "SQLite database opened: " << db_file << std::endl;
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

        // Check if person already exists in database
        bool person_exists = is_person_registered(person_id);

        if (!person_exists) {
            // Insert new person into persons table
            std::string sql = "INSERT INTO persons (person_id, person_name) VALUES (?, ?);";
            sqlite3_stmt* stmt;
            int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);

            if (rc != SQLITE_OK) {
                std::cerr << "SQL error preparing insert: " << sqlite3_errmsg(db) << std::endl;
                return false;
            }

            sqlite3_bind_text(stmt, 1, person_id.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 2, person_name.c_str(), -1, SQLITE_STATIC);

            rc = sqlite3_step(stmt);
            sqlite3_finalize(stmt);

            if (rc != SQLITE_DONE) {
                std::cerr << "Error inserting person: " << sqlite3_errmsg(db) << std::endl;
                return false;
            }
        }

        // Save embedding to database
        if (!save_embedding(person_id, embedding)) {
            std::cerr << "Failed to save embedding for person: " << person_id << std::endl;
            return false;
        }

        // Update in-memory person names
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

        // Query all persons from database
        std::string sql = "SELECT person_id, person_name FROM persons;";
        sqlite3_stmt* stmt;
        int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);

        if (rc != SQLITE_OK) {
            std::cerr << "SQL error preparing select: " << sqlite3_errmsg(db) << std::endl;
            return false;
        }

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            const char* person_id = (const char*)sqlite3_column_text(stmt, 0);
            const char* person_name = (const char*)sqlite3_column_text(stmt, 1);

            if (person_id && person_name) {
                person_names[person_id] = person_name;

                // Load embeddings for this person
                load_embeddings_for_person(person_id);
            }
        }

        sqlite3_finalize(stmt);

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
        // Convert embedding vector to space-separated string
        std::string embedding_str;
        for (size_t i = 0; i < embedding.size(); ++i) {
            embedding_str += std::to_string(embedding[i]);
            if (i < embedding.size() - 1) {
                embedding_str += " ";
            }
        }

        // Insert embedding into database
        std::string sql = "INSERT INTO embeddings (person_id, embedding_vector) VALUES (?, ?);";
        sqlite3_stmt* stmt;
        int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);

        if (rc != SQLITE_OK) {
            std::cerr << "SQL error preparing embedding insert: " << sqlite3_errmsg(db) << std::endl;
            return false;
        }

        sqlite3_bind_text(stmt, 1, person_id.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, embedding_str.c_str(), -1, SQLITE_TRANSIENT);

        rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);

        if (rc != SQLITE_DONE) {
            std::cerr << "Error inserting embedding: " << sqlite3_errmsg(db) << std::endl;
            return false;
        }

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
        // Query embeddings from database
        std::string sql = "SELECT embedding_vector FROM embeddings WHERE person_id = ?;";
        sqlite3_stmt* stmt;
        int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);

        if (rc != SQLITE_OK) {
            std::cerr << "SQL error preparing select: " << sqlite3_errmsg(db) << std::endl;
            return false;
        }

        sqlite3_bind_text(stmt, 1, person_id.c_str(), -1, SQLITE_STATIC);

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            const unsigned char* embedding_str = sqlite3_column_text(stmt, 0);
            if (embedding_str) {
                std::vector<float> embedding;
                std::istringstream iss((const char*)embedding_str);
                float value;

                while (iss >> value) {
                    embedding.push_back(value);
                }

                if (!embedding.empty()) {
                    embeddings[person_id].push_back(embedding);
                }
            }
        }

        sqlite3_finalize(stmt);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error loading embeddings: " << e.what() << std::endl;
        return false;
    }
}

bool FaceDatabase::create_database_schema() {
    // Create persons table
    std::string persons_sql = R"(
        CREATE TABLE IF NOT EXISTS persons (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            person_id TEXT UNIQUE NOT NULL,
            person_name TEXT NOT NULL,
            created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
        );
    )";

    // Create embeddings table
    std::string embeddings_sql = R"(
        CREATE TABLE IF NOT EXISTS embeddings (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            person_id TEXT NOT NULL,
            embedding_vector TEXT NOT NULL,
            created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY (person_id) REFERENCES persons(person_id)
        );
    )";

    // Create faces table for metadata
    std::string faces_sql = R"(
        CREATE TABLE IF NOT EXISTS faces (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            person_id TEXT NOT NULL,
            image_path TEXT NOT NULL,
            created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY (person_id) REFERENCES persons(person_id)
        );
    )";

    if (!execute_sql(persons_sql)) {
        return false;
    }

    if (!execute_sql(embeddings_sql)) {
        return false;
    }

    if (!execute_sql(faces_sql)) {
        return false;
    }

    std::cout << "Database schema created successfully" << std::endl;
    return true;
}

bool FaceDatabase::execute_sql(const std::string& sql) {
    char* err_msg = nullptr;
    int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &err_msg);

    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << err_msg << std::endl;
        sqlite3_free(err_msg);
        return false;
    }

    return true;
}
