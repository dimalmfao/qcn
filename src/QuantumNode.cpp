#include "QuantumNode.hpp"
#include <random>
#include <algorithm>
#include <cmath>

QuantumNode::QuantumNode() : quantumState(1) {
    quantumState[0] = 1.0f; // Initial collapsed state
}

void QuantumNode::enterSuperposition(const std::vector<std::string>& thoughts) {
    thoughtSuperposition = thoughts;
    quantumState.resize(thoughts.size());
    quantumState.setConstant(1.0f / std::sqrt(thoughts.size()));
}

std::string QuantumNode::collapseConsciousness() {
    float measurement = measureQuantumState();
    float cumulative = 0.0f;
    size_t selected = 0;
    
    for (size_t i = 0; i < quantumState.size(); ++i) {
        cumulative += quantumState[i] * quantumState[i];
        if (measurement <= cumulative) {
            selected = i;
            break;
        }
    }
    
    // Collapse the state
    quantumState.setZero();
    quantumState[selected] = 1.0f;
    
    return thoughtSuperposition[selected];
}

void QuantumNode::entangleWith(QuantumNode& other) {
    entangledNodes.push_back(&other);
    other.entangledNodes.push_back(this);
    
    // Simple entanglement simulation - sync states
    if (quantumState.size() == other.quantumState.size()) {
        other.quantumState = quantumState;
    }
}

void QuantumNode::storeMemoryFragment(const std::string& memory) {
    memoryFragments.push_back(memory);
}

std::string QuantumNode::recallMemory(const std::string& pattern) {
    for (const auto& mem : memoryFragments) {
        if (mem.find(pattern) != std::string::npos) {
            return mem;
        }
    }
    return "";
}

void QuantumNode::processThoughts() {
    // Simple thought processing - modify quantum state
    for (int i = 0; i < quantumState.size(); ++i) {
        quantumState[i] += 0.1f * (std::rand() / (float)RAND_MAX - 0.5f);
    }
    normalizeState();
}

std::vector<std::string> QuantumNode::getCurrentThoughts() const {
    return thoughtSuperposition;
}

// Private helper methods
void QuantumNode::normalizeState() {
    float norm = quantumState.norm();
    if (norm > 0) {
        quantumState /= norm;
    }
}

float QuantumNode::measureQuantumState() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(0.0f, 1.0f);
    return dis(gen);
}