#include "ConsoleUI.hpp"
#include <iostream>
#include <sstream>
#include <limits>

ConsoleUI::ConsoleUI(QuantumNode& node, QNetwork& network) 
    : node(node), network(network) {
    network.setMessageHandler([this](const std::string& sender, const std::string& msg) {
        std::cout << "\n[Message from " << sender << "]: " << msg << "\n> ";
        std::cout.flush();
    });
}

void ConsoleUI::run() {
    std::cout << "Quantum Consciousness Network Console\n";
    std::cout << "Type 'help' for available commands\n\n";
    
    if (!network.start()) {
        std::cerr << "Failed to start network!\n";
        return;
    }
    
    while (true) {
        displayMenu();
        
        std::string command;
        std::cout << "> ";
        std::getline(std::cin, command);
        
        if (command == "exit" || command == "quit") {
            break;
        }
        
        processCommand(command);
    }
}

void ConsoleUI::displayMenu() {
    std::cout << "\nCurrent status:\n";
    std::cout << "Network: " << (network.isRunning() ? "Running" : "Stopped") << "\n";
    
    auto peers = network.getConnectedPeers();
    std::cout << "Connected peers: " << peers.size() << "\n";
    std::cout << "Thoughts in superposition: " << node.getCurrentThoughts().size() << "\n\n";
}

void ConsoleUI::processCommand(const std::string& command) {
    if (command == "help") {
        showHelp();
    } else if (command == "enter") {
        enterThoughts();
    } else if (command == "collapse") {
        collapseConsciousness();
    } else if (command == "thoughts") {
        listThoughts();
    } else if (command == "peers") {
        listPeers();
    } else if (command == "send") {
        sendMessage();
    } else if (command == "store") {
        storeMemory();
    } else if (command == "recall") {
        recallMemory();
    } else {
        std::cout << "Unknown command. Type 'help' for available commands.\n";
    }
}

void ConsoleUI::enterThoughts() {
    std::cout << "Enter thoughts (one per line, empty line to finish):\n";
    std::vector<std::string> thoughts;
    std::string thought;
    
    while (true) {
        std::getline(std::cin, thought);
        if (thought.empty()) break;
        thoughts.push_back(thought);
    }
    
    if (!thoughts.empty()) {
        node.enterSuperposition(thoughts);
        std::cout << "Entered " << thoughts.size() << " thoughts into quantum superposition.\n";
    }
}

void ConsoleUI::collapseConsciousness() {
    std::string result = node.collapseConsciousness();
    std::cout << "Consciousness collapsed to: " << result << "\n";
}

void ConsoleUI::listThoughts() {
    auto thoughts = node.getCurrentThoughts();
    if (thoughts.empty()) {
        std::cout << "No thoughts in superposition.\n";
    } else {
        std::cout << "Current thoughts in superposition:\n";
        for (const auto& thought : thoughts) {
            std::cout << "- " << thought << "\n";
        }
    }
}

void ConsoleUI::listPeers() {
    auto peers = network.getConnectedPeers();
    if (peers.empty()) {
        std::cout << "No peers connected. Network may still be discovering nodes.\n";
    } else {
        std::cout << "Connected peers:\n";
        for (const auto& peer : peers) {
            std::cout << "- " << peer.id << " (" 
                      << peer.endpoint.address().to_string() << ":" 
                      << peer.endpoint.port() << ")\n";
        }
    }
}

void ConsoleUI::sendMessage() {
    if (!network.isRunning()) {
        std::cout << "Network not running.\n";
        return;
    }
    
    std::cout << "Enter message: ";
    std::string message;
    std::getline(std::cin, message);
    
    network.broadcast(message);
    std::cout << "Message broadcasted to network.\n";
}

void ConsoleUI::storeMemory() {
    std::cout << "Enter memory fragment: ";
    std::string memory;
    std::getline(std::cin, memory);
    
    node.storeMemoryFragment(memory);
    std::cout << "Memory fragment stored.\n";
}

void ConsoleUI::recallMemory() {
    std::cout << "Enter pattern to recall: ";
    std::string pattern;
    std::getline(std::cin, pattern);
    
    std::string memory = node.recallMemory(pattern);
    if (memory.empty()) {
        std::cout << "No matching memory found.\n";
    } else {
        std::cout << "Recalled memory: " << memory << "\n";
    }
}

void ConsoleUI::showHelp() {
    std::cout << "\nAvailable commands:\n";
    std::cout << "help      - Show this help\n";
    std::cout << "enter     - Enter thoughts into quantum superposition\n";
    std::cout << "collapse  - Collapse quantum state to a single thought\n";
    std::cout << "thoughts  - List current thoughts in superposition\n";
    std::cout << "peers     - List connected network peers\n";
    std::cout << "send      - Send message to network\n";
    std::cout << "store     - Store memory fragment\n";
    std::cout << "recall    - Recall memory by pattern\n";
    std::cout << "exit/quit - Exit the program\n\n";
}