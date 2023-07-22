#include "splashScreen.h"
#include <SFML/Graphics.hpp>
#include "gamestate.h"



void loadSplashScreen()
{
	if (splashScreenTexture.loadFromFile("textures/SplashScreen.png"))
	{
		splashScreenSprite.setTexture(splashScreenTexture);
	}
	else
	{
		std::cout << "Error loading texture: textures/SplashScreen.png \n" << std::endl;
	}
	
}

void splashScreen()
{
	currentGame.loadFonts();
	setViewports();
	loadSplashScreen();
	bool keypressed = false;
	sf::Event currentEvent;
	currentGame.text.setCharacterSize(64);
	currentGame.text.setOutlineColor(sf::Color::Red);
	currentGame.text.setOutlineThickness(2.f);
	currentGame.text.setFillColor(sf::Color::Yellow);
	currentGame.text.setString("Press any key to continue...");
	sf::FloatRect textRect = currentGame.text.getLocalBounds();
	currentGame.text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
	currentGame.text.setPosition(static_cast<float>(mainWindowWidth / 2), static_cast<float>(mainWindowHeigth-72));
	sf::Clock splashTime;
	sf::Time lastRefresh = splashTime.getElapsedTime();
	bool displayText = false;

	while (!keypressed && window.isOpen()) {
		while (window.pollEvent(currentEvent))
		{
			switch (currentEvent.type) {
			case sf::Event::Closed:
				window.close();
				break;
			case sf::Event::KeyPressed:
				keypressed = true;
				break;
			case sf::Event::MouseButtonReleased:
				keypressed = true;
			}
		}
		window.clear();
		window.draw(splashScreenSprite);

		if (splashTime.getElapsedTime().asSeconds() > lastRefresh.asSeconds() + 1) {
			displayText = !displayText;
			lastRefresh = splashTime.getElapsedTime();
		}
		if (displayText) {
			window.draw(currentGame.text);
		}
		window.display();
	}
}
