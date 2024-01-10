#pragma once
#include "globalfunctions.h"

class connection
{
public:
	connection() {};
	bool connect(sf::IpAddress hostIp, int remotePort, std::string playerName);
	sf::TcpSocket*	getTcpSocket();
	bool reconnect();
	void disconnect();
	bool isConnected();
	std::string getPlayerName();

private:
	sf::TcpSocket	tcpSocket;
	std::string		playerName;
	sf::IpAddress	hostIp;
	int				remotePort = 0;
	bool			connected;

};

extern connection currentConnection;
