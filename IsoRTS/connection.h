#pragma once
#include "globalfunctions.h"

class connection
{
public:
	connection() {};
	bool connect(sf::IpAddress hostIp, int remotePort, std::string playerName);
	sf::TcpSocket*	getTcpSocket();
	bool reconnect();
	std::string getPlayerName();

private:
	sf::TcpSocket	tcpSocket;
	std::string		playerName;
	sf::IpAddress	hostIp;
	int				remotePort = 0;

};

extern connection currentConnection;
