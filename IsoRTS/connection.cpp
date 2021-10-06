#include "connection.h"

bool connection::connect(sf::IpAddress hostIp, int remotePort, std::string playerName)
{
	this->hostIp = hostIp;
	this->remotePort = remotePort;
	this->playerName = playerName;
	if (this->tcpSocket.connect(hostIp, remotePort) != sf::Socket::Error) {

		sf::Packet packet;
		packet << playerName;
		this->tcpSocket.send(packet);

		this->tcpSocket.setBlocking(true);
		bool succesfullJoin;
		sf::Packet joinPacket;
		this->tcpSocket.receive(joinPacket);
		joinPacket >> succesfullJoin;
		if (!succesfullJoin) {
			std::cout << "Could not join server, because the sever is full!" << std::endl;
			std::cout << "Enter ip of the server to connect to" << std::endl;
			return false;
		}
		this->tcpSocket.setBlocking(false);
		return true;
	}
	else {
		return false;
	}
}

sf::TcpSocket* connection::getTcpSocket()
{
	return &this->tcpSocket;
}

bool connection::reconnect()
{
	if (this->tcpSocket.connect(hostIp, remotePort) != sf::Socket::Error) {

		sf::Packet packet;
		packet << playerName;
		this->tcpSocket.send(packet);

		this->tcpSocket.setBlocking(true);
		bool succesfullJoin;
		sf::Packet joinPacket;
		this->tcpSocket.receive(joinPacket);
		joinPacket >> succesfullJoin;
		if (!succesfullJoin) {
			std::cout << "Could not reconnect to server, because the sever is full!" << std::endl;
			std::cout << "Enter ip of the server to connect to" << std::endl;
			return false;
		}
		this->tcpSocket.setBlocking(false);
		return true;
	}
	else {
		return false;
	}
}

std::string connection::getPlayerName()
{
	return this->playerName;
}

connection currentConnection;