#include <iostream>
#include "QuantumNode.hpp"
#include "QNetwork.hpp"
#include "ConsoleUI.hpp"

int main() {
    try {
        // Initialize quantum node
        QuantumNode node;
        
        // Initialize network connection (use port 12345)
        QNetwork network(12345);
        
        // Start console interface
        ConsoleUI ui(node, network);
        ui.run();
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}