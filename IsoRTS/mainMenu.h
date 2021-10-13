#pragma once

#include <string>
#include <vector>
#include "SFML/Graphics.hpp"
#include <iostream>

enum menuItemNames
{
	newGame,
	loadGame,
	multiplayerGame,
	sandBoxGame,
	skirmishGame,
	oneAIPlayers,
	twoAIPlayers,
	threeAIPlayers,
	fourAIPlayers,
	fiveAIPlayers,
	sixAIPlayers,
	sevenAIPlayers,
	showCredits,
	showTechTree,
	rootMenu,
	exitGame,
	nothingSelected
};

struct menuItem {
	menuItemNames id;
	std::string name;
	int menuLevel;
};

class menu
{
public:
	menu() {
		if (textureMousePointer.loadFromFile("textures/mouseHints.png"))
		{
			spriteMousePointer.setTexture(textureMousePointer);
			spriteMousePointer.setTextureRect(sf::IntRect(0, 0, 32, 32));
		}
		else
		{
			std::cout << "Error loading texture: mouseHints.png \n" << std::endl;
		}

		if (menuButtonTexture.loadFromFile("textures/menuButton.png"))
		{
			menuButtonSprite.setTexture(menuButtonTexture);
			menuButtonSprite.setTextureRect(sf::IntRect(0, 0, 900, 130));
		}
		else
		{
			std::cout << "Error loading texture: menuButton.png \n" << std::endl;
		}
	};
	void addMenuItem(menuItemNames id, std::string name, int menuLevel);
	void displayMenu();
	void interactMenu();
	int getMenuLevel();
	void setMenuLevel(int level);
	void preformMenuAction(menuItemNames itemClicked);
	void doNetworking();

private:
	std::vector<menuItem> menuItems;
	int menuLevel;
	sf::Vector2f mousePosition;
	sf::Sprite spriteMousePointer, menuButtonSprite;
	sf::Texture textureMousePointer, menuButtonTexture;
	bool focus;
	bool mouseLeftPressed = false;
	menuItemNames itemSelected = nothingSelected;
};

extern void createMainMenuItems(menu& mainMenu);
