#pragma once

#include "globalfunctions.h"

enum class textBoxSelected {
	ipAdress,
	portNumber,
	playerName,
	None
};

struct errorMessage {
	int timeAdded;
	std::string error;
};

class connectionSetupScreen
{
public:
	connectionSetupScreen() {
		splashScreenTexture.loadFromFile("textures/SplashScreen.png");
		splashScreenSprite.setTexture(splashScreenTexture);
		splashScreenSprite.setPosition(0, 0);
		overlay.setFillColor(sf::Color(95, 205, 228, 102));
		overlay.setSize(sf::Vector2f(1920, 1080));
		overlay.setPosition(0, 0);
		font.loadFromFile("fonts/SatellaRegular.ttf");

		playerNameLabel.setFont(this->font);
		playerNameLabel.setCharacterSize(20);
		playerNameLabel.setPosition(48, 130);
		playerNameLabel.setString("Player Name:");
		playerNameLabel.setFillColor(sf::Color::Yellow);
		playerNameLabel.setOutlineThickness(2);
		playerNameLabel.setOutlineColor(sf::Color::Black);
		playerNameInputField.setFillColor(sf::Color(46, 99, 110, 204));
		playerNameInputField.setSize(sf::Vector2f(600, 25));
		playerNameInputField.setOutlineThickness(2);
		playerNameInputField.setOutlineColor(sf::Color::White);
		playerNameInputField.setPosition(45, 160);
		playerNameText.setFont(this->font);
		playerNameText.setCharacterSize(20);
		playerNameText.setPosition(48, 160);
		playerNameText.setOutlineThickness(2);
		playerNameText.setOutlineColor(sf::Color::Black);


		ipAdressLabel.setFont(this->font);
		ipAdressLabel.setCharacterSize(20);
		ipAdressLabel.setPosition(48, 270);
		ipAdressLabel.setString("Server IPv4 Adress:");
		ipAdressLabel.setFillColor(sf::Color::Yellow);
		ipAdressLabel.setOutlineThickness(2);
		ipAdressLabel.setOutlineColor(sf::Color::Black);
		ipAdressInputField.setFillColor(sf::Color(46, 99, 110, 204));
		ipAdressInputField.setSize(sf::Vector2f(600, 25));
		ipAdressInputField.setOutlineThickness(2);
		ipAdressInputField.setOutlineColor(sf::Color::White);
		ipAdressInputField.setPosition(45, 300);
		ipAdressText.setFont(this->font);
		ipAdressText.setCharacterSize(20);
		ipAdressText.setPosition(48, 300);
		ipAdressText.setOutlineThickness(2);
		ipAdressText.setOutlineColor(sf::Color::Black);
		
		portNumberLabel.setFont(this->font);
		portNumberLabel.setCharacterSize(20);
		portNumberLabel.setPosition(48, 360);
		portNumberLabel.setString("Port number:");
		portNumberLabel.setFillColor(sf::Color::Yellow);
		portNumberLabel.setOutlineThickness(2);
		portNumberLabel.setOutlineColor(sf::Color::Black);
		portNumberInputField.setFillColor(sf::Color(46, 99, 110, 204));
		portNumberInputField.setSize(sf::Vector2f(600, 25));
		portNumberInputField.setOutlineThickness(2);
		portNumberInputField.setOutlineColor(sf::Color::White);
		portNumberInputField.setPosition(45, 390);
		portNumberText.setFont(this->font);
		portNumberText.setCharacterSize(20);
		portNumberText.setPosition(48, 390);
		portNumberText.setOutlineThickness(2);
		portNumberText.setOutlineColor(sf::Color::Black);

		portNumberString = "8756";

		hostLabel.setFont(this->font);
		hostLabel.setCharacterSize(20);
		hostLabel.setPosition(48, 500);
		hostLabel.setString("Host server");
		hostLabel.setFillColor(sf::Color::White);
		hostLabel.setOutlineThickness(2);
		hostLabel.setOutlineColor(sf::Color::Black);
		hostButton.setFillColor(sf::Color(46, 99, 110, 204));
		hostButton.setSize(sf::Vector2f(hostLabel.getGlobalBounds().width+6, 25));
		hostButton.setOutlineThickness(2);
		hostButton.setOutlineColor(sf::Color::White);
		hostButton.setPosition(hostLabel.getGlobalBounds().left-3, hostLabel.getGlobalBounds().top-2);

		joinLabel.setFont(this->font);
		joinLabel.setCharacterSize(20);
		joinLabel.setPosition(48+hostLabel.getGlobalBounds().width+20, 500);
		joinLabel.setString("Join server");
		joinLabel.setFillColor(sf::Color::White);
		joinLabel.setOutlineThickness(2);
		joinLabel.setOutlineColor(sf::Color::Black);
		joinButton.setFillColor(sf::Color(46, 99, 110, 204));
		joinButton.setSize(sf::Vector2f(joinLabel.getGlobalBounds().width + 6, 25));
		joinButton.setOutlineThickness(2);
		joinButton.setOutlineColor(sf::Color::White);
		joinButton.setPosition(joinLabel.getGlobalBounds().left - 3, joinLabel.getGlobalBounds().top-2);

		errorText.setFont(this->font);
		errorText.setCharacterSize(20);
		errorText.setPosition(48, 550);
		errorText.setFillColor(sf::Color::Red);
		errorText.setOutlineThickness(2);
		errorText.setOutlineColor(sf::Color::Black);

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
	void showConnectionSetupScreen();
	void interact();
	void detectMouseOvers(sf::Vector2i& mousePosition);
	void drawConnectionSetupScreen();

private:
	sf::Texture splashScreenTexture, textureMousePointer;
	sf::Sprite splashScreenSprite, spriteMousePointer;
	sf::RectangleShape overlay, ipAdressInputField, portNumberInputField, playerNameInputField, hostButton, joinButton;
	sf::Font font;
	sf::Text ipAdressText, portNumberText, playerNameText, ipAdressLabel, portNumberLabel, playerNameLabel, hostLabel, joinLabel, errorText;
	bool connectionEstablished = false;
	textBoxSelected currentTextBoxSelected = textBoxSelected::None;
	std::string ipAdressString, portNumberString, playerNameString;
	std::vector<errorMessage> errorMessages;
	sf::Clock clock;
	sf::Time currentTime = clock.getElapsedTime();
	sf::Vector2i mousePosition;
};

