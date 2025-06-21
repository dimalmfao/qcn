#ifndef QNETWORK_HPP
#define QNETWORK_HPP

#include <vector>
#include <string>
#include <functional>
#include <thread>
#include <mutex>
#include <asio.hpp>
#include "QuantumNode.hpp"

class QNetwork {
public:
    using MessageHandler = std::function<void(const std::string&, const std::string&)>;
    
    struct PeerNode {
        std::string id;
        asio::ip::udp::endpoint endpoint;
        std::chrono::system_clock::time_point lastSeen;
    };

    QNetwork(unsigned short port);
    ~QNetwork();

    bool start();
    void stop();
    bool isRunning() const;

    void broadcast(const std::string& message);
    void sendToNode(const std::string& nodeId, const std::string& message);

    std::vector<PeerNode> getConnectedPeers() const;
    void syncQuantumState(QuantumNode& node);

    void setMessageHandler(MessageHandler handler);

private:
    void runService();
    void handleReceive(const asio::error_code& error, size_t bytes);
    void discoveryLoop();
    void processMessage(const std::string& msg, const asio::ip::udp::endpoint& sender);

    asio::io_context ioContext;
    asio::ip::udp::socket socket;
    std::thread networkThread;
    std::thread discoveryThread;
    mutable std::mutex peersMutex;
    mutable std::vector<PeerNode> peers;
    MessageHandler messageHandler;
    bool running;
    unsigned short port;
    std::string nodeId;
    asio::ip::udp::endpoint broadcastEndpoint;
    std::array<char, 1024> recvBuffer;
};

#endif // QNETWORK_HPP