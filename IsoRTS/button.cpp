#include <iostream>
#include <sstream>
#include <SFML/Graphics.hpp>
#include "actors.h"
#include "buildings.h"
#include "button.h"
#include "gametext.h"
#include "player.h"

std::list<button> listOfButtons;

button::button(int positionX, int positionY, spriteTypes spriteId, actionTypes actionType, int actorOrBuildingId, int buttonId, int taskId)
{
    this->positionX = positionX;
    this->positionY = positionY;
    this->realPositionX = positionX;
    this->realPositionY = static_cast<int>(positionY+(mainWindowHeigth*0.8f));
    this->spriteId = spriteId;
    this->actionType = actionType;
    this->actorOrBuildingId = actorOrBuildingId;
    this->buttonId = buttonId;
    this->taskId = taskId;
}

bool button::isClicked(sf::Vector2i mousePosition)
{
    if (sf::IntRect(this->realPositionX, this->realPositionY, 64, 64).contains(mousePosition))
    {
        performAction();
        return true;
    }
    return false;

}

bool button::isHovered(sf::Vector2i mousePosition) const
{
    if (sf::IntRect(this->realPositionX, this->realPositionY, 64, 64).contains(mousePosition))
    {
        showToolTip();
        return true;
    }
    return false;
}

void button::showToolTip() const
{
    std::stringstream toolTipTitle;
    std::stringstream toolTipText;
    std::stringstream toolTipDiscription;
    switch(this->actionType)
    {
    case actionTypes::actionBuildTownCenter:
        toolTipTitle << "Town center";
        toolTipText << "Cost: Food: "<< priceOfBuilding[1].food <<" Wood: "<< priceOfBuilding[1].wood <<" Stone: "<< priceOfBuilding[1].stone <<" Gold: "<< priceOfBuilding[1].gold;
        toolTipDiscription << "Collection point of resources, production of new villagers and research facility. Grants 10 population.";
        break;
    case actionTypes::actionBuildHouse:
        toolTipTitle << "House";
        toolTipText << "Cost: Food: "<< priceOfBuilding[0].food <<" Wood: "<< priceOfBuilding[0].wood <<" Stone: "<< priceOfBuilding[0].stone <<" Gold: "<< priceOfBuilding[0].gold;
        toolTipDiscription << "Grants 5 population.";
        break;
    case actionTypes::actionUnitSelect:
        toolTipTitle << "Unit selection";
        toolTipText << listOfActors[this->actorOrBuildingId].nameOfActor();
        toolTipDiscription << "Selects this single unit from the current selection.";
        break;
    case actionTypes::actionMakeVillager:
        toolTipTitle << "Make villager";
        toolTipText << "Cost: Food: "<< priceOfActor[0].food <<" Wood: "<< priceOfActor[0].wood <<" Stone: "<< priceOfActor[0].stone <<" Gold: "<< priceOfActor[0].gold;
        toolTipDiscription << "The town center will start to well... generate a new villager. Villagers can collect resources and build buildings.";
        break;
    case actionTypes::actionCancelBuilding:
        toolTipTitle << "Cancel building";
        toolTipText << "Cancels building the current building.";
        toolTipDiscription << "Removes blueprint and returns half of the invested resources.";
        break;
    case actionTypes::actionCancelProduction:
        toolTipTitle << "Cancel production";
        toolTipText << "Cancels research or unit production";
        toolTipDiscription << "Production ceases and half of the invested resources are returned.";
        break;
    case actionTypes::actionBuildMill:
        toolTipTitle << "Build Mill";
        toolTipText << "Cost: Food: " << priceOfBuilding[2].food << " Wood: " << priceOfBuilding[2].wood << " Stone: " << priceOfBuilding[2].stone << " Gold: " << priceOfBuilding[2].gold;
        toolTipDiscription << "Collection point for food, build farms and research facility for food";
        break;
    case actionTypes::actionBuildLumberCamp:
        toolTipTitle << "Build Lumber Camp";
        toolTipText << "Cost: Food: " << priceOfBuilding[3].food << " Wood: " << priceOfBuilding[3].wood << " Stone: " << priceOfBuilding[3].stone << " Gold: " << priceOfBuilding[3].gold;
        toolTipDiscription << "Collection point for wood and research new wood chopping abilities";
        break;
    case actionTypes::actionBuildBarracks:
        toolTipTitle << "Build Barracks";
        toolTipText << "Cost: Food: " << priceOfBuilding[4].food << " Wood: " << priceOfBuilding[4].wood << " Stone: " << priceOfBuilding[4].stone << " Gold: " << priceOfBuilding[4].gold;
        toolTipDiscription << "Train and research melee soldiers and combat abilities";
        break;
    case actionTypes::actionMakeSwordsman:
        toolTipTitle << "Create Swordsman";
        toolTipText << "Cost: Food: " << priceOfActor[1].food << " Wood: " << priceOfActor[1].wood << " Stone: " << priceOfActor[1].stone << " Gold: " << priceOfActor[1].gold;
        toolTipDiscription << "Train a basic melee sword wielding soldier";
        break;
    case actionTypes::actionBuildMiningCamp:
        toolTipTitle << "Build Mining Camp";
        toolTipText << "Cost: Food: " << priceOfBuilding[5].food << " Wood: " << priceOfBuilding[5].wood << " Stone: " << priceOfBuilding[5].stone << " Gold: " << priceOfBuilding[5].gold;
        toolTipDiscription << "Collection point for gold and stone and research new mining abilities";
        break;
    }

    int longestStringLength = 0;
    if(toolTipDiscription.str().length() >= toolTipText.str().length() && toolTipDiscription.str().length() >= toolTipTitle.str().length())
    {
        longestStringLength = static_cast<int>(toolTipDiscription.str().length());
    }
    else if(toolTipText.str().length() >= toolTipDiscription.str().length() && toolTipText.str().length() >= toolTipTitle.str().length())
    {
        longestStringLength = static_cast<int>(toolTipText.str().length());
    }
    else
    {
        longestStringLength = static_cast<int>(toolTipTitle.str().length());
    }
    int charSize = 20;
    int startPositionY = static_cast<int>(mainWindowHeigth*0.8)-(3*(charSize+2));
    sf::RectangleShape toolTipBox(sf::Vector2f(static_cast<float>(longestStringLength*8), static_cast<float>(3*(charSize+2))+5));
    toolTipBox.setFillColor(sf::Color(0, 0, 0, 75));
    toolTipBox.setOutlineThickness(2.f);
    toolTipBox.setOutlineColor(sf::Color(255, 255, 255,150));
    toolTipBox.setPosition(2, static_cast<float>(startPositionY-7));
    window.draw(toolTipBox);
    startPositionY -= 4;
    currentGame.text.setCharacterSize(charSize);
    currentGame.text.setOutlineColor(sf::Color::Black);
    currentGame.text.setOutlineThickness(1.f);
    currentGame.text.setFillColor(sf::Color::White);
    currentGame.text.setPosition(5, static_cast<float>(startPositionY));
    currentGame.text.setString(toolTipTitle.str());
    window.draw(currentGame.text);
    charSize = 16;
    startPositionY += 22;
    currentGame.text.setCharacterSize(charSize);
    currentGame.text.setPosition(5, static_cast<float>(startPositionY));
    currentGame.text.setString(toolTipText.str());
    window.draw(currentGame.text);
    startPositionY += 18;
    currentGame.text.setCharacterSize(14);
    currentGame.text.setCharacterSize(charSize);
    currentGame.text.setPosition(5, static_cast<float>(startPositionY));
    currentGame.text.setString(toolTipDiscription.str());
    window.draw(currentGame.text);
}

void button::performAction()
{
    switch(this->actionType)
    {
    case actionTypes::actionBuildTownCenter:
        if(priceOfBuilding[1].food <= currentPlayer.getStats().amountOfFood && priceOfBuilding[1].wood <= currentPlayer.getStats().amountOfWood && priceOfBuilding[1].stone <= currentPlayer.getStats().amountOfStone && priceOfBuilding[1].gold <= currentPlayer.getStats().amountOfGold)
        {
            currentGame.setBuildingType(1);
            currentGame.setIsPlacingBuilding();
        }
        else
        {
            std::stringstream errortext;
            errortext << "Not enough resources to build a town center! Cost Food: "<< priceOfBuilding[1].food <<" Wood: "<< priceOfBuilding[1].wood <<" Stone: "<< priceOfBuilding[1].stone <<" Gold: "<< priceOfBuilding[1].gold;
            gameText.addNewMessage(errortext.str(), 1);
        }
        break;
    case actionTypes::actionBuildHouse:
        if(priceOfBuilding[0].food <= currentPlayer.getStats().amountOfFood && priceOfBuilding[0].wood <= currentPlayer.getStats().amountOfWood && priceOfBuilding[0].stone <= currentPlayer.getStats().amountOfStone && priceOfBuilding[0].gold <= currentPlayer.getStats().amountOfGold)
        {
            currentGame.setBuildingType(0);
            currentGame.setIsPlacingBuilding();
        }
        else
        {
            std::stringstream errortext;
            errortext << "Not enough resources to build a house! Cost Food: "<< priceOfBuilding[0].food <<" Wood: "<< priceOfBuilding[0].wood <<" Stone: "<< priceOfBuilding[0].stone <<" Gold: "<< priceOfBuilding[0].gold;
            gameText.addNewMessage(errortext.str(), 1);

        }
        break;
    case actionTypes::actionUnitSelect:
        //select that unit
        currentGame.selectUnit(this->actorOrBuildingId);
        break;
    case actionTypes::actionMakeVillager:
        //create villager
        if(priceOfActor[0].food <= currentPlayer.getStats().amountOfFood && priceOfActor[0].wood <= currentPlayer.getStats().amountOfWood && priceOfActor[0].stone <= currentPlayer.getStats().amountOfStone && priceOfActor[0].gold <= currentPlayer.getStats().amountOfGold)
        {
            listOfBuildings[this->actorOrBuildingId].getTask(false, 0);
        }
        else
        {
            std::stringstream errortext;
            errortext << "Not enough resources to produce a villager! Cost Food: "<< priceOfActor[0].food <<" Wood: "<< priceOfActor[0].wood <<" Stone: "<< priceOfActor[0].stone <<" Gold: "<< priceOfActor[0].gold;
            gameText.addNewMessage(errortext.str(), 1);
        }
        break;
    case actionTypes::actionCancelBuilding:
        //cancel building
        currentPlayer.addResources(resourceTypes::resourceFood, priceOfBuilding[listOfBuildings[this->actorOrBuildingId].getType()].food/2 );
        currentPlayer.addResources(resourceTypes::resourceWood, priceOfBuilding[listOfBuildings[this->actorOrBuildingId].getType()].wood/2 );
        currentPlayer.addResources(resourceTypes::resourceStone, priceOfBuilding[listOfBuildings[this->actorOrBuildingId].getType()].stone/2 );
        currentPlayer.addResources(resourceTypes::resourceGold, priceOfBuilding[listOfBuildings[this->actorOrBuildingId].getType()].gold/2 );
        listOfBuildings[this->actorOrBuildingId].removeBuilding();
        break;
    case actionTypes::actionCancelProduction:
        //cancel production or research
        break;

    case actionTypes::actionBuildMill:
        if (priceOfBuilding[2].food <= currentPlayer.getStats().amountOfFood && priceOfBuilding[2].wood <= currentPlayer.getStats().amountOfWood && priceOfBuilding[2].stone <= currentPlayer.getStats().amountOfStone && priceOfBuilding[2].gold <= currentPlayer.getStats().amountOfGold)
        {
            currentGame.setBuildingType(2);
            currentGame.setIsPlacingBuilding();
        }
        else
        {
            std::stringstream errortext;
            errortext << "Not enough resources to build a Mill! Cost Food: " << priceOfBuilding[2].food << " Wood: " << priceOfBuilding[2].wood << " Stone: " << priceOfBuilding[2].stone << " Gold: " << priceOfBuilding[2].gold;
            gameText.addNewMessage(errortext.str(), 1);
        }
        break;

    case actionTypes::actionBuildLumberCamp:
        if (priceOfBuilding[3].food <= currentPlayer.getStats().amountOfFood && priceOfBuilding[3].wood <= currentPlayer.getStats().amountOfWood && priceOfBuilding[3].stone <= currentPlayer.getStats().amountOfStone && priceOfBuilding[3].gold <= currentPlayer.getStats().amountOfGold)
        {
            currentGame.setBuildingType(3);
            currentGame.setIsPlacingBuilding();
        }
        else
        {
            std::stringstream errortext;
            errortext << "Not enough resources to build a Lumber Camp! Cost Food: " << priceOfBuilding[3].food << " Wood: " << priceOfBuilding[3].wood << " Stone: " << priceOfBuilding[3].stone << " Gold: " << priceOfBuilding[3].gold;
            gameText.addNewMessage(errortext.str(), 1);
        }
        break;

    case actionTypes::actionBuildBarracks:
        if (priceOfBuilding[4].food <= currentPlayer.getStats().amountOfFood && priceOfBuilding[4].wood <= currentPlayer.getStats().amountOfWood && priceOfBuilding[4].stone <= currentPlayer.getStats().amountOfStone && priceOfBuilding[4].gold <= currentPlayer.getStats().amountOfGold)
        {
            currentGame.setBuildingType(4);
            currentGame.setIsPlacingBuilding();
        }
        else
        {
            std::stringstream errortext;
            errortext << "Not enough resources to build a Barracks! Cost Food: " << priceOfBuilding[4].food << " Wood: " << priceOfBuilding[4].wood << " Stone: " << priceOfBuilding[4].stone << " Gold: " << priceOfBuilding[4].gold;
            gameText.addNewMessage(errortext.str(), 1);
        }
        break;
    case actionTypes::actionMakeSwordsman:
        //create a swordman
        if (priceOfActor[1].food <= currentPlayer.getStats().amountOfFood && priceOfActor[1].wood <= currentPlayer.getStats().amountOfWood && priceOfActor[1].stone <= currentPlayer.getStats().amountOfStone && priceOfActor[1].gold <= currentPlayer.getStats().amountOfGold)
        {
            listOfBuildings[this->actorOrBuildingId].getTask(false, 1);
        }
        else
        {
            std::stringstream errortext;
            errortext << "Not enough resources to produce a Swordsman! Cost Food: " << priceOfActor[1].food << " Wood: " << priceOfActor[1].wood << " Stone: " << priceOfActor[1].stone << " Gold: " << priceOfActor[1].gold;
            gameText.addNewMessage(errortext.str(), 1);
        }
        break;
    case actionTypes::actionBuildMiningCamp:
        if (priceOfBuilding[5].food <= currentPlayer.getStats().amountOfFood && priceOfBuilding[5].wood <= currentPlayer.getStats().amountOfWood && priceOfBuilding[5].stone <= currentPlayer.getStats().amountOfStone && priceOfBuilding[5].gold <= currentPlayer.getStats().amountOfGold)
        {
            currentGame.setBuildingType(5);
            currentGame.setIsPlacingBuilding();
        }
        else
        {
            std::stringstream errortext;
            errortext << "Not enough resources to build a Mining Camp! Cost Food: " << priceOfBuilding[5].food << " Wood: " << priceOfBuilding[5].wood << " Stone: " << priceOfBuilding[5].stone << " Gold: " << priceOfBuilding[5].gold;
            gameText.addNewMessage(errortext.str(), 1);
        }
        break;
    }
}

void button::drawButton() const
{
    int xOffSet = 0;
    int yOffset = 0;
    switch(this->spriteId)
    {
    case spriteTypes::spriteTownCenter:// town center
        xOffSet = 0;
        yOffset = 0;
        break;
    case spriteTypes::spriteHouse:// house
        xOffSet = 0;
        yOffset = 64;
        break;
    case spriteTypes::spriteVillager://villager
        xOffSet = 64;
        yOffset = 0;
        break;
    case spriteTypes::spriteCancel:
        xOffSet = 192;
        yOffset = 0;
        break;
    case spriteTypes::spriteMill: //Mill
        xOffSet = 0;
        yOffset = 192;
        break;
    case spriteTypes::spriteLumberCamp: //Lumber Camp
        xOffSet = 0;
        yOffset = 128;
        break;
    case spriteTypes::spriteBarracks: //barracks
        xOffSet = 0;
        yOffset = 256;
        break;
    case spriteTypes::spriteSwordsman: //Swordsman
        xOffSet = 64;
        yOffset = 64;
        break;
    case spriteTypes::spriteMiningCamp: //Mining Camp
        xOffSet = 0;
        yOffset = 320;
        break;
    }
    currentGame.spriteUIButton.setTextureRect(sf::IntRect(xOffSet, yOffset, 64, 64));
    currentGame.spriteUIButton.setPosition(static_cast<float>(this->positionX), static_cast<float>(this->positionY));
    window.draw(currentGame.spriteUIButton);
}

button::~button()
{
    //dtor
}
