#ifndef CONSOLE_UI_HPP
#define CONSOLE_UI_HPP

#include "QuantumNode.hpp"
#include "QNetwork.hpp"
#include <string>
#include <vector>

class ConsoleUI {
public:
    ConsoleUI(QuantumNode& node, QNetwork& network);
    void run();

private:
    QuantumNode& node;
    QNetwork& network;

    void displayMenu();
    void processCommand(const std::string& command);
    
    void enterThoughts();
    void collapseConsciousness();
    void listThoughts();
    void listPeers();
    void sendMessage();
    void storeMemory();
    void recallMemory();
    void showHelp();
};

#endif // CONSOLE_UI_HPP