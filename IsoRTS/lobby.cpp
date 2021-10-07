#include "lobby.h"
#include "gamestate.h"

void lobbyWindow::drawLobby() {
	unsigned textLine = 0;
	window.clear(sf::Color::White);
	window.draw(splashScreenSprite);
	window.draw(overlay);
	window.draw(chatWindow);
	window.draw(playerWindow);
	window.draw(sendTextBackground);
	sf::Text playerWindowText("Players in lobby:", font, 20);
	playerWindowText.setFillColor(sf::Color::White);
	playerWindowText.setOutlineColor(sf::Color::Black);
	playerWindowText.setOutlineThickness(2);
	playerWindowText.setPosition(48, 48);
	window.draw(playerWindowText);
	sf::Text playerPingText("Ping:", font, 20);
	playerPingText.setFillColor(sf::Color::White);
	playerPingText.setOutlineColor(sf::Color::Black);
	playerPingText.setOutlineThickness(2);
	playerPingText.setPosition(1136, 48);
	window.draw(playerPingText);
	int playerCounter = 0;
	for (playersClient& player : playerList) {
		readyIcon.setPosition(52.f, 8.f + 68.f + (20.f * static_cast<float>(playerCounter)));
		if (player.isReady) {
			readyIcon.setFillColor(sf::Color::Green);
		}
		else {
			readyIcon.setFillColor(sf::Color::Red);
		}
		window.draw(readyIcon);
		sf::Text playerWindowText(player.name, font, 20);
		playerWindowText.setFillColor(networkTeamColors[playerCounter]);
		playerWindowText.setOutlineColor(sf::Color::Black);
		playerWindowText.setOutlineThickness(2);
		playerWindowText.setPosition(72.f, static_cast<float>(68 + (20 * playerCounter)));
		window.draw(playerWindowText);
		sf::Text playerPingText(std::to_string(player.lastPing) + " ms", font, 20);
		playerPingText.setFillColor(networkTeamColors[playerCounter]);
		playerPingText.setOutlineColor(sf::Color::Black);
		playerPingText.setOutlineThickness(2);
		playerPingText.setPosition(1136.f, static_cast<float>(68 + (20 * playerCounter)));
		window.draw(playerPingText);
		playerCounter++;
		if (connectionLost) {
			sf::Text drawText("CONNECTION LOST!!!", font, 20);
			drawText.setFillColor(sf::Color::Red);
			drawText.setOutlineColor(sf::Color::Black);
			drawText.setOutlineThickness(2);
			drawText.setPosition(3.f, static_cast<float>(textLine));
			window.draw(drawText);
		}
	}

	int lineCounter = 0;
	for (sf::Text chatLine : chat) {
		if (chat.size() < 25 || chat.size() - lineCounter - lineOffSetByScrolling < 25) {
			if (textLine < 25) {
				chatLine.setPosition(48.f, static_cast<float>((textLine * 20) + 490));
				chatLine.setOutlineColor(sf::Color::Black);
				chatLine.setOutlineThickness(2);
				window.draw(chatLine);
				textLine++;
			}
		}
		lineCounter++;
	}

	sf::Text drawText(text, font, 20);
	drawText.setFillColor(sf::Color::Green);
	drawText.setOutlineColor(sf::Color::Black);
	drawText.setOutlineThickness(2);
	drawText.setPosition(48, 1010);
	window.draw(drawText);
	window.draw(sendButtonRect);
	window.draw(sendButton);
	if (playerReady) {
		readyButton.setFillColor(sf::Color::Green);
	}
	else {
		readyButton.setFillColor(sf::Color::Red);
	}
	window.draw(readyButtonRect);
	window.draw(readyButton);
	if (chat.size() > 25) {
		float percentageOfScollMovement = static_cast<float>(lineOffSetByScrolling) / (static_cast<float>(chat.size()) - 25.f);
		float yOffsetScrollbar = static_cast<float>(maxScrollPosition) - (percentageOfScollMovement * static_cast<float>(scrollSpace));
		if (yOffsetScrollbar < minScrollPosition) {
			yOffsetScrollbar = static_cast<float>(minScrollPosition);
		}
		scrollBar.setPosition(1218, floor(yOffsetScrollbar));
		window.draw(scrollBar);
		window.draw(up);
		window.draw(down);
	}

	this->spriteMousePointer.setTextureRect(sf::IntRect(0, 0, 32, 32));
	this->spriteMousePointer.setPosition(static_cast<sf::Vector2f>(this->mousePosition));
	window.draw(this->spriteMousePointer);

	window.display();
}

int lobbyWindow::moveScrollBar(sf::Vector2i& mousePosition)
{
	if (scrollBarClicked) {
		int newPosition = mousePosition.y - 12;
		if (newPosition < minScrollPosition) {
			newPosition = minScrollPosition;
			lineOffSetByScrolling = static_cast<int>(chat.size()) - 24;
		}
		else if (newPosition > maxScrollPosition) {
			newPosition = maxScrollPosition;
			lineOffSetByScrolling = 0;
		}
		else {
			scrollBar.setPosition(scrollBar.getPosition().x, static_cast<float>( newPosition));
			float line = (static_cast<float>(chat.size()) - 25.f) * (static_cast<float>(newPosition) - static_cast<float>(minScrollPosition)) / (static_cast<float>(maxScrollPosition) - static_cast<float>(minScrollPosition));
			lineOffSetByScrolling = static_cast<int>(chat.size()) - 24 - static_cast<int>(line);
		}
	}
	return lineOffSetByScrolling;
}

void lobbyWindow::detectMouseOvers(sf::Vector2i& mousePosition) {
	if (up.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePosition))) {
		up.setFillColor(sf::Color::Yellow);
		down.setFillColor(sf::Color(46, 99, 110, 255));
	}
	else if (down.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePosition))) {
		up.setFillColor(sf::Color(46, 99, 110, 255));
		down.setFillColor(sf::Color::Yellow);
	}
	else {
		up.setFillColor(sf::Color(46, 99, 110, 255));
		down.setFillColor(sf::Color(46, 99, 110, 255));
	}
	if (textAreaSelected) {
		sendTextBackground.setOutlineColor(sf::Color::Yellow);
	}
	else {
		if (sendTextBackground.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePosition))) {
			sendTextBackground.setOutlineColor(sf::Color::Red);
		}
		else {
			sendTextBackground.setOutlineColor(sf::Color::White);
		}
	}

	if (sendButton.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePosition))) {
		sendButton.setFillColor(sf::Color::Yellow);
		sendButtonRect.setOutlineColor(sf::Color::Yellow);
	}
	else {
		sendButton.setFillColor(sf::Color::White);
		sendButtonRect.setOutlineColor(sf::Color::White);
	}

	if (scrollBar.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePosition)) && !scrollBarClicked) {
		scrollBar.setOutlineColor(sf::Color::Yellow);
	}
	else {
		scrollBar.setOutlineColor(sf::Color::White);
	}
}

void lobbyWindow::interact(sf::TcpSocket& socket) {
	mousePosition = sf::Mouse::getPosition(window);
	detectMouseOvers(mousePosition);
	moveScrollBar(mousePosition);
	sf::Event event;
	while (window.pollEvent(event)) {
		switch (event.type) {

		case sf::Event::KeyPressed:
			if (event.key.code == sf::Keyboard::Escape || event.Closed) {
				window.close();
			}
			else if (event.key.code == sf::Keyboard::Return && textAreaSelected) {
				sf::Packet textPacket;
				sf::Uint8 header = dataType::Text;
				textPacket << header << text;
				socket.send(textPacket);
				text = "";
			}
			else if (event.key.code == sf::Keyboard::Up) {
				if (chat.size() >= 25) {
					lineOffSetByScrolling++;
					if (lineOffSetByScrolling > chat.size() - 25) {
						lineOffSetByScrolling = static_cast<int>(chat.size()) - 24;
					}
				}
			}if (event.key.code == sf::Keyboard::Down) {
				if (chat.size() >= 25) {
					lineOffSetByScrolling--;
					if (lineOffSetByScrolling < 0) {
						lineOffSetByScrolling = 0;
					}
				}
			}
			break;

		case event.MouseButtonPressed:
			if (event.mouseButton.button == sf::Mouse::Left) {
				if (scrollBar.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePosition))) {
					scrollBar.setFillColor(sf::Color::Yellow);
					scrollBarClicked = true;
				}
			}
			break;

		case event.MouseButtonReleased:
			if (event.mouseButton.button == sf::Mouse::Left) {
				textAreaSelected = false;
				scrollBar.setFillColor(sf::Color(46, 99, 110, 255));
				scrollBarClicked = false;
				if (sendTextBackground.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePosition))) {
					textAreaSelected = true;
				}
				else if (up.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePosition))) {
					if (chat.size() >= 25) {
						lineOffSetByScrolling++;
						if (lineOffSetByScrolling > chat.size() - 25) {
							lineOffSetByScrolling = static_cast<int>(chat.size()) - 24;
						}
					}
				}
				else if (down.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePosition))) {
					if (chat.size() >= 25) {
						lineOffSetByScrolling--;
						if (lineOffSetByScrolling < 0) {
							lineOffSetByScrolling = 0;
						}
					}
				}
				else if (sendButton.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePosition))) {
					sf::Packet textPacket;
					sf::Uint8 header = dataType::Text;
					textPacket << header << text;
					socket.send(textPacket);
					text = "";
					
				}
				else if (readyButtonRect.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePosition))) {
					sf::Packet readyPacket;
					playerReady = !playerReady;
					sf::Uint8 header = dataType::playerReady;
					readyPacket << header << playerReady;
					socket.send(readyPacket);
				}
			}
			break;

		case sf::Event::MouseWheelMoved:
			if (chat.size() > 25) {
				if (event.mouseWheel.delta > 0) {
					lineOffSetByScrolling++;
					if (lineOffSetByScrolling > chat.size() - 25) {
						lineOffSetByScrolling = static_cast<int>(chat.size()) - 24;
					}
				}
				else {
					lineOffSetByScrolling--;
					if (lineOffSetByScrolling < 0) {
						lineOffSetByScrolling = 0;
					}
				}
			}
			break;

		case sf::Event::TextEntered:
			if (textAreaSelected) {
				if (event.text.unicode != '\b') {
					if (event.text.unicode != '\r') {
						if (sf::Text(id + ": " + text, font, 20).getLocalBounds().width < sendTextBackground.getLocalBounds().width)
							text += event.text.unicode;
					}
				}
				else {
					if (!text.empty()) {
						text.erase(text.size() - 1, 1);
					}
				}
			}
			break;
		}
	}
}


void lobbyWindow::pollConnectionAndGetUpdate(sf::TcpSocket& socket) {
	//do network stuff here
	sf::Packet recievePacket;
	sf::Socket::Status statusOfSocket = socket.receive(recievePacket);
	connectionLost = false;
	if (statusOfSocket == sf::Socket::Disconnected) {
		//complain
		connectionLost = true;
		//do something about it

		/*
		socket.connect(ip, port);
		sf::Packet reconnectPacket;
		reconnectPacket << id;
		socket.send(reconnectPacket);
		socket.setBlocking(false);
		*/

	}
	else if (statusOfSocket == sf::Socket::Done) {
		sf::Uint8 recievedHeader;
		recievePacket >> recievedHeader;
		switch (recievedHeader) {
		case dataType::Text:
		{
			std::string recievedText;
			int userId;
			recievePacket >> userId;
			recievePacket >> recievedText;

			if (userId == -1) {
				sf::Text displayText("*** " + recievedText, font, 20);
				displayText.setFillColor(sf::Color::Yellow);
				chat.push_back(displayText);
			}
			else {
				sf::Text displayText(playerList[userId].name + ": " + recievedText, font, 20);
				displayText.setFillColor(networkTeamColors[userId]);
				chat.push_back(displayText);
			}
			break;
		}
		case dataType::MessageStatus:
			bool relayed;
			recievePacket >> relayed;
			if (relayed) {
				//chat.back().setFillColor(sf::Color::White);
			}
			break;
		case dataType::UserList:
			playerList.clear();
			int numberOfUsers;
			recievePacket >> numberOfUsers;
			for (int i = 0; i < numberOfUsers; i++) {
				std::string tempString;
				recievePacket >> tempString;
				playerList.push_back({ tempString, 0 });
			}
			break;
		case dataType::Ping:
			socket.send(pingPacket);
			for (playersClient& player : playerList) {
				recievePacket >> player.lastPing;
			}
			break;
		case dataType::playerReady:
			for (playersClient& player : playerList) {
				recievePacket >> player.isReady;
			}
		}
	}
}

void lobbyWindow::setId(std::string id)
{
	this->id = id;
}

bool lobbyWindow::isDone() const
{
	return this->done;
}

void lobbyWindow::showLobby()
{
	while (!this->isDone()) {
		this->interact(*currentConnection.getTcpSocket());
		this->pollConnectionAndGetUpdate(*currentConnection.getTcpSocket());
		this->drawLobby();
	}
}
