//
// Created by dj on 1/14/26.
//
#include <SFML/Network.hpp>

class Network {
public:
    bool connect(const sf::IpAddress& ip, unsigned short port) {
        return socket.connect(ip, port) == sf::Socket::Status::Done;
    }

    bool send(sf::Packet& packet) {
        return socket.send(packet) == sf::Socket::Status::Done;
    }

    bool receive(sf::Packet& packet) {
        return socket.receive(packet) == sf::Socket::Status::Done;
    }

private:
    sf::TcpSocket socket;
};