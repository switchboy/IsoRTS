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
	int lastReadyCheck = 0;
	int countdown = 0;
	bool gameStarted = false;
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
					clients.push_back({ socket, id, socket->getRemoteAddress() , 0, serverTime.asMilliseconds(), false, false });
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
						int userId = 0;
						for (connectedPlayers& client : clients) {
							sf::Packet userIdPacket;
							sf::Uint8 userIdPacketHeader = dataType::giveUserId;
							userIdPacket << userIdPacketHeader << userId;
							client.playerSocket->send(userIdPacket);
							userId++;
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
							case dataType::commandPacket:
							{
								for (int j = 0; j < clients.size(); j++) {
									if (i != j) {
										clients[j].playerSocket->send(packet);
									}
								}
								break;
							}
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
							{
								packet >> clients[i].isReady;
								sf::Uint8 header = dataType::playerReady;
								sendPacket << header;
								for (int k = 0; k < clients.size(); k++) {
									sendPacket << clients[k].isReady;
								}
								for (int j = 0; j < clients.size(); j++) {
									clients[j].playerSocket->send(sendPacket);
								}
								break;
							}
							case dataType::mapObjectBlob:
							{
								for (int j = 0; j < clients.size(); j++) {
									clients[j].playerSocket->send(packet);
								}
								break;
							}
							case dataType::actorsBlob:
							{
								for (int j = 0; j < clients.size(); j++) {
									clients[j].playerSocket->send(packet);
								}
								break;
							}
							case dataType::buildingsBlob:
							{
								for (int j = 0; j < clients.size(); j++) {
									clients[j].playerSocket->send(packet);
								}
								break;
							}

							case dataType::objectsBlob:
							{
								for (int j = 0; j < clients.size(); j++) {
									clients[j].playerSocket->send(packet);
								}
								break;
							}
							case dataType::playersBlob:
							{
								for (int j = 0; j < clients.size(); j++) {
									clients[j].playerSocket->send(packet);
								}
								break;
							}

							case dataType::startGame:
							{
								clients[i].fullyLoaded = true;
								bool allReady = true;
								for (connectedPlayers& p : clients) {
									if (p.fullyLoaded == false) {
										allReady = false;
									}
								}
								if (allReady) {
									for (int j = 0; j < clients.size(); j++) {
										clients[j].playerSocket->send(packet);
									}
								}
								break;
							}

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
					int userId = 0;
					for (connectedPlayers& client : clients) {
						sf::Packet userIdPacket;
						sf::Uint8 userIdPacketHeader = dataType::giveUserId;
						userIdPacket << userIdPacketHeader << userId;
						client.playerSocket->send(userIdPacket);
						userId++;
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

				if (!gameStarted) {
					if (lastReadyCheck + 1000 < serverTime.asMilliseconds()) {
						if (clients.size() > 1) {
							bool allReady = true;
							for (connectedPlayers& player : clients) {
								if (!player.isReady) {
									allReady = false;
									break;
								}
							}
							if (allReady) {
								if (countdown < 5) {
									std::string countdownText = "This game will start in " + std::to_string(5 - countdown) + " seconds!";
									sf::Packet countdownPacket;
									int i = -1;
									sf::Uint8 header = dataType::Text;
									countdownPacket << header << i << countdownText;
									for (connectedPlayers& client : clients) {
										client.playerSocket->send(countdownPacket);
									}
									countdown++;
								}
								else{
									sf::Packet startGamePacket;
									sf::Uint8 header = dataType::startGame;
									startGamePacket << header;
									for (connectedPlayers& client : clients) {
										client.playerSocket->send(startGamePacket);
									}
									gameStarted = true;
								}
							}
							else {
								countdown = 0;
							}
						}
						lastReadyCheck = serverTime.asMilliseconds();
					}
				}
			}
		}
	}
}
