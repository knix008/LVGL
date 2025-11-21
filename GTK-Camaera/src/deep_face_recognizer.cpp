#include "deep_face_recognizer.h"
#include <iostream>
#include <filesystem>
#include <algorithm>
#include <cmath>

namespace fs = std::filesystem;

DeepFaceRecognizer::DeepFaceRecognizer() {
    model_loader = std::make_unique<ModelLoader>();
    faiss_index = std::make_unique<FAISSIndex>(128);  // FaceNet uses 128-D embeddings
}

bool DeepFaceRecognizer::load_model(const std::string& onnx_model_path) {
    if (!model_loader) {
        std::cerr << "Error: ModelLoader not initialized" << std::endl;
        return false;
    }

    if (!model_loader->load_model(onnx_model_path)) {
        std::cerr << "Error: Failed to load ONNX model" << std::endl;
        return false;
    }

    // Get the actual embedding dimension from the loaded model
    // For multi-dimensional outputs, use the flattened size
    int embedding_dim = model_loader->get_flattened_output_size();
    std::cout << "Model embedding dimension (flattened): " << embedding_dim << std::endl;

    // Recreate FAISS index with the correct embedding dimension
    faiss_index = std::make_unique<FAISSIndex>(embedding_dim);

    model_path = onnx_model_path;
    std::cout << "Deep learning model loaded successfully" << std::endl;
    return true;
}

void DeepFaceRecognizer::set_database(FaceDatabase* database) {
    db = database;
    load_labels_from_database();
}

cv::Mat DeepFaceRecognizer::preprocess_face(const cv::Mat& face_image) {
    if (face_image.empty()) {
        std::cerr << "Error: Input image is empty" << std::endl;
        return cv::Mat();
    }

    cv::Mat processed = face_image.clone();

    // Convert grayscale to BGR if needed
    if (processed.channels() == 1) {
        cv::cvtColor(processed, processed, cv::COLOR_GRAY2BGR);
    } else if (processed.channels() == 4) {
        cv::cvtColor(processed, processed, cv::COLOR_BGRA2BGR);
    }

    // Resize to match model input (224x224 for FaceNet)
    int target_size = model_loader->get_input_width();
    cv::resize(processed, processed, cv::Size(target_size, target_size));

    return processed;
}

bool DeepFaceRecognizer::validate_face_image(const cv::Mat& image) {
    if (image.empty()) {
        return false;
    }

    // Check minimum size (at least 30x30 for meaningful face recognition)
    if (image.rows < 30 || image.cols < 30) {
        return false;
    }

    return true;
}

std::vector<float> DeepFaceRecognizer::extract_embedding(const cv::Mat& face_image) {
    if (!model_loader || !model_loader->is_model_loaded()) {
        std::cerr << "Error: Model not loaded" << std::endl;
        return std::vector<float>();
    }

    if (!validate_face_image(face_image)) {
        std::cerr << "Error: Invalid face image" << std::endl;
        return std::vector<float>();
    }

    // Preprocess
    cv::Mat processed = preprocess_face(face_image);

    // Extract embedding using ONNX model
    std::vector<float> embedding = model_loader->inference(processed);

    if (embedding.empty()) {
        std::cerr << "Error: Failed to extract embedding" << std::endl;
        return std::vector<float>();
    }

    return embedding;
}

std::vector<std::pair<int, std::vector<float>>>
DeepFaceRecognizer::extract_embeddings_from_directory(const std::string& dataset_path) {
    std::vector<std::pair<int, std::vector<float>>> result;

    if (!fs::exists(dataset_path)) {
        std::cerr << "Error: Dataset path does not exist: " << dataset_path << std::endl;
        return result;
    }

    try {
        // Iterate through person directories
        int person_count = 0;
        int total_images = 0;

        for (const auto& person_dir : fs::directory_iterator(dataset_path)) {
            if (!fs::is_directory(person_dir)) continue;

            std::string person_name = person_dir.path().filename().string();

            // Skip hidden directories
            if (person_name[0] == '.') continue;

            int person_id = register_person(person_name);
            int image_count = 0;

            // Extract embeddings from person's images
            for (const auto& image_file : fs::directory_iterator(person_dir)) {
                if (!fs::is_regular_file(image_file)) continue;

                std::string extension = image_file.path().extension().string();
                std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

                // Check if it's an image file
                if (extension != ".jpg" && extension != ".jpeg" &&
                    extension != ".png" && extension != ".bmp") {
                    continue;
                }

                // Load and extract embedding
                cv::Mat image = cv::imread(image_file.path().string());
                if (image.empty()) {
                    std::cerr << "Warning: Could not load image: " << image_file.path() << std::endl;
                    continue;
                }

                std::vector<float> embedding = extract_embedding(image);
                if (!embedding.empty()) {
                    result.push_back({person_id, embedding});
                    image_count++;
                    total_images++;

                    // Store in database
                    if (db) {
                        std::vector<unsigned char> embedding_bytes(
                            reinterpret_cast<unsigned char*>(embedding.data()),
                            reinterpret_cast<unsigned char*>(embedding.data()) + embedding.size() * sizeof(float)
                        );
                        db->add_face_embedding(person_id, image_file.path().string(), embedding_bytes);
                    }
                }
            }

            if (image_count > 0) {
                person_count++;
                std::cout << "  Person: " << person_name << " (" << person_id << ") - "
                          << image_count << " images" << std::endl;
            }
        }

        std::cout << "Extracted embeddings from " << person_count << " people, "
                  << total_images << " images" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error processing dataset directory: " << e.what() << std::endl;
    }

    return result;
}

bool DeepFaceRecognizer::train_from_images(const std::string& dataset_path) {
    if (!model_loader || !model_loader->is_model_loaded()) {
        std::cerr << "Error: Model not loaded. Call load_model() first." << std::endl;
        return false;
    }

    std::cout << "Starting training from images in: " << dataset_path << std::endl;

    // Extract embeddings from all images
    auto embeddings_data = extract_embeddings_from_directory(dataset_path);

    if (embeddings_data.empty()) {
        std::cerr << "Error: No embeddings extracted from dataset" << std::endl;
        return false;
    }

    // Use batch function to add embeddings
    std::vector<int> person_ids;
    std::vector<std::vector<float>> embeddings;

    for (const auto& [id, emb] : embeddings_data) {
        person_ids.push_back(id);
        embeddings.push_back(emb);
    }

    return train_from_embeddings(person_ids, embeddings);
}

bool DeepFaceRecognizer::train_from_embeddings(const std::vector<int>& person_ids,
                                              const std::vector<std::vector<float>>& embeddings) {
    if (person_ids.empty() || embeddings.empty()) {
        std::cerr << "Error: Empty training data" << std::endl;
        return false;
    }

    if (person_ids.size() != embeddings.size()) {
        std::cerr << "Error: Person IDs and embeddings size mismatch" << std::endl;
        return false;
    }

    try {
        std::cout << "Building FAISS index with " << embeddings.size() << " embeddings..." << std::endl;

        // Build FAISS index
        if (!faiss_index->build_index(embeddings.size())) {
            std::cerr << "Error: Failed to build FAISS index" << std::endl;
            return false;
        }

        // Add all embeddings to index
        if (!faiss_index->add_vectors(person_ids, embeddings)) {
            std::cerr << "Error: Failed to add vectors to FAISS index" << std::endl;
            return false;
        }

        // Save index to disk
        std::string index_path = "models/faiss_index.bin";
        if (!faiss_index->save_index(index_path)) {
            std::cerr << "Warning: Could not save FAISS index to disk" << std::endl;
        }

        is_trained = true;
        std::cout << "Training completed successfully!" << std::endl;
        std::cout << "  Total embeddings: " << embeddings.size() << std::endl;
        std::cout << "  Total people: " << get_num_people() << std::endl;
        std::cout << "  FAISS index clusters: " << faiss_index->get_num_clusters() << std::endl;

        return true;

    } catch (const std::exception& e) {
        std::cerr << "Exception during training: " << e.what() << std::endl;
        return false;
    }
}

bool DeepFaceRecognizer::train_from_database() {
    if (!db) {
        std::cerr << "Error: Database not set" << std::endl;
        return false;
    }

    std::cout << "Training from database embeddings..." << std::endl;

    // Load all embeddings from database
    std::vector<FaceEmbedding> db_embeddings;
    if (!db->get_all_face_embeddings(db_embeddings)) {
        std::cerr << "Error: Failed to load embeddings from database" << std::endl;
        return false;
    }

    if (db_embeddings.empty()) {
        std::cerr << "Error: No embeddings found in database" << std::endl;
        return false;
    }

    // Convert to vectors
    std::vector<int> person_ids;
    std::vector<std::vector<float>> embeddings;

    for (const auto& emb : db_embeddings) {
        person_ids.push_back(emb.person_id);

        // Deserialize embedding
        std::vector<float> embedding_vec(
            reinterpret_cast<const float*>(emb.embedding_data.data()),
            reinterpret_cast<const float*>(emb.embedding_data.data()) + emb.embedding_data.size() / sizeof(float)
        );
        embeddings.push_back(embedding_vec);
    }

    return train_from_embeddings(person_ids, embeddings);
}

bool DeepFaceRecognizer::retrain_model() {
    if (!db) {
        std::cerr << "Error: Database not set" << std::endl;
        return false;
    }

    clear();
    return train_from_database();
}

bool DeepFaceRecognizer::add_training_data(const cv::Mat& face_image, int person_id) {
    if (!model_loader || !model_loader->is_model_loaded()) {
        std::cerr << "Error: Model not loaded" << std::endl;
        return false;
    }

    if (!validate_face_image(face_image)) {
        std::cerr << "Error: Invalid face image" << std::endl;
        return false;
    }

    // Extract embedding from the face image
    std::vector<float> embedding = extract_embedding(face_image);
    if (embedding.empty()) {
        std::cerr << "Error: Failed to extract embedding" << std::endl;
        return false;
    }

    // If index isn't built yet, build it with some initial capacity
    if (!faiss_index->is_index_built()) {
        if (!faiss_index->build_index(1000)) {
            std::cerr << "Error: Failed to build FAISS index" << std::endl;
            return false;
        }
    }

    // Add the embedding to the FAISS index
    if (!faiss_index->add_vector(person_id, embedding)) {
        std::cerr << "Error: Failed to add vector to FAISS index" << std::endl;
        return false;
    }

    // Save embedding to database if available
    if (db) {
        try {
            std::vector<unsigned char> embedding_bytes(
                reinterpret_cast<unsigned char*>(embedding.data()),
                reinterpret_cast<unsigned char*>(embedding.data()) + embedding.size() * sizeof(float)
            );
            db->add_face_embedding(person_id, "", embedding_bytes);
        } catch (const std::exception& e) {
            std::cerr << "Warning: Failed to save to database: " << e.what() << std::endl;
            // Don't fail the operation if DB save fails
        }
    }

    is_trained = (faiss_index->get_num_vectors() > 0);
    std::cout << "Added training data for person " << person_id
              << " - Total vectors: " << faiss_index->get_num_vectors() << std::endl;
    return true;
}

int DeepFaceRecognizer::recognize(const cv::Mat& face_image, double& confidence) {
    if (!is_trained || !faiss_index->is_index_built()) {
        std::cerr << "Error: Model not trained" << std::endl;
        confidence = 0.0;
        return -1;
    }

    // Extract embedding
    std::vector<float> embedding = extract_embedding(face_image);
    if (embedding.empty()) {
        confidence = 0.0;
        return -1;
    }

    // Search FAISS index
    int person_id = faiss_index->search(embedding, confidence);

    // Apply threshold
    if (confidence < confidence_threshold) {
        return -1;  // Unknown
    }

    return person_id;
}

std::string DeepFaceRecognizer::recognize_with_name(const cv::Mat& face_image,
                                                   double& confidence) {
    int person_id = recognize(face_image, confidence);

    if (person_id < 0) {
        return "Unknown";
    }

    return get_label_name(person_id);
}

int DeepFaceRecognizer::register_person(const std::string& name) {
    // Check if person already registered
    auto it = name_to_person_id.find(name);
    if (it != name_to_person_id.end()) {
        return it->second;
    }

    // Find next available ID
    int new_id = 1;
    for (const auto& [id, n] : person_id_to_name) {
        if (id >= new_id) {
            new_id = id + 1;
        }
    }

    // Register in memory
    person_id_to_name[new_id] = name;
    name_to_person_id[name] = new_id;

    // Register in database if available
    if (db) {
        db->add_person(name);
    }

    std::cout << "Registered person: " << name << " (ID: " << new_id << ")" << std::endl;
    return new_id;
}

bool DeepFaceRecognizer::set_label_name(int person_id, const std::string& name) {
    person_id_to_name[person_id] = name;
    name_to_person_id[name] = person_id;
    return true;
}

std::string DeepFaceRecognizer::get_label_name(int person_id) const {
    auto it = person_id_to_name.find(person_id);
    if (it != person_id_to_name.end()) {
        return it->second;
    }
    return "Unknown";
}

int DeepFaceRecognizer::get_label_from_name(const std::string& name) const {
    auto it = name_to_person_id.find(name);
    if (it != name_to_person_id.end()) {
        return it->second;
    }
    return -1;
}

void DeepFaceRecognizer::load_labels_from_database() {
    if (!db) return;

    person_id_to_name.clear();
    name_to_person_id.clear();

    std::vector<PersonRecord> people;
    if (db->get_all_people(people)) {
        for (const auto& person : people) {
            person_id_to_name[person.id] = person.name;
            name_to_person_id[person.name] = person.id;
        }
        std::cout << "Loaded " << people.size() << " people from database" << std::endl;
    }
}

void DeepFaceRecognizer::set_confidence_threshold(double threshold) {
    confidence_threshold = std::max(0.0, std::min(1.0, threshold));
}

int DeepFaceRecognizer::get_num_people() const {
    return person_id_to_name.size();
}

bool DeepFaceRecognizer::is_model_loaded() const {
    return model_loader && model_loader->is_model_loaded();
}

bool DeepFaceRecognizer::save_index(const std::string& filepath) {
    if (!faiss_index) {
        std::cerr << "Error: FAISS index not initialized" << std::endl;
        return false;
    }

    return faiss_index->save_index(filepath);
}

bool DeepFaceRecognizer::load_index(const std::string& filepath) {
    if (!faiss_index) {
        std::cerr << "Error: FAISS index not initialized" << std::endl;
        return false;
    }

    if (!faiss_index->load_index(filepath)) {
        return false;
    }

    is_trained = true;
    return true;
}

void DeepFaceRecognizer::clear() {
    faiss_index->clear();
    person_id_to_name.clear();
    name_to_person_id.clear();
    is_trained = false;
}
