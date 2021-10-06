#pragma once
#include "globalfunctions.h"
#include "connection.h"


class lobbyWindow {
public:
	lobbyWindow() {
		splashScreenTexture.loadFromFile("textures/SplashScreen.png");
		splashScreenSprite.setTexture(splashScreenTexture);
		splashScreenSprite.setPosition(0, 0);
		overlay.setFillColor(sf::Color(95, 205, 228, 102));
		overlay.setSize(sf::Vector2f(1920, 1080));
		overlay.setPosition(0, 0);
		chatWindow.setFillColor(sf::Color(46, 99, 110, 204));
		chatWindow.setSize(sf::Vector2f(1200, 545));
		chatWindow.setPosition(45, 490);
		chatWindow.setOutlineThickness(2);
		playerWindow.setFillColor(sf::Color(46, 99, 110, 204));
		playerWindow.setSize(sf::Vector2f(1200, 400));
		playerWindow.setPosition(45, 45);
		playerWindow.setOutlineThickness(2);
		scrollBar.setFillColor(sf::Color(46, 99, 110, 255));
		scrollBar.setSize(sf::Vector2f(25, 25));
		scrollBar.setOutlineThickness(2);
		scrollBar.setOutlineColor(sf::Color::White);
		up.setRadius(14);
		up.setPointCount(3);
		up.setFillColor(sf::Color(46, 99, 110, 255));
		up.setPosition(1217, 492);
		up.setOutlineThickness(2);
		up.setOutlineColor(sf::Color::White);
		down.setRadius(14);
		down.setPointCount(3);
		down.setFillColor(sf::Color(46, 99, 110, 255));
		down.setPosition(1245, 1006);
		down.setOutlineThickness(2);
		down.setOutlineColor(sf::Color::White);
		down.setRotation(180);
		font.loadFromFile("fonts/SatellaRegular.ttf");
		sendButton.setString("Send");
		sendButton.setFont(this->font);
		sendButton.setCharacterSize(20);
		sendButton.setOutlineColor(sf::Color::Black);
		sendButton.setOutlineThickness(2);
		sendButton.setFillColor(sf::Color::White);
		sendButton.setPosition(1245 - sendButton.getLocalBounds().width, 1010);
		sendTextBackground.setFillColor(sf::Color(46, 99, 110, 204));
		sendTextBackground.setSize(sf::Vector2f(1195 - sendButton.getLocalBounds().width, 25));
		sendTextBackground.setOutlineThickness(2);
		sendTextBackground.setOutlineColor(sf::Color::White);
		sendTextBackground.setPosition(45, 1010);
		sendButtonRect.setFillColor(sf::Color(46, 99, 110, 204));
		sendButtonRect.setSize(sf::Vector2f(sendButton.getLocalBounds().width + 1, 25));
		sendButtonRect.setOutlineThickness(2);
		sendButtonRect.setOutlineColor(sf::Color::White);
		sendButtonRect.setPosition(1243 - sendButton.getLocalBounds().width, 1010);
		readyButton.setString("Ready");
		readyButton.setFont(this->font);
		readyButton.setCharacterSize(45);
		readyButton.setOutlineColor(sf::Color::Black);
		readyButton.setOutlineThickness(2);
		readyButton.setFillColor(sf::Color::White);
		readyButtonRect.setFillColor(sf::Color(46, 99, 110, 204));
		readyButtonRect.setSize({ (readyButton.getGlobalBounds().width + 15), 65 });
		readyButtonRect.setOutlineThickness(2);
		readyButtonRect.setOutlineColor(sf::Color::White);
		readyButtonRect.setPosition(1530, 280);
		readyButton.setPosition(readyButtonRect.getPosition().x + 9, readyButtonRect.getPosition().y + 3);
		readyIcon.setSize(sf::Vector2f(10, 10));
		readyIcon.setOutlineThickness(2);
		readyIcon.setOutlineColor(sf::Color::White);
		pingPacket << pingPacketHeader;

		if (textureMousePointer.loadFromFile("textures/mouseHints.png"))
		{
			spriteMousePointer.setTexture(textureMousePointer);
			spriteMousePointer.setTextureRect(sf::IntRect(0, 0, 32, 32));
		}
		else
		{
			std::cout << "Error loading texture: mouseHints.png \n" << std::endl;
		}
	}
	void drawLobby();
	void detectMouseOvers(sf::Vector2i& mousePosition);
	int moveScrollBar(sf::Vector2i& mousePosition);
	void interact(sf::TcpSocket& socket);
	void pollConnectionAndGetUpdate(sf::TcpSocket& socket);
	void setId(std::string id);
	bool isDone() const;
	void showLobby();
	


private:
	sf::RectangleShape overlay, chatWindow, playerWindow, sendTextBackground, scrollBar, sendButtonRect, readyButtonRect, readyIcon;
	sf::Texture splashScreenTexture, textureMousePointer;
	sf::Sprite splashScreenSprite, spriteMousePointer;
	sf::CircleShape up, down;
	sf::Font font;
	sf::Text sendButton, readyButton;
	int minScrollPosition = 516;
	int maxScrollPosition = 957;
	int scrollSpace = 441;
	int drawableLines = 25;
	int lineOffSetByScrolling = 0;
	bool scrollBarClicked = false;
	bool textAreaSelected = false;
	bool playerReady = false;
	bool connectionLost = false;
	std::vector<sf::Text>chat;
	std::vector<playersClient> playerList;
	std::string text = "";
	std::string id;
	sf::Packet pingPacket;
	sf::Uint8 pingPacketHeader = dataType::Ping;
	sf::Vector2i mousePosition;
	bool done = false;
};



