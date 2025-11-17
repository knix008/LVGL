#include "face_database.h"
#include <iostream>
#include <ctime>
#include <iomanip>
#include <sstream>

FaceDatabase::FaceDatabase(const std::string& path) : db_path(path) {}

FaceDatabase::~FaceDatabase() {
    close();
}

std::string get_timestamp() {
    auto now = std::time(nullptr);
    auto tm = *std::localtime(&now);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

bool FaceDatabase::open() {
    if (is_open) return true;

    int rc = sqlite3_open(db_path.c_str(), &db);
    if (rc) {
        std::cerr << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    is_open = true;
    std::cout << "Database opened: " << db_path << std::endl;
    return true;
}

bool FaceDatabase::close() {
    if (!is_open || !db) return true;

    if (sqlite3_close(db) != SQLITE_OK) {
        std::cerr << "Cannot close database: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    is_open = false;
    db = nullptr;
    return true;
}

bool FaceDatabase::initialize() {
    if (!is_open) return false;

    try {
        // Create people table
        const char* sql_people = R"(
            CREATE TABLE IF NOT EXISTS people (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                name TEXT UNIQUE NOT NULL,
                face_count INTEGER DEFAULT 0,
                created_at TEXT,
                updated_at TEXT
            )
        )";

        if (!execute_sql(sql_people)) {
            std::cerr << "Failed to create people table" << std::endl;
            return false;
        }

        // Create face_images table
        const char* sql_images = R"(
            CREATE TABLE IF NOT EXISTS face_images (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                person_id INTEGER NOT NULL,
                image_path TEXT UNIQUE NOT NULL,
                created_at TEXT,
                FOREIGN KEY (person_id) REFERENCES people(id) ON DELETE CASCADE
            )
        )";

        if (!execute_sql(sql_images)) {
            std::cerr << "Failed to create face_images table" << std::endl;
            return false;
        }

        std::cout << "Database initialized successfully" << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Exception in initialize: " << e.what() << std::endl;
        return false;
    }
}

bool FaceDatabase::execute_sql(const std::string& sql) {
    if (!is_open || !db) return false;

    char* err_msg = nullptr;
    int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &err_msg);

    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << err_msg << std::endl;
        sqlite3_free(err_msg);
        return false;
    }

    return true;
}

bool FaceDatabase::execute_query(const std::string& sql, std::vector<std::map<std::string, std::string>>& results) {
    if (!is_open || !db) return false;

    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare SQL: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    results.clear();

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::map<std::string, std::string> row;

        int col_count = sqlite3_column_count(stmt);
        for (int i = 0; i < col_count; ++i) {
            const char* col_name = sqlite3_column_name(stmt, i);
            const unsigned char* col_value = sqlite3_column_text(stmt, i);

            row[col_name] = col_value ? reinterpret_cast<const char*>(col_value) : "";
        }

        results.push_back(row);
    }

    sqlite3_finalize(stmt);
    return true;
}

bool FaceDatabase::add_person(const std::string& name) {
    if (!is_open || !db) return false;

    try {
        std::string timestamp = get_timestamp();
        const char* sql = "INSERT INTO people (name, created_at, updated_at) VALUES (?, ?, ?)";
        sqlite3_stmt* stmt;

        int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            std::cerr << "Failed to prepare SQL statement: " << sqlite3_errmsg(db) << std::endl;
            return false;
        }

        sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, timestamp.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, timestamp.c_str(), -1, SQLITE_STATIC);

        rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);

        if (rc != SQLITE_DONE) {
            std::cerr << "Failed to add person: " << name << " - " << sqlite3_errmsg(db) << std::endl;
            return false;
        }

        std::cout << "Person added: " << name << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Exception in add_person: " << e.what() << std::endl;
        return false;
    }
}

bool FaceDatabase::get_person(int id, PersonRecord& person) {
    if (!is_open) return false;

    try {
        std::string sql = "SELECT * FROM people WHERE id = " + std::to_string(id);
        std::vector<std::map<std::string, std::string>> results;

        if (!execute_query(sql, results) || results.empty()) {
            return false;
        }

        const auto& row = results[0];
        person.id = std::stoi(row.at("id"));
        person.name = row.at("name");
        person.face_count = std::stoi(row.at("face_count"));
        person.created_at = row.at("created_at");
        person.updated_at = row.at("updated_at");

        return true;
    } catch (const std::exception& e) {
        std::cerr << "Exception in get_person: " << e.what() << std::endl;
        return false;
    }
}

bool FaceDatabase::get_person_by_name(const std::string& name, PersonRecord& person) {
    if (!is_open || !db) return false;

    try {
        const char* sql = "SELECT id, name, face_count, created_at, updated_at FROM people WHERE name = ?";
        sqlite3_stmt* stmt;

        int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            std::cerr << "Failed to prepare SQL statement: " << sqlite3_errmsg(db) << std::endl;
            return false;
        }

        sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
        rc = sqlite3_step(stmt);

        if (rc != SQLITE_ROW) {
            sqlite3_finalize(stmt);
            return false;
        }

        person.id = sqlite3_column_int(stmt, 0);
        person.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        person.face_count = sqlite3_column_int(stmt, 2);
        person.created_at = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        person.updated_at = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));

        sqlite3_finalize(stmt);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Exception in get_person_by_name: " << e.what() << std::endl;
        return false;
    }
}

bool FaceDatabase::get_all_people(std::vector<PersonRecord>& people) {
    if (!is_open) return false;

    try {
        std::string sql = "SELECT * FROM people ORDER BY name";
        std::vector<std::map<std::string, std::string>> results;

        if (!execute_query(sql, results)) {
            return false;
        }

        people.clear();
        for (const auto& row : results) {
            PersonRecord person;
            person.id = std::stoi(row.at("id"));
            person.name = row.at("name");
            person.face_count = std::stoi(row.at("face_count"));
            person.created_at = row.at("created_at");
            person.updated_at = row.at("updated_at");
            people.push_back(person);
        }

        return true;
    } catch (const std::exception& e) {
        std::cerr << "Exception in get_all_people: " << e.what() << std::endl;
        return false;
    }
}

bool FaceDatabase::add_face_image(int person_id, const std::string& image_path) {
    if (!is_open || !db) return false;

    try {
        std::string timestamp = get_timestamp();
        const char* sql = "INSERT INTO face_images (person_id, image_path, created_at) VALUES (?, ?, ?)";
        sqlite3_stmt* stmt;

        int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            std::cerr << "Failed to prepare SQL statement: " << sqlite3_errmsg(db) << std::endl;
            return false;
        }

        sqlite3_bind_int(stmt, 1, person_id);
        sqlite3_bind_text(stmt, 2, image_path.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, timestamp.c_str(), -1, SQLITE_STATIC);

        rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);

        if (rc != SQLITE_DONE) {
            std::cerr << "Failed to add face image: " << sqlite3_errmsg(db) << std::endl;
            return false;
        }

        std::cout << "Face image added: " << image_path << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Exception in add_face_image: " << e.what() << std::endl;
        return false;
    }
}

bool FaceDatabase::get_face_images(int person_id, std::vector<std::string>& image_paths) {
    if (!is_open) return false;

    try {
        std::string sql = "SELECT image_path FROM face_images WHERE person_id = " +
                         std::to_string(person_id) + " ORDER BY created_at";
        std::vector<std::map<std::string, std::string>> results;

        if (!execute_query(sql, results)) {
            return false;
        }

        image_paths.clear();
        for (const auto& row : results) {
            image_paths.push_back(row.at("image_path"));
        }

        return true;
    } catch (const std::exception& e) {
        std::cerr << "Exception in get_face_images: " << e.what() << std::endl;
        return false;
    }
}

bool FaceDatabase::get_all_face_images(std::vector<std::pair<int, std::string>>& images) {
    if (!is_open) return false;

    try {
        std::string sql = "SELECT person_id, image_path FROM face_images ORDER BY created_at";
        std::vector<std::map<std::string, std::string>> results;

        if (!execute_query(sql, results)) {
            return false;
        }

        images.clear();
        for (const auto& row : results) {
            int person_id = std::stoi(row.at("person_id"));
            std::string image_path = row.at("image_path");
            images.push_back({person_id, image_path});
        }

        return true;
    } catch (const std::exception& e) {
        std::cerr << "Exception in get_all_face_images: " << e.what() << std::endl;
        return false;
    }
}

bool FaceDatabase::update_person(int id, const std::string& name) {
    if (!is_open) return false;

    try {
        std::string timestamp = get_timestamp();
        std::string sql = "UPDATE people SET name = '" + name + "', updated_at = '" +
                         timestamp + "' WHERE id = " + std::to_string(id);

        return execute_sql(sql);
    } catch (const std::exception& e) {
        std::cerr << "Exception in update_person: " << e.what() << std::endl;
        return false;
    }
}

bool FaceDatabase::delete_person(int id) {
    if (!is_open) return false;

    try {
        std::string sql = "DELETE FROM people WHERE id = " + std::to_string(id);
        return execute_sql(sql);
    } catch (const std::exception& e) {
        std::cerr << "Exception in delete_person: " << e.what() << std::endl;
        return false;
    }
}

bool FaceDatabase::delete_face_image(const std::string& image_path) {
    if (!is_open) return false;

    try {
        std::string sql = "DELETE FROM face_images WHERE image_path = '" + image_path + "'";
        return execute_sql(sql);
    } catch (const std::exception& e) {
        std::cerr << "Exception in delete_face_image: " << e.what() << std::endl;
        return false;
    }
}

bool FaceDatabase::person_exists(const std::string& name) {
    PersonRecord person;
    return get_person_by_name(name, person);
}

bool FaceDatabase::is_open_connection() const {
    return is_open && db != nullptr;
}

int FaceDatabase::get_total_faces() const {
    if (!is_open || !db) return 0;

    sqlite3_stmt* stmt = nullptr;
    int count = 0;

    const char* sql = "SELECT COUNT(*) FROM face_images";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            count = sqlite3_column_int(stmt, 0);
        }
    }

    sqlite3_finalize(stmt);
    return count;
}

int FaceDatabase::get_faces_per_person(int person_id) const {
    if (!is_open || !db) return 0;

    sqlite3_stmt* stmt = nullptr;
    int count = 0;

    std::string sql = "SELECT COUNT(*) FROM face_images WHERE person_id = " + std::to_string(person_id);
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            count = sqlite3_column_int(stmt, 0);
        }
    }

    sqlite3_finalize(stmt);
    return count;
}

int FaceDatabase::get_num_people() const {
    if (!is_open || !db) return 0;

    sqlite3_stmt* stmt = nullptr;
    int count = 0;

    const char* sql = "SELECT COUNT(*) FROM people";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            count = sqlite3_column_int(stmt, 0);
        }
    }

    sqlite3_finalize(stmt);
    return count;
}
