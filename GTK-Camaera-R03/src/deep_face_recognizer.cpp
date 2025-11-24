#include "deep_face_recognizer.h"
#include <iostream>
#include <filesystem>
#include <algorithm>
#include <cmath>

namespace fs = std::filesystem;

DeepFaceRecognizer::DeepFaceRecognizer() {
    model_loader = std::make_unique<ModelLoader>();
    faiss_index = std::make_unique<FAISSIndex>(128);  // Will be resized when model loads
    face_detector = std::make_unique<FaceDetector>();
    face_detector->initialize();  // Initialize Haar cascade for face detection
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

    // Resize directly to match model input (112x112 for ArcFace)
    // No padding - ArcFace expects the face to fill the frame
    int target_size = model_loader->get_input_width();
    cv::resize(processed, processed, cv::Size(target_size, target_size), 0, 0, cv::INTER_LINEAR);

    return processed;
}

bool DeepFaceRecognizer::validate_face_image(const cv::Mat& image) {
    if (image.empty()) {
        return false;
    }

    // Check minimum size for reliable recognition (default 80x80 for >70% confidence)
    // Faces smaller than this threshold are rejected to avoid low-confidence recognition
    if (image.rows < min_face_size_for_recognition || image.cols < min_face_size_for_recognition) {
        return false;
    }

    return true;
}

bool DeepFaceRecognizer::is_face_size_sufficient(int width, int height) const {
    return width >= min_face_size_for_recognition && height >= min_face_size_for_recognition;
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

                // Load image
                cv::Mat image = cv::imread(image_file.path().string());
                if (image.empty()) {
                    std::cerr << "Warning: Could not load image: " << image_file.path() << std::endl;
                    continue;
                }

                // Detect faces in the image
                std::vector<Face> detected_faces = face_detector->detect_faces(image);

                if (detected_faces.empty()) {
                    std::cerr << "Warning: No face detected in: " << image_file.path() << std::endl;
                    continue;
                }

                // Use the largest face detected (most likely the main subject)
                cv::Rect best_face = detected_faces[0].bbox;
                for (const auto& face : detected_faces) {
                    if (face.bbox.area() > best_face.area()) {
                        best_face = face.bbox;
                    }
                }

                // Expand the face region slightly to include some context
                int expand_x = static_cast<int>(best_face.width * 0.1);
                int expand_y = static_cast<int>(best_face.height * 0.1);
                cv::Rect expanded_face(
                    std::max(0, best_face.x - expand_x),
                    std::max(0, best_face.y - expand_y),
                    std::min(image.cols - best_face.x + expand_x, best_face.width + 2 * expand_x),
                    std::min(image.rows - best_face.y + expand_y, best_face.height + 2 * expand_y)
                );

                // Crop the face region
                cv::Mat face_crop = image(expanded_face).clone();

                // Extract embedding from cropped face
                std::vector<float> embedding = extract_embedding(face_crop);
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

    // Clear existing FAISS index and embeddings before retraining
    std::cout << "Clearing existing FAISS index..." << std::endl;
    faiss_index->clear();

    // Clear old embeddings from database
    if (db) {
        std::cout << "Clearing old embeddings from database..." << std::endl;
        db->clear_all_embeddings();
    }

    // Clear in-memory label maps (will be rebuilt during extraction)
    person_id_to_name.clear();
    name_to_person_id.clear();

    // Extract embeddings from all images
    // Note: This will call register_person() for each person folder,
    // which adds them to both the database and the label maps
    auto embeddings_data = extract_embeddings_from_directory(dataset_path);

    // Reload label maps from database to ensure consistency
    // This captures any new persons added during extraction
    std::cout << "Reloading label maps from database..." << std::endl;
    load_labels_from_database();

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

        // Debug: Print all person IDs being added to FAISS
        std::cout << "[Training] Person IDs being added to FAISS:" << std::endl;
        for (size_t i = 0; i < person_ids.size(); i++) {
            std::cout << "  Embedding " << i << " -> person_id=" << person_ids[i] << std::endl;
        }

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

        // Save index to disk (in project root directory)
        std::string index_path = "faiss_index.bin";
        if (!faiss_index->save_index(index_path)) {
            std::cerr << "Warning: Could not save FAISS index to disk" << std::endl;
        } else {
            std::cout << "FAISS index saved to: " << index_path << std::endl;
        }

        model_trained = true;

        // CRITICAL: Reload label maps from database after training
        // This ensures person_id -> name mappings are available for recognition
        std::cout << "Reloading label maps from database..." << std::endl;
        load_labels_from_database();

        std::cout << "Training completed successfully!" << std::endl;
        std::cout << "  Total embeddings: " << embeddings.size() << std::endl;
        std::cout << "  Total people: " << get_num_people() << std::endl;
        std::cout << "  FAISS index clusters: " << faiss_index->get_num_clusters() << std::endl;

        // Debug: Print final label map state
        std::cout << "[Training] Final label map state:" << std::endl;
        for (const auto& [id, name] : person_id_to_name) {
            std::cout << "  ID " << id << " -> " << name << std::endl;
        }

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

    model_trained = (faiss_index->get_num_vectors() > 0);
    std::cout << "Added training data for person " << person_id
              << " - Total vectors: " << faiss_index->get_num_vectors() << std::endl;
    return true;
}

int DeepFaceRecognizer::recognize(const cv::Mat& face_image, double& confidence) {
    if (!model_trained || !faiss_index->is_index_built()) {
        std::cerr << "Error: Model not trained" << std::endl;
        confidence = 0.0;
        return -1;
    }

    // Extract embedding
    std::vector<float> embedding = extract_embedding(face_image);
    if (embedding.empty()) {
        std::cerr << "[Recognition] Failed to extract embedding" << std::endl;
        confidence = 0.0;
        return -1;
    }

    // Search FAISS index
    int person_id = faiss_index->search(embedding, confidence);

    // Debug logging with detailed info
    std::cout << "[Recognition] Person ID: " << person_id 
              << ", Raw Confidence: " << confidence
              << " (" << (confidence * 100.0) << "%)"
              << ", Threshold: " << confidence_threshold 
              << " (" << (confidence_threshold * 100.0) << "%)" << std::endl;

    // Apply threshold
    if (confidence < confidence_threshold) {
        std::cout << "[Recognition] BELOW threshold (" << confidence << " < " << confidence_threshold 
                  << ") - returning Unknown" << std::endl;
        confidence = confidence;  // Keep the confidence value for display
        return -1;  // Unknown
    }

    std::string person_name = get_label_name(person_id);
    std::cout << "[Recognition] MATCH FOUND: " << person_name 
              << " with confidence " << (confidence * 100.0) << "%" << std::endl;
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
    // Check if person already registered in memory
    auto it = name_to_person_id.find(name);
    if (it != name_to_person_id.end()) {
        return it->second;
    }

    int new_id = -1;

    // Register in database if available and get the database-assigned ID
    if (db) {
        // First check if person already exists in database
        PersonRecord person;
        if (db->get_person_by_name(name, person)) {
            new_id = person.id;
        } else {
            // Add new person to database
            db->add_person(name);
            // Get the database-assigned ID
            if (db->get_person_by_name(name, person)) {
                new_id = person.id;
            }
        }
    }

    // Fallback: generate ID if database not available
    if (new_id < 0) {
        new_id = 1;
        for (const auto& [id, n] : person_id_to_name) {
            if (id >= new_id) {
                new_id = id + 1;
            }
        }
    }

    // Register in memory with the correct ID
    person_id_to_name[new_id] = name;
    name_to_person_id[name] = new_id;

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
        std::cout << "[Label] Found: person_id=" << person_id << " -> name=" << it->second << std::endl;
        return it->second;
    }
    std::cout << "[Label] NOT FOUND: person_id=" << person_id << " (map has " << person_id_to_name.size() << " entries)" << std::endl;
    for (const auto& [id, name] : person_id_to_name) {
        std::cout << "  - ID " << id << " -> " << name << std::endl;
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
            std::cout << "[DB Load] ID " << person.id << " -> " << person.name << std::endl;
        }
        std::cout << "Loaded " << people.size() << " people from database" << std::endl;
    }
}

void DeepFaceRecognizer::set_confidence_threshold(double threshold) {
    confidence_threshold = std::max(0.0, std::min(1.0, threshold));
}

int DeepFaceRecognizer::get_person_count() const {
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

    // IMPORTANT: Reload label maps from database after loading FAISS index
    // The FAISS index contains person_ids, but the label maps (person_id -> name)
    // need to be loaded from the database to resolve names
    std::cout << "Loading label maps from database after FAISS index load..." << std::endl;
    load_labels_from_database();

    model_trained = true;
    return true;
}

void DeepFaceRecognizer::clear_model() {
    if (faiss_index) {
        faiss_index->clear();
    }
    person_id_to_name.clear();
    name_to_person_id.clear();
    model_trained = false;
}

void DeepFaceRecognizer::clear() {
    clear_model();
}

double DeepFaceRecognizer::compare_embeddings(const std::vector<float>& emb1, 
                                              const std::vector<float>& emb2) {
    if (emb1.size() != emb2.size() || emb1.empty()) {
        return 0.0;
    }

    // Compute L2 distance
    float distance = 0.0f;
    for (size_t i = 0; i < emb1.size(); i++) {
        float diff = emb1[i] - emb2[i];
        distance += diff * diff;
    }
    distance = std::sqrt(distance);

    // Convert to similarity using the same formula as FAISS
    float d_squared = distance * distance;
    float cos_theta = 1.0f - (d_squared / 2.0f);
    cos_theta = std::max(-1.0f, std::min(1.0f, cos_theta));
    double similarity = (1.0 + cos_theta) / 2.0;

    return similarity;
}

std::vector<std::pair<std::string, double>> 
DeepFaceRecognizer::recognize_top_k(const cv::Mat& face_image, int k) {
    std::vector<std::pair<std::string, double>> results;

    if (!model_trained || !faiss_index->is_index_built()) {
        std::cerr << "Error: Model not trained" << std::endl;
        return results;
    }

    // Extract embedding
    std::vector<float> embedding = extract_embedding(face_image);
    if (embedding.empty()) {
        return results;
    }

    // Get top-k matches
    std::vector<double> confidences;
    std::vector<int> person_ids = faiss_index->search_k(embedding, k, confidences);

    // Convert to name-confidence pairs
    for (size_t i = 0; i < person_ids.size(); i++) {
        std::string name = get_label_name(person_ids[i]);
        results.push_back({name, confidences[i]});
        
        std::cout << "[Top-" << (i+1) << "] " << name 
                  << ": " << (confidences[i] * 100.0) << "%" << std::endl;
    }

    return results;
}
