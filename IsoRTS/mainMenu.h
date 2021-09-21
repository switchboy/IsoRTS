#pragma once

#include <string>
#include <vector>
#include "SFML/Graphics.hpp"
#include <iostream>

enum menuItemNames
{
	newGame,
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
	exitGame
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
	};
	void addMenuItem(menuItemNames id, std::string name, int menuLevel);
	void displayMenu();
	void interactMenu();
	int getMenuLevel();
	void setMenuLevel(int level);
	void preformMenuAction(menuItemNames itemClicked);

private:
	std::vector<menuItem> menuItems;
	int menuLevel;
	sf::Vector2f mousePosition;
	sf::Sprite spriteMousePointer;
	sf::Texture textureMousePointer;
	bool focus;
};

extern void createMainMenuItems(menu& mainMenu);