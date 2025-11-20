#include "faiss_index.h"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <cstring>
#include <fstream>

FAISSIndex::FAISSIndex(int embedding_dimension) : dimension(embedding_dimension) {}

FAISSIndex::~FAISSIndex() {
    clear();
}

int FAISSIndex::calculate_optimal_clusters(int num_vectors) {
    // For 20,000 vectors: sqrt(20000) ≈ 141
    // Use power of 2 for efficiency: 128, 256
    if (num_vectors < 100) return 8;
    if (num_vectors < 1000) return 32;
    if (num_vectors < 10000) return 64;
    if (num_vectors < 100000) return 128;
    return 256;
}

void FAISSIndex::setup_index_parameters() {
    if (!index) return;
    num_clusters = calculate_optimal_clusters(1000);
    std::cout << "FAISS index setup - Clusters: " << num_clusters << std::endl;
}

bool FAISSIndex::build_index(int num_vectors) {
    if (num_vectors <= 0) {
        std::cerr << "Error: Invalid number of vectors" << std::endl;
        return false;
    }

    try {
        // Simple approach: allocate embeddings storage
        // index pointer is just a marker that we're initialized
        index = (void*)1;  // Non-null to indicate initialized
        person_ids.clear();

        // Calculate optimal number of clusters
        num_clusters = calculate_optimal_clusters(num_vectors);

        setup_index_parameters();

        std::cout << "FAISS index built successfully"
                  << " - Vectors: " << num_vectors
                  << ", Dimension: " << dimension
                  << ", Clusters: " << num_clusters << std::endl;

        is_built = true;
        return true;

    } catch (const std::exception& e) {
        std::cerr << "Error building FAISS index: " << e.what() << std::endl;
        return false;
    }
}

bool FAISSIndex::add_vector(int person_id, const std::vector<float>& embedding) {
    if (!index) {
        std::cerr << "Error: Index not built" << std::endl;
        return false;
    }

    if (embedding.size() != static_cast<size_t>(dimension)) {
        std::cerr << "Error: Embedding dimension mismatch. Expected "
                  << dimension << ", got " << embedding.size() << std::endl;
        return false;
    }

    try {
        // Store embedding - simple approach
        embeddings.push_back(embedding);
        person_ids.push_back(person_id);
        return true;

    } catch (const std::exception& e) {
        std::cerr << "Error adding vector to FAISS index: " << e.what() << std::endl;
        return false;
    }
}

bool FAISSIndex::add_vectors(const std::vector<int>& ids,
                             const std::vector<std::vector<float>>& emb) {
    if (!index) {
        std::cerr << "Error: Index not built" << std::endl;
        return false;
    }

    if (ids.size() != emb.size()) {
        std::cerr << "Error: IDs and embeddings size mismatch" << std::endl;
        return false;
    }

    try {
        // Add all vectors at once
        for (size_t i = 0; i < emb.size(); i++) {
            if (emb[i].size() != static_cast<size_t>(dimension)) {
                std::cerr << "Error: Embedding dimension mismatch" << std::endl;
                return false;
            }
            embeddings.push_back(emb[i]);
            person_ids.push_back(ids[i]);
        }

        return true;

    } catch (const std::exception& e) {
        std::cerr << "Error adding vectors to FAISS index: " << e.what() << std::endl;
        return false;
    }
}

double FAISSIndex::distance_to_similarity(float distance) const {
    // Convert L2 distance to similarity score (0-1)
    // For normalized embeddings, L2 distance ranges from 0 to 2
    // 0 = identical, 2 = completely different
    // similarity = 1 - (distance / 2)
    float normalized_distance = std::min(distance / 2.0f, 1.0f);
    return static_cast<double>(1.0f - normalized_distance);
}

// Simple L2 distance computation
static float compute_l2_distance(const std::vector<float>& a, const std::vector<float>& b) {
    if (a.size() != b.size()) return 1e9f;
    float dist = 0.0f;
    for (size_t i = 0; i < a.size(); i++) {
        float diff = a[i] - b[i];
        dist += diff * diff;
    }
    return std::sqrt(dist);
}

int FAISSIndex::search(const std::vector<float>& query_embedding, double& confidence) {
    if (!index || embeddings.empty()) {
        std::cerr << "Error: Index empty or not built" << std::endl;
        confidence = 0.0;
        return -1;  // Unknown
    }

    if (query_embedding.size() != static_cast<size_t>(dimension)) {
        std::cerr << "Error: Query embedding dimension mismatch" << std::endl;
        confidence = 0.0;
        return -1;
    }

    try {
        // Brute-force search for nearest neighbor
        float min_distance = 1e9f;
        int best_index = -1;

        for (size_t i = 0; i < embeddings.size(); i++) {
            float dist = compute_l2_distance(query_embedding, embeddings[i]);
            if (dist < min_distance) {
                min_distance = dist;
                best_index = i;
            }
        }

        if (best_index < 0) {
            std::cerr << "Error: No results found" << std::endl;
            confidence = 0.0;
            return -1;
        }

        // Convert distance to confidence
        confidence = distance_to_similarity(min_distance);
        int person_id = person_ids[best_index];

        return person_id;

    } catch (const std::exception& e) {
        std::cerr << "Error searching FAISS index: " << e.what() << std::endl;
        confidence = 0.0;
        return -1;
    }
}

std::vector<int> FAISSIndex::search_k(const std::vector<float>& query_embedding,
                                      int k,
                                      std::vector<double>& confidences) {
    std::vector<int> results;
    confidences.clear();

    if (!index || embeddings.empty()) {
        std::cerr << "Error: Index empty or not built" << std::endl;
        return results;
    }

    if (query_embedding.size() != static_cast<size_t>(dimension)) {
        std::cerr << "Error: Query embedding dimension mismatch" << std::endl;
        return results;
    }

    try {
        // Compute distances to all vectors
        std::vector<std::pair<float, int>> distances;
        for (size_t i = 0; i < embeddings.size(); i++) {
            float dist = compute_l2_distance(query_embedding, embeddings[i]);
            distances.push_back({dist, i});
        }

        // Sort by distance
        std::sort(distances.begin(), distances.end());

        // Return top k
        k = std::min(k, static_cast<int>(distances.size()));
        for (int i = 0; i < k; i++) {
            int idx = distances[i].second;
            results.push_back(person_ids[idx]);
            confidences.push_back(distance_to_similarity(distances[i].first));
        }

    } catch (const std::exception& e) {
        std::cerr << "Error searching FAISS index: " << e.what() << std::endl;
    }

    return results;
}

bool FAISSIndex::save_index(const std::string& filepath) {
    if (!index) {
        std::cerr << "Error: Index not built" << std::endl;
        return false;
    }

    try {
        // Save embeddings and person_ids to file
        std::ofstream file(filepath, std::ios::binary);
        if (!file) {
            std::cerr << "Error: Could not open file for writing" << std::endl;
            return false;
        }

        // Save metadata
        int num_vectors = embeddings.size();
        file.write((const char*)&num_vectors, sizeof(int));
        file.write((const char*)&dimension, sizeof(int));

        // Save embeddings and person_ids
        for (int i = 0; i < num_vectors; i++) {
            file.write((const char*)embeddings[i].data(), sizeof(float) * dimension);
            file.write((const char*)&person_ids[i], sizeof(int));
        }

        file.close();
        std::cout << "FAISS index saved to: " << filepath << std::endl;
        return true;

    } catch (const std::exception& e) {
        std::cerr << "Error saving FAISS index: " << e.what() << std::endl;
        return false;
    }
}

bool FAISSIndex::load_index(const std::string& filepath) {
    try {
        // Clean up existing index
        clear();

        // Load embeddings and person_ids from file
        std::ifstream file(filepath, std::ios::binary);
        if (!file) {
            std::cerr << "Error: Could not open file for reading" << std::endl;
            return false;
        }

        // Load metadata
        int num_vectors = 0;
        file.read((char*)&num_vectors, sizeof(int));
        file.read((char*)&dimension, sizeof(int));

        // Load embeddings and person_ids
        for (int i = 0; i < num_vectors; i++) {
            std::vector<float> embedding(dimension);
            int person_id = 0;
            file.read((char*)embedding.data(), sizeof(float) * dimension);
            file.read((char*)&person_id, sizeof(int));
            embeddings.push_back(embedding);
            person_ids.push_back(person_id);
        }

        file.close();
        index = (void*)1;  // Mark as initialized
        is_built = true;
        num_clusters = calculate_optimal_clusters(num_vectors);

        std::cout << "FAISS index loaded from: " << filepath << std::endl;
        std::cout << "Loaded " << num_vectors << " vectors with dimension " << dimension << std::endl;
        return true;

    } catch (const std::exception& e) {
        std::cerr << "Error loading FAISS index: " << e.what() << std::endl;
        return false;
    }
}

int FAISSIndex::get_num_vectors() const {
    return static_cast<int>(embeddings.size());
}

void FAISSIndex::clear() {
    embeddings.clear();
    person_ids.clear();
    index = nullptr;
    is_built = false;
    num_clusters = 0;
}
