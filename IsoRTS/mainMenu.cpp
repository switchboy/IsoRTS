#include "mainMenu.h"
#include "globalfunctions.h"
#include <SFML/Graphics.hpp>
#include "gamestate.h"

void menu::addMenuItem(menuItemNames id, std::string name, int menuLevel)
{
	this->menuItems.push_back({ id, name, menuLevel });
}

void menu::displayMenu()
{
	window.clear();
	this->spriteMousePointer.setTextureRect(sf::IntRect(0, 0, 32, 32));
	this->spriteMousePointer.setPosition(this->mousePosition);
	window.draw(this->spriteMousePointer);
	window.display();
}

void menu::interactMenu()
{
	this->mousePosition = static_cast<sf::Vector2f>(sf::Mouse::getPosition(window));
	sf::Event currentEvent;
	while (window.pollEvent(currentEvent))
	{
		switch (currentEvent.type) {
		case sf::Event::Closed:
			window.close();
			break;
		case sf::Event::LostFocus:
			this->focus = false;
			break;
		case sf::Event::GainedFocus:
			this->focus = true;
			break;
		case sf::Event::MouseButtonReleased:
			if (currentEvent.mouseButton.button == sf::Mouse::Left) {
				//Check if a button was pressed TBI
			}	
		}
	}
}

int menu::getMenuLevel()
{
	return this->menuLevel;
}

void menu::setMenuLevel(int level)
{
	this->menuLevel = level;
}

void menu::preformMenuAction(menuItemNames itemClicked)
{
	switch (itemClicked) {
	case newGame:
		this->setMenuLevel(1);
		break;
	case multiplayerGame:
		break;
	case sandBoxGame:
		AIPlayers = 0;
		this->setMenuLevel(-1);
		break;
	case skirmishGame:
		this->setMenuLevel(2);
		break;
	case oneAIPlayers:
		AIPlayers = 1;
		this->setMenuLevel(-1);
		break;
	case twoAIPlayers:
		AIPlayers = 2;
		this->setMenuLevel(-1);
		break;
	case threeAIPlayers:
		AIPlayers = 3;
		this->setMenuLevel(-1);
		break;
	case fourAIPlayers:
		AIPlayers = 4;
		this->setMenuLevel(-1);
		break;
	case fiveAIPlayers:
		AIPlayers = 5;
		this->setMenuLevel(-1);
		break;
	case sixAIPlayers:
		AIPlayers = 6;
		this->setMenuLevel(-1);
		break;
	case sevenAIPlayers:
		AIPlayers = 7;
		this->setMenuLevel(-1);
		break;
	case showCredits:
		break;
	case showTechTree:
		break;
	case exitGame:
		window.close();
		break;
	}
}

void createMainMenuItems(menu& mainMenu)
{
	mainMenu.addMenuItem(newGame,			"New Game",				0);
	mainMenu.addMenuItem(multiplayerGame,	"Multiplayer Game",		0);
	mainMenu.addMenuItem(sandBoxGame,		"Sandbox Game",			1);
	mainMenu.addMenuItem(skirmishGame,		"Skirmish Game",		1);
	mainMenu.addMenuItem(oneAIPlayers,		"One AI player",		2);
	mainMenu.addMenuItem(twoAIPlayers,		"Two AI players",		2);
	mainMenu.addMenuItem(threeAIPlayers,	"Three AI players",		2);
	mainMenu.addMenuItem(fourAIPlayers,		"Four AI players",		2);
	mainMenu.addMenuItem(fiveAIPlayers,		"Five AI players",		2);
	mainMenu.addMenuItem(sixAIPlayers,		"Six AI players",		2);
	mainMenu.addMenuItem(sevenAIPlayers,	"Seven AI players",		2);
	mainMenu.addMenuItem(showCredits,		"",						0);
	mainMenu.addMenuItem(showTechTree,		"",						0);
	mainMenu.addMenuItem(exitGame,			"",						0);
}
