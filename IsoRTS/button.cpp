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
        toolTipText << "Cost: Food: "<< listOfBuildingTemplates[1].getPriceOfBuilding().food <<" Wood: "<< listOfBuildingTemplates[1].getPriceOfBuilding().wood <<" Stone: "<< listOfBuildingTemplates[1].getPriceOfBuilding().stone <<" Gold: "<< listOfBuildingTemplates[1].getPriceOfBuilding().gold;
        toolTipDiscription << "Collection point of resources, production of new villagers and research facility. Grants 10 population.";
        break;
    case actionTypes::actionBuildHouse:
        toolTipTitle << "House";
        toolTipText << "Cost: Food: " << listOfBuildingTemplates[0].getPriceOfBuilding().food << " Wood: " << listOfBuildingTemplates[0].getPriceOfBuilding().wood << " Stone: " << listOfBuildingTemplates[0].getPriceOfBuilding().stone << " Gold: " << listOfBuildingTemplates[0].getPriceOfBuilding().gold;
        toolTipDiscription << "Grants 5 population.";
        break;
    case actionTypes::actionUnitSelect:
        toolTipTitle << "Unit selection";
        toolTipText << listOfActors[this->actorOrBuildingId].nameOfActor();
        toolTipDiscription << "Selects this single unit from the current selection.";
        break;
    case actionTypes::actionMakeVillager:
        toolTipTitle << "Make villager";
        toolTipText << "Cost: Food: "<< listOfActorTemplates[0].getPriceOfActor().food <<" Wood: "<< listOfActorTemplates[0].getPriceOfActor().wood <<" Stone: "<< listOfActorTemplates[0].getPriceOfActor().stone <<" Gold: "<< listOfActorTemplates[0].getPriceOfActor().gold;
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
        toolTipText << "Cost: Food: " << listOfBuildingTemplates[2].getPriceOfBuilding().food << " Wood: " << listOfBuildingTemplates[2].getPriceOfBuilding().wood << " Stone: " << listOfBuildingTemplates[2].getPriceOfBuilding().stone << " Gold: " << listOfBuildingTemplates[2].getPriceOfBuilding().gold;
        toolTipDiscription << "Collection point for food, build farms and research facility for food";
        break;
    case actionTypes::actionBuildLumberCamp:
        toolTipTitle << "Build Lumber Camp";
        toolTipText << "Cost: Food: " << listOfBuildingTemplates[3].getPriceOfBuilding().food << " Wood: " << listOfBuildingTemplates[3].getPriceOfBuilding().wood << " Stone: " << listOfBuildingTemplates[3].getPriceOfBuilding().stone << " Gold: " << listOfBuildingTemplates[3].getPriceOfBuilding().gold;
        toolTipDiscription << "Collection point for wood and research new wood chopping abilities";
        break;
    case actionTypes::actionBuildBarracks:
        toolTipTitle << "Build Barracks";
        toolTipText << "Cost: Food: " << listOfBuildingTemplates[4].getPriceOfBuilding().food << " Wood: " << listOfBuildingTemplates[4].getPriceOfBuilding().wood << " Stone: " << listOfBuildingTemplates[4].getPriceOfBuilding().stone << " Gold: " << listOfBuildingTemplates[4].getPriceOfBuilding().gold;
        toolTipDiscription << "Train and research melee soldiers and combat abilities";
        break;
    case actionTypes::actionMakeSwordsman:
        toolTipTitle << "Create Swordsman";
        toolTipText << "Cost: Food: " << listOfActorTemplates[1].getPriceOfActor().food << " Wood: " << listOfActorTemplates[1].getPriceOfActor().wood << " Stone: " << listOfActorTemplates[1].getPriceOfActor().stone << " Gold: " << listOfActorTemplates[1].getPriceOfActor().gold;
        toolTipDiscription << "Train a basic melee sword wielding soldier";
        break;
    case actionTypes::actionBuildMiningCamp:
        toolTipTitle << "Build Mining Camp";
        toolTipText << "Cost: Food: " << listOfBuildingTemplates[5].getPriceOfBuilding().food << " Wood: " << listOfBuildingTemplates[5].getPriceOfBuilding().wood << " Stone: " << listOfBuildingTemplates[5].getPriceOfBuilding().stone << " Gold: " << listOfBuildingTemplates[5].getPriceOfBuilding().gold;
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

        if(listOfBuildingTemplates[1].getPriceOfBuilding().food <= currentPlayer.getStats().amountOfFood && listOfBuildingTemplates[1].getPriceOfBuilding().wood <= currentPlayer.getStats().amountOfWood && listOfBuildingTemplates[1].getPriceOfBuilding().stone <= currentPlayer.getStats().amountOfStone && listOfBuildingTemplates[1].getPriceOfBuilding().gold <= currentPlayer.getStats().amountOfGold)
        {
            currentGame.setBuildingType(1);
            currentGame.setIsPlacingBuilding();
        }
        else
        {
            std::stringstream errortext;
            errortext << "Not enough resources to build a town center! Cost Food: " << listOfBuildingTemplates[1].getPriceOfBuilding().food << " Wood: " << listOfBuildingTemplates[1].getPriceOfBuilding().wood << " Stone: " << listOfBuildingTemplates[1].getPriceOfBuilding().stone << " Gold: " << listOfBuildingTemplates[1].getPriceOfBuilding().gold;
            gameText.addNewMessage(errortext.str(), 1);
        }
        break;
    case actionTypes::actionBuildHouse:
        if (listOfBuildingTemplates[0].getPriceOfBuilding().food <= currentPlayer.getStats().amountOfFood && listOfBuildingTemplates[0].getPriceOfBuilding().wood <= currentPlayer.getStats().amountOfWood && listOfBuildingTemplates[0].getPriceOfBuilding().stone <= currentPlayer.getStats().amountOfStone && listOfBuildingTemplates[0].getPriceOfBuilding().gold <= currentPlayer.getStats().amountOfGold)
        {
            currentGame.setBuildingType(0);
            currentGame.setIsPlacingBuilding();
        }
        else
        {
            std::stringstream errortext;
            errortext << "Not enough resources to build a house! Cost Food: " << listOfBuildingTemplates[0].getPriceOfBuilding().food << " Wood: " << listOfBuildingTemplates[0].getPriceOfBuilding().wood << " Stone: " << listOfBuildingTemplates[0].getPriceOfBuilding().stone << " Gold: " << listOfBuildingTemplates[0].getPriceOfBuilding().gold;
            gameText.addNewMessage(errortext.str(), 1);

        }
        break;
    case actionTypes::actionUnitSelect:
        //select that unit
        currentGame.selectUnit(this->actorOrBuildingId);
        break;
    case actionTypes::actionMakeVillager:
        //create villager
        if(listOfActorTemplates[0].getPriceOfActor().food <= currentPlayer.getStats().amountOfFood && listOfActorTemplates[0].getPriceOfActor().wood <= currentPlayer.getStats().amountOfWood && listOfActorTemplates[0].getPriceOfActor().stone <= currentPlayer.getStats().amountOfStone && listOfActorTemplates[0].getPriceOfActor().gold <= currentPlayer.getStats().amountOfGold)
        {
            listOfBuildings[this->actorOrBuildingId].getTask(false, 0);
        }
        else
        {
            std::stringstream errortext;
            errortext << "Not enough resources to produce a villager! Cost Food: "<< listOfActorTemplates[0].getPriceOfActor().food <<" Wood: "<< listOfActorTemplates[0].getPriceOfActor().wood <<" Stone: "<< listOfActorTemplates[0].getPriceOfActor().stone <<" Gold: "<< listOfActorTemplates[0].getPriceOfActor().gold;
            gameText.addNewMessage(errortext.str(), 1);
        }
        break;
    case actionTypes::actionCancelBuilding:
        //cancel building
        currentPlayer.addResources(resourceTypes::resourceFood, listOfBuildingTemplates[listOfBuildings[this->actorOrBuildingId].getType()].getPriceOfBuilding().food/2 );
        currentPlayer.addResources(resourceTypes::resourceWood, listOfBuildingTemplates[listOfBuildings[this->actorOrBuildingId].getType()].getPriceOfBuilding().wood/2 );
        currentPlayer.addResources(resourceTypes::resourceStone, listOfBuildingTemplates[listOfBuildings[this->actorOrBuildingId].getType()].getPriceOfBuilding().stone/2 );
        currentPlayer.addResources(resourceTypes::resourceGold, listOfBuildingTemplates[listOfBuildings[this->actorOrBuildingId].getType()].getPriceOfBuilding().gold/2 );
        listOfBuildings[this->actorOrBuildingId].removeBuilding();
        break;
    case actionTypes::actionCancelProduction:
        //cancel production or research
        break;

    case actionTypes::actionBuildMill:
        if (listOfBuildingTemplates[2].getPriceOfBuilding().food <= currentPlayer.getStats().amountOfFood && listOfBuildingTemplates[2].getPriceOfBuilding().wood <= currentPlayer.getStats().amountOfWood && listOfBuildingTemplates[2].getPriceOfBuilding().stone <= currentPlayer.getStats().amountOfStone && listOfBuildingTemplates[2].getPriceOfBuilding().gold <= currentPlayer.getStats().amountOfGold)
        {
            currentGame.setBuildingType(2);
            currentGame.setIsPlacingBuilding();
        }
        else
        {
            std::stringstream errortext;
            errortext << "Not enough resources to build a Mill! Cost Food: " << listOfBuildingTemplates[2].getPriceOfBuilding().food << " Wood: " << listOfBuildingTemplates[2].getPriceOfBuilding().wood << " Stone: " << listOfBuildingTemplates[2].getPriceOfBuilding().stone << " Gold: " << listOfBuildingTemplates[2].getPriceOfBuilding().gold;
            gameText.addNewMessage(errortext.str(), 1);
        }
        break;

    case actionTypes::actionBuildLumberCamp:
        if (listOfBuildingTemplates[3].getPriceOfBuilding().food <= currentPlayer.getStats().amountOfFood && listOfBuildingTemplates[3].getPriceOfBuilding().wood <= currentPlayer.getStats().amountOfWood && listOfBuildingTemplates[3].getPriceOfBuilding().stone <= currentPlayer.getStats().amountOfStone && listOfBuildingTemplates[3].getPriceOfBuilding().gold <= currentPlayer.getStats().amountOfGold)
        {
            currentGame.setBuildingType(3);
            currentGame.setIsPlacingBuilding();
        }
        else
        {
            std::stringstream errortext;
            errortext << "Not enough resources to build a Lumber Camp! Cost Food: " << listOfBuildingTemplates[3].getPriceOfBuilding().food << " Wood: " << listOfBuildingTemplates[3].getPriceOfBuilding().wood << " Stone: " << listOfBuildingTemplates[3].getPriceOfBuilding().stone << " Gold: " << listOfBuildingTemplates[3].getPriceOfBuilding().gold;
            gameText.addNewMessage(errortext.str(), 1);
        }
        break;

    case actionTypes::actionBuildBarracks:
        if (listOfBuildingTemplates[4].getPriceOfBuilding().food <= currentPlayer.getStats().amountOfFood && listOfBuildingTemplates[4].getPriceOfBuilding().wood <= currentPlayer.getStats().amountOfWood && listOfBuildingTemplates[4].getPriceOfBuilding().stone <= currentPlayer.getStats().amountOfStone && listOfBuildingTemplates[4].getPriceOfBuilding().gold <= currentPlayer.getStats().amountOfGold)
        {
            currentGame.setBuildingType(4);
            currentGame.setIsPlacingBuilding();
        }
        else
        {
            std::stringstream errortext;
            errortext << "Not enough resources to build a Barracks! Cost Food: " << listOfBuildingTemplates[4].getPriceOfBuilding().food << " Wood: " << listOfBuildingTemplates[4].getPriceOfBuilding().wood << " Stone: " << listOfBuildingTemplates[4].getPriceOfBuilding().stone << " Gold: " << listOfBuildingTemplates[4].getPriceOfBuilding().gold;
            gameText.addNewMessage(errortext.str(), 1);
        }
        break;
    case actionTypes::actionMakeSwordsman:
        //create a swordman
        if (listOfActorTemplates[1].getPriceOfActor().food <= currentPlayer.getStats().amountOfFood && listOfActorTemplates[1].getPriceOfActor().wood <= currentPlayer.getStats().amountOfWood && listOfActorTemplates[1].getPriceOfActor().stone <= currentPlayer.getStats().amountOfStone && listOfActorTemplates[1].getPriceOfActor().gold <= currentPlayer.getStats().amountOfGold)
        {
            listOfBuildings[this->actorOrBuildingId].getTask(false, 1);
        }
        else
        {
            std::stringstream errortext;
            errortext << "Not enough resources to produce a Swordsman! Cost Food: " << listOfActorTemplates[1].getPriceOfActor().food << " Wood: " << listOfActorTemplates[1].getPriceOfActor().wood << " Stone: " << listOfActorTemplates[1].getPriceOfActor().stone << " Gold: " << listOfActorTemplates[1].getPriceOfActor().gold;
            gameText.addNewMessage(errortext.str(), 1);
        }
        break;
    case actionTypes::actionBuildMiningCamp:
        if (listOfBuildingTemplates[5].getPriceOfBuilding().food <= currentPlayer.getStats().amountOfFood && listOfBuildingTemplates[5].getPriceOfBuilding().wood <= currentPlayer.getStats().amountOfWood && listOfBuildingTemplates[5].getPriceOfBuilding().stone <= currentPlayer.getStats().amountOfStone && listOfBuildingTemplates[5].getPriceOfBuilding().gold <= currentPlayer.getStats().amountOfGold)
        {
            currentGame.setBuildingType(5);
            currentGame.setIsPlacingBuilding();
        }
        else
        {
            std::stringstream errortext;
            errortext << "Not enough resources to build a Mining Camp! Cost Food: " << listOfBuildingTemplates[5].getPriceOfBuilding().food << " Wood: " << listOfBuildingTemplates[5].getPriceOfBuilding().wood << " Stone: " << listOfBuildingTemplates[5].getPriceOfBuilding().stone << " Gold: " << listOfBuildingTemplates[5].getPriceOfBuilding().gold;
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
