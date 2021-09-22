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
	int padding = 125; // allows for 6 items
	int startX = (1920 / 2) - 450;
	int currentY = 125;
	menuItemNames tItemSelected = nothingSelected;

	currentGame.text.setCharacterSize(64);
	currentGame.text.setOutlineColor(sf::Color::Black);
	currentGame.text.setOutlineThickness(2.f);
	currentGame.text.setFillColor(sf::Color::White);
		
	window.clear();
	splashScreenSprite.setColor(sf::Color(40, 40, 40));
	window.draw(splashScreenSprite);

	std::list<menuItem> tempMenuItemList;
	for (auto& item : this->menuItems) {
		if (item.menuLevel == this->menuLevel) {
			tempMenuItemList.push_back(item);
		}
	}
	int spaceBetween;
	if (!tempMenuItemList.empty()) {
		if (tempMenuItemList.size() > 6) {
			spaceBetween = (1080 - (padding * 2) - (65 * tempMenuItemList.size())) / tempMenuItemList.size();
			startX = (1920 / 2) - 475;
		}
		else {
			spaceBetween = (1080 - (padding * 2) - (130 * tempMenuItemList.size())) / tempMenuItemList.size();
		}
	}
	else {
		spaceBetween = 0;
	}
	int oddCounter = 0;
	for (auto& item : tempMenuItemList) {
		if (tempMenuItemList.size() <= 6) {
			if (this->mousePosition.x >= startX && this->mousePosition.x <= startX + 900 && this->mousePosition.y >= currentY && this->mousePosition.y <= currentY + 130) {
				if (mouseLeftPressed) {
					menuButtonSprite.setTextureRect(sf::IntRect(0, 260, 900, 130));
				}
				else {
					menuButtonSprite.setTextureRect(sf::IntRect(0, 130, 900, 130));
				}
				tItemSelected = item.id;
			}
			else {
				menuButtonSprite.setTextureRect(sf::IntRect(0, 0, 900, 130));
			}

			menuButtonSprite.setPosition(static_cast<float>(startX), static_cast<float>(currentY));
			window.draw(menuButtonSprite);

			currentGame.text.setString(item.name);
			sf::FloatRect textRect = currentGame.text.getLocalBounds();
			currentGame.text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
			currentGame.text.setPosition(static_cast<float>(mainWindowWidth / 2), static_cast<float>(currentY + 65));
			window.draw(currentGame.text);
			currentY += spaceBetween + 130;
		}
		else {
			if (this->mousePosition.x >= startX && this->mousePosition.x <= startX + 450 && this->mousePosition.y >= currentY && this->mousePosition.y <= currentY + 130) {
				if (mouseLeftPressed) {
					menuButtonSprite.setTextureRect(sf::IntRect(0, 650, 450, 130));
				}
				else {
					menuButtonSprite.setTextureRect(sf::IntRect(0, 520, 450, 130));
				}
				tItemSelected = item.id;
			}
			else {
				menuButtonSprite.setTextureRect(sf::IntRect(0, 390, 450, 130));
			}

			menuButtonSprite.setPosition(static_cast<float>(startX), static_cast<float>(currentY));
			window.draw(menuButtonSprite);

			currentGame.text.setString(item.name);
			sf::FloatRect textRect = currentGame.text.getLocalBounds();
			currentGame.text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
			currentGame.text.setPosition(static_cast<float>(startX+225), static_cast<float>(currentY + 65));
			window.draw(currentGame.text);
			if (oddCounter == 0) {
				oddCounter++;
				startX = (1920 / 2) + 25;
			}
			else {
				currentY += spaceBetween + 130;
				startX = (1920 / 2) - 475;
				oddCounter = 0;
			}
		}
	}
	this->itemSelected = tItemSelected;
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
		case sf::Event::MouseButtonPressed:
			if (currentEvent.mouseButton.button == sf::Mouse::Left) {
				this->mouseLeftPressed = true;
			}
			break;
		case sf::Event::MouseButtonReleased:
			if (currentEvent.mouseButton.button == sf::Mouse::Left) {
				preformMenuAction(this->itemSelected);
			}
			break;
		default:
			this->mouseLeftPressed = false;
			break;
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
	case rootMenu:
		this->setMenuLevel(0);
		break;
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
	mainMenu.addMenuItem(newGame,			"Load Saved Game",		0);
	mainMenu.addMenuItem(multiplayerGame,	"Multiplayer Game",		0);
	mainMenu.addMenuItem(sandBoxGame,		"Sandbox Game",			1);
	mainMenu.addMenuItem(skirmishGame,		"Skirmish Game",		1);
	mainMenu.addMenuItem(oneAIPlayers,		"One AI",				2);
	mainMenu.addMenuItem(twoAIPlayers,		"Two AI",				2);
	mainMenu.addMenuItem(threeAIPlayers,	"Three AI",				2);
	mainMenu.addMenuItem(fourAIPlayers,		"Four AI",				2);
	mainMenu.addMenuItem(fiveAIPlayers,		"Five AI",				2);
	mainMenu.addMenuItem(sixAIPlayers,		"Six AI",				2);
	mainMenu.addMenuItem(sevenAIPlayers,	"Seven AI",				2);
	mainMenu.addMenuItem(showCredits,		"Credits",				0);
	mainMenu.addMenuItem(showTechTree,		"Show Tech Tree",		0);
	mainMenu.addMenuItem(rootMenu,			"Back",					1);
	mainMenu.addMenuItem(newGame,			"Back",					2);
	mainMenu.addMenuItem(exitGame,			"Exit",					0);
}
