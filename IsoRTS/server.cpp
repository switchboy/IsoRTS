#include "server.h"

void server() {
	const int maxClients = 8;
	int clientsConnected = 0;
	sf::TcpListener listener;
	sf::SocketSelector selector;
	bool done = false;
	std::vector<connectedPlayers> clients;
	sf::Clock clock;
	sf::Time serverTime = clock.getElapsedTime();
	sf::Int32 lastPingSent = 0;
	sf::Packet pingPacket;
	sf::Uint8 pingPacketHeader = dataType::Ping;
	pingPacket << pingPacketHeader;

	while (listener.listen(networkStuff::port) != sf::Socket::Done) {
		std::cout << "Failed to open port " << networkStuff::port << std::endl;
		std::cout << "Enter a port for listening" << std::endl;
		std::cin >> networkStuff::port;
		std::cout << "Server starting" << std::endl;
	}
	selector.add(listener);

	while (!done) {
		sf::Time serverTime = clock.getElapsedTime();
		if (selector.wait()) {
			if (selector.isReady(listener)) {
				if (clientsConnected < maxClients) {
					sf::TcpSocket* socket = new sf::TcpSocket;
					listener.accept(*socket);
					sf::Packet packet;
					std::string id;

					if (socket->receive(packet) == sf::Socket::Done) {
						packet >> id;
					}

					clientsConnected++;

					sf::Packet succesFullJoin;
					bool joinSuccesFull = true;
					succesFullJoin << joinSuccesFull;
					socket->send(succesFullJoin);


					sf::Packet newUserJoined;
					int i = -1;
					std::string newUserText = id + " had joined the server!";
					sf::Uint8 header = dataType::Text;
					newUserJoined << header << i << newUserText;
					for (connectedPlayers& client : clients) {
						client.playerSocket->send(newUserJoined);
					}

					newUserJoined.clear();
					std::string welcomeText = "Welcome to the server " + id + "!";
					newUserJoined << header << i << welcomeText;
					socket->send(newUserJoined);
					clients.push_back({ socket, id, socket->getRemoteAddress() , 0, serverTime.asMilliseconds(), false });
					socket->send(pingPacket);
					clients.back().lastPingPacketSend = serverTime.asMilliseconds();
					selector.add(*socket);

					{
						sf::Packet userListPacket;
						sf::Uint8 userListPacketHeader = dataType::UserList;
						userListPacket << userListPacketHeader << static_cast<int>(clients.size());
						for (connectedPlayers& client : clients) {
							userListPacket << client.name;
						}
						for (connectedPlayers& client : clients) {
							client.playerSocket->send(userListPacket);
						}
					}

					{
						sf::Packet sendPacket;
						sf::Uint8 header = dataType::playerReady;
						sendPacket << header;
						for (int i = 0; i < clients.size(); i++) {
							sendPacket << clients[i].isReady;
						}
						for (int j = 0; j < clients.size(); j++) {
							clients[j].playerSocket->send(sendPacket);
						}
					}

				}
				else {
					sf::TcpSocket tempSocket;
					listener.accept(tempSocket);
					sf::Packet succesFullJoin;
					bool joinSuccesFull = false;
					succesFullJoin << joinSuccesFull;
					tempSocket.send(succesFullJoin);
					tempSocket.disconnect();
				}
			}
			else {
				std::vector<int> clientsDisconnected;
				for (int i = 0; i < clients.size(); i++) {
					if (selector.isReady(*clients[i].playerSocket)) {
						sf::Packet packet, sendPacket, confirmPacket;
						if (clients[i].playerSocket->receive(packet) == sf::Socket::Done) {
							sf::Uint8 recievedHeader;
							packet >> recievedHeader;
							switch (recievedHeader) {
							case dataType::Text:
							{
								std::string text;
								packet >> text;
								sf::Uint8 header = dataType::Text;
								sendPacket << header << i << text;
								for (int j = 0; j < clients.size(); j++) {
									clients[j].playerSocket->send(sendPacket);
								}
								sf::Uint8 headerCFM = dataType::MessageStatus;
								bool isSentToAll = true;
								confirmPacket << headerCFM << isSentToAll;
								clients[i].playerSocket->send(confirmPacket);
								break;
							}

							case dataType::Ping:
								clients[i].lastPing = serverTime.asMilliseconds() - clients[i].lastPingPacketSend;
								break;

							case dataType::playerReady:
								packet >> clients[i].isReady;
								sf::Uint8 header = dataType::playerReady;
								sendPacket << header;
								for (int i = 0; i < clients.size(); i++) {
									sendPacket << clients[i].isReady;
								}
								for (int j = 0; j < clients.size(); j++) {
									clients[j].playerSocket->send(sendPacket);
								}
								break;
							}
						}
						else if (clients[i].playerSocket->receive(packet) == sf::Socket::Disconnected) {
							clientsDisconnected.push_back(i);
							sf::Uint8 header = dataType::Text;
							std::string text = clients[i].name + " disconnected!";
							sendPacket << header << -1 << text;
							for (int j = 0; j < clients.size(); j++) {
								if (i != j) {
									clients[j].playerSocket->send(sendPacket);
								}
							}
						}
					}
				}
				for (int& clientId : clientsDisconnected) {
					clients.erase(clients.begin() + clientId);
					clientsConnected--;
					sf::Packet userListPacket;
					sf::Uint8 userListPacketHeader = dataType::UserList;
					userListPacket << userListPacketHeader << static_cast<int>(clients.size());
					for (connectedPlayers& client : clients) {
						userListPacket << client.name;
					}
					for (connectedPlayers& client : clients) {
						client.playerSocket->send(userListPacket);
					}
					sf::Packet sendPacket;
					sf::Uint8 header = dataType::playerReady;
					sendPacket << header;
					for (int i = 0; i < clients.size(); i++) {
						sendPacket << clients[i].isReady;
					}
					for (int j = 0; j < clients.size(); j++) {
						clients[j].playerSocket->send(sendPacket);
					}


				}

				if (serverTime.asMilliseconds() > lastPingSent + 1000) {
					pingPacket.clear();
					pingPacket << pingPacketHeader;
					for (int i = 0; i < clients.size(); i++) {
						pingPacket << clients[i].lastPing;
					}
					for (int j = 0; j < clients.size(); j++) {
						clients[j].playerSocket->send(pingPacket);
						clients[j].lastPingPacketSend = serverTime.asMilliseconds();
					}
					lastPingSent = serverTime.asMilliseconds();
				}
			}
		}
	}
}
