#include <SFML/Network.hpp>
#include "Network.h"
//#include <iostream>

Network::~Network() {
    disconnect();
}

// ================= CLIENT =================

bool Network::connect(const sf::IpAddress& address, unsigned short port) {
    sf::Time time = sf::milliseconds(1000);

    if (socket.connect(address, port, time) != sf::Socket::Status::Done) {
        //std::cerr << "Failed to connect to server\n";
        return false;
    }
    socket.setBlocking(false);

    return true;

}

sf::TcpSocket& Network::getSocket() {
    return socket;
}

// ================= SERVER =================

bool Network::listen(unsigned short port) {
    if (listener.listen(port) != sf::Socket::Status::Done) {
        //std::cerr << "Failed to listen on port " << port << "\n";
        return false;
    }
    return true;
}

sf::TcpSocket* Network::acceptClient() {
    auto* client = new sf::TcpSocket();
    socket.setBlocking(false);
    if (listener.accept(*client) != sf::Socket::Status::Done) {
        delete client;
        return nullptr;
    }
    return client;
}

std::optional<std::unique_ptr<sf::TcpSocket>> Network::acceptClientNonBlocking() {
    auto client = std::make_unique<sf::TcpSocket>();
    listener.setBlocking(false);
    if (listener.accept(*client) == sf::Socket::Status::NotReady) {
        return std::nullopt; // no client yet
    }
    return client;
}

// ================= COMMON =================

bool Network::send(sf::TcpSocket& socket, MessageType type, sf::Packet& data) {
    sf::Packet packet;
    packet << static_cast<std::uint8_t>(type);
    const void* dataa = packet.getData();
    std::size_t size = packet.getDataSize();

    const auto* bytes = static_cast<const std::uint8_t*>(dataa);
    for (std::size_t i = 0; i < size; ++i) {
        printf("%02X ", bytes[i]);
    }
    packet.append(data.getData(), data.getDataSize());

    return socket.send(packet) == sf::Socket::Status::Done;
}



bool Network::receive(sf::TcpSocket& socket, MessageType& type, sf::Packet& packet) {
    sf::Socket::Status status = socket.receive(packet);

    if (status == sf::Socket::Status::Done) {
        // extract type first
        std::uint8_t raw;
        packet >> raw;
        type = static_cast<MessageType>(raw);
        return true;
    }
    else if (status == sf::Socket::Status::Disconnected) {
        return false; // client disconnected
    }
    else if (status == sf::Socket::Status::NotReady) {
        return true; // no data yet, keep socket
    }
    else {
        //std::cerr << "Network error on receive: " << static_cast<int>(status) << "\n";
        return false;
    }
}

void debugpacket(sf::Packet& packet) {

}

sf::Socket::Status Network::receiveNonBlocking(
    sf::TcpSocket &socket,
    MessageType &type,
    sf::Packet &packet
) {
    sf::Packet raw;
    sf::Socket::Status status = socket.receive(raw);

    if (status != sf::Socket::Status::Done)
        return status;

    std::uint8_t rawType;
    raw >> rawType;
    type = static_cast<MessageType>(rawType);

    packet = std::move(raw);
    return sf::Socket::Status::Done;
}


std::queue<std::pair<MessageType, sf::Packet>> Network::poll() {
    std::queue<std::pair<MessageType, sf::Packet>> queue;

    sf::Packet packet;
    while (true) {
        MessageType type;
        sf::Socket::Status status = socket.receive(packet);

        if (status == sf::Socket::Status::Done) {
            std::uint8_t raw;
            packet >> raw;
            type = static_cast<MessageType>(raw);
            queue.emplace(type, packet);
            packet.clear();
        }
        else if (status == sf::Socket::Status::NotReady) {
            break; // nothing to read
        }
        else {
            // disconnected or error
            break;
        }
    }

    return queue;
}




void Network::disconnect() {
    socket.disconnect();
    listener.close();
}
