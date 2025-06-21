#include "QNetwork.hpp"
#include <iostream>
#include <chrono>
#include <random>
#include <asio.hpp>

using namespace std::chrono_literals;

const size_t MAX_PEERS = 50;

QNetwork::QNetwork(unsigned short port) 
    : ioContext(), socket(ioContext), port(port), running(false) {
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 9999);
    nodeId = "node-" + std::to_string(dis(gen));
    
    broadcastEndpoint = asio::ip::udp::endpoint(
        asio::ip::address_v4::broadcast(), port);
}

QNetwork::~QNetwork() {
    stop();
}

bool QNetwork::start() {
    if (running) return true;
    
    try {
        socket.open(asio::ip::udp::v4());
        socket.set_option(asio::ip::udp::socket::reuse_address(true));
        socket.set_option(asio::socket_base::broadcast(true));
        socket.bind(asio::ip::udp::endpoint(asio::ip::udp::v4(), port));
        
        running = true;
        networkThread = std::thread(&QNetwork::runService, this);
        discoveryThread = std::thread(&QNetwork::discoveryLoop, this);
        
        std::cout << "Network started on port " << port << " as " << nodeId << "\n";
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Network error: " << e.what() << "\n";
        return false;
    }
}

void QNetwork::stop() {
    if (!running) return;
    
    running = false;
    ioContext.stop();
    
    if (networkThread.joinable()) networkThread.join();
    if (discoveryThread.joinable()) discoveryThread.join();
    
    if (socket.is_open()) socket.close();
    
    {
        std::lock_guard<std::mutex> lock(peersMutex);
        peers.clear();
    }
    
    ioContext.restart();
    std::cout << "Network stopped\n";
}

bool QNetwork::isRunning() const {
    return running;
}

void QNetwork::broadcast(const std::string& message) {
    if (!running) return;
    std::string fullMsg = "MSG:" + nodeId + ":" + message;
    socket.send_to(asio::buffer(fullMsg), broadcastEndpoint);
}

void QNetwork::sendToNode(const std::string& nodeId, const std::string& message) {
    if (!running) return;
    
    std::lock_guard<std::mutex> lock(peersMutex);
    for (const auto& peer : peers) {
        if (peer.id == nodeId) {
            std::string fullMsg = "MSG:" + this->nodeId + ":" + message;
            socket.send_to(asio::buffer(fullMsg), peer.endpoint);
            return;
        }
    }
    std::cerr << "Node " << nodeId << " not found\n";
}

std::vector<QNetwork::PeerNode> QNetwork::getConnectedPeers() const {
    std::lock_guard<std::mutex> lock(peersMutex);
    auto peersCopy = peers;
    
    auto now = std::chrono::system_clock::now();
    auto it = std::remove_if(peersCopy.begin(), peersCopy.end(), 
        [now](const PeerNode& p) { 
            return (now - p.lastSeen) > 10s; 
        });
    peersCopy.erase(it, peersCopy.end());
    
    return peersCopy;
}

void QNetwork::syncQuantumState(QuantumNode& node) {
    broadcast("SYNC:" + std::to_string(node.getCurrentThoughts().size()));
}

void QNetwork::setMessageHandler(MessageHandler handler) {
    messageHandler = handler;
}

void QNetwork::runService() {
    while (running) {
        try {
            recvBuffer.fill(0);
            socket.async_receive_from(
                asio::buffer(recvBuffer), 
                broadcastEndpoint,
                [this](const asio::error_code& error, size_t bytes) {
                    if (!error && running) {
                        this->handleReceive(error, bytes);
                    }
                });
            
            ioContext.run_one_for(100ms);
            ioContext.reset();
        } catch (const std::exception& e) {
            if (running) {
                std::cerr << "Network error: " << e.what() << "\n";
            }
        }
    }
}

void QNetwork::handleReceive(const asio::error_code& error, size_t bytes) {
    if (error || !running) return;
    
    std::string msg(recvBuffer.data(), bytes);
    processMessage(msg, broadcastEndpoint);
}

void QNetwork::processMessage(const std::string& msg, const asio::ip::udp::endpoint& sender) {
    if (msg.find("DISCOVER:") == 0) {
        std::string peerId = msg.substr(9);
        if (peerId != nodeId) {
            std::lock_guard<std::mutex> lock(peersMutex);
            
            if (peers.size() >= MAX_PEERS) {
                peers.erase(peers.begin());
            }
            
            auto it = std::find_if(peers.begin(), peers.end(), 
                [&peerId](const PeerNode& p) { return p.id == peerId; });
            
            if (it != peers.end()) {
                it->endpoint = sender;
                it->lastSeen = std::chrono::system_clock::now();
            } else {
                peers.push_back({peerId, sender, std::chrono::system_clock::now()});
            }
            
            std::string response = "DISCOVER:" + nodeId;
            socket.send_to(asio::buffer(response), sender);
        }
    } else if (msg.find("MSG:") == 0 && messageHandler) {
        size_t colon1 = msg.find(':', 4);
        if (colon1 != std::string::npos) {
            std::string senderId = msg.substr(4, colon1 - 4);
            std::string content = msg.substr(colon1 + 1);
            messageHandler(senderId, content);
        }
    }
}

void QNetwork::discoveryLoop() {
    while (running) {
        std::string discoverMsg = "DISCOVER:" + nodeId;
        socket.send_to(asio::buffer(discoverMsg), broadcastEndpoint);
        std::this_thread::sleep_for(2s);
    }
}