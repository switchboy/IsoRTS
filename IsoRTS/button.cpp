#include <iostream>
#include <sstream>
#include <SFML/Graphics.hpp>
#include "actors.h"
#include "buildings.h"
#include "button.h"
#include "gametext.h"
#include "player.h"
#include "commandSync.h"

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
        toolTipDiscription << "Collection point of resources (any type), production of new villagers and research facility. Grants 10 population.";
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
        toolTipDiscription << "The people of the town will start to well... generate a new villager. Villagers can collect resources and build buildings.";
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
    case actionTypes::actionBuildWall:
        toolTipTitle << "Build Wall";
        toolTipText << "Cost: Food: " << listOfBuildingTemplates[7].getPriceOfBuilding().food << " Wood: " << listOfBuildingTemplates[7].getPriceOfBuilding().wood << " Stone: " << listOfBuildingTemplates[7].getPriceOfBuilding().stone << " Gold: " << listOfBuildingTemplates[7].getPriceOfBuilding().gold;
        toolTipDiscription << "Stone building that acts as a barrier. To place left-click a starting location then move the mouse to an end location and left-click again  ";
        break;
    case actionTypes::actionMakeGate:
        toolTipTitle << "Make gate";
        toolTipDiscription << "Turns a section of straight wall into a gate. When opened all units can pass. When closed none shall pass.";
        break;
    case actionTypes::actionOpenGate:
            toolTipTitle << "Open the gate";
            toolTipDiscription << "Lets everyone pass!";
        
        break;
    case actionTypes::actionCloseGate:
            toolTipTitle << "Close the gate";
            toolTipDiscription << "Thou shall not pass!";
        break;
    case actionTypes::actionShowCivilBuildings:
        toolTipTitle << "Show civil buildings";
        toolTipDiscription << "Overview of buildings to improve your civilasation";
        break;
    case actionTypes::actionShowMilitairyBuildings:
        toolTipTitle << "Show militairy buildings";
        toolTipDiscription << "Overview of militairy buildings to expand and defend your civilasation";
        break;
    case actionTypes::actionShowBack:
        toolTipTitle << "Go back";
        toolTipDiscription << "Turns the toolbar back one page";
        break;
    case actionTypes::actionAttackMove:
        toolTipTitle << "Attack move";
        toolTipDiscription << "Attack any enemy unit within range from selected site";
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

        if(listOfBuildingTemplates[1].getPriceOfBuilding().food <= listOfPlayers[currentPlayerI].getStats().amountOfFood && listOfBuildingTemplates[1].getPriceOfBuilding().wood <= listOfPlayers[currentPlayerI].getStats().amountOfWood && listOfBuildingTemplates[1].getPriceOfBuilding().stone <= listOfPlayers[currentPlayerI].getStats().amountOfStone && listOfBuildingTemplates[1].getPriceOfBuilding().gold <= listOfPlayers[currentPlayerI].getStats().amountOfGold)
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
        if (listOfBuildingTemplates[0].getPriceOfBuilding().food <= listOfPlayers[currentPlayerI].getStats().amountOfFood && listOfBuildingTemplates[0].getPriceOfBuilding().wood <= listOfPlayers[currentPlayerI].getStats().amountOfWood && listOfBuildingTemplates[0].getPriceOfBuilding().stone <= listOfPlayers[currentPlayerI].getStats().amountOfStone && listOfBuildingTemplates[0].getPriceOfBuilding().gold <= listOfPlayers[currentPlayerI].getStats().amountOfGold)
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
        if(listOfActorTemplates[0].getPriceOfActor().food <= listOfPlayers[currentPlayerI].getStats().amountOfFood && listOfActorTemplates[0].getPriceOfActor().wood <= listOfPlayers[currentPlayerI].getStats().amountOfWood && listOfActorTemplates[0].getPriceOfActor().stone <= listOfPlayers[currentPlayerI].getStats().amountOfStone && listOfActorTemplates[0].getPriceOfActor().gold <= listOfPlayers[currentPlayerI].getStats().amountOfGold)
        {
            gameDirector.addCommand(
                {
                    currentGame.elapsedTimeMS,
                    listOfPlayers[currentPlayerI].getTeam(),
                    this->actorOrBuildingId,
                    false,
                    false,
                    {0,0},
                    worldObject::building,
                    stackOrderTypes::none,
                    actionTypes::actionMakeVillager
                }
            );
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
        gameDirector.addCommand(
            {
                currentGame.elapsedTimeMS,
                listOfPlayers[currentPlayerI].getTeam(),
                this->actorOrBuildingId,
                false,
                false,
                {0,0},
                worldObject::building,
                stackOrderTypes::none,
                actionTypes::actionCancelBuilding
            }
        );
        break;
    case actionTypes::actionCancelProduction:
        //cancel production or research
        gameDirector.addCommand(
            {
                currentGame.elapsedTimeMS,
                listOfPlayers[currentPlayerI].getTeam(),
                this->actorOrBuildingId,
                false,
                false,
                {this->taskId,0},
                worldObject::building,
                stackOrderTypes::none,
                actionTypes::actionCancelBuilding
            }
        );
        break;

    case actionTypes::actionBuildMill:
        if (listOfBuildingTemplates[2].getPriceOfBuilding().food <= listOfPlayers[currentPlayerI].getStats().amountOfFood && listOfBuildingTemplates[2].getPriceOfBuilding().wood <= listOfPlayers[currentPlayerI].getStats().amountOfWood && listOfBuildingTemplates[2].getPriceOfBuilding().stone <= listOfPlayers[currentPlayerI].getStats().amountOfStone && listOfBuildingTemplates[2].getPriceOfBuilding().gold <= listOfPlayers[currentPlayerI].getStats().amountOfGold)
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
        if (listOfBuildingTemplates[3].getPriceOfBuilding().food <= listOfPlayers[currentPlayerI].getStats().amountOfFood && listOfBuildingTemplates[3].getPriceOfBuilding().wood <= listOfPlayers[currentPlayerI].getStats().amountOfWood && listOfBuildingTemplates[3].getPriceOfBuilding().stone <= listOfPlayers[currentPlayerI].getStats().amountOfStone && listOfBuildingTemplates[3].getPriceOfBuilding().gold <= listOfPlayers[currentPlayerI].getStats().amountOfGold)
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
        if (listOfBuildingTemplates[4].getPriceOfBuilding().food <= listOfPlayers[currentPlayerI].getStats().amountOfFood && listOfBuildingTemplates[4].getPriceOfBuilding().wood <= listOfPlayers[currentPlayerI].getStats().amountOfWood && listOfBuildingTemplates[4].getPriceOfBuilding().stone <= listOfPlayers[currentPlayerI].getStats().amountOfStone && listOfBuildingTemplates[4].getPriceOfBuilding().gold <= listOfPlayers[currentPlayerI].getStats().amountOfGold)
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
        if (listOfActorTemplates[1].getPriceOfActor().food <= listOfPlayers[currentPlayerI].getStats().amountOfFood && listOfActorTemplates[1].getPriceOfActor().wood <= listOfPlayers[currentPlayerI].getStats().amountOfWood && listOfActorTemplates[1].getPriceOfActor().stone <= listOfPlayers[currentPlayerI].getStats().amountOfStone && listOfActorTemplates[1].getPriceOfActor().gold <= listOfPlayers[currentPlayerI].getStats().amountOfGold)
        {
            gameDirector.addCommand(
                {
                    currentGame.elapsedTimeMS,
                    listOfPlayers[currentPlayerI].getTeam(),
                    this->actorOrBuildingId,
                    false,
                    false,
                    {0,0},
                    worldObject::building,
                    stackOrderTypes::none,
                    actionTypes::actionMakeSwordsman
                }
            );
        }
        else
        {
            std::stringstream errortext;
            errortext << "Not enough resources to produce a Swordsman! Cost Food: " << listOfActorTemplates[1].getPriceOfActor().food << " Wood: " << listOfActorTemplates[1].getPriceOfActor().wood << " Stone: " << listOfActorTemplates[1].getPriceOfActor().stone << " Gold: " << listOfActorTemplates[1].getPriceOfActor().gold;
            gameText.addNewMessage(errortext.str(), 1);
        }
        break;
    case actionTypes::actionBuildMiningCamp:
        if (listOfBuildingTemplates[5].getPriceOfBuilding().food <= listOfPlayers[currentPlayerI].getStats().amountOfFood && listOfBuildingTemplates[5].getPriceOfBuilding().wood <= listOfPlayers[currentPlayerI].getStats().amountOfWood && listOfBuildingTemplates[5].getPriceOfBuilding().stone <= listOfPlayers[currentPlayerI].getStats().amountOfStone && listOfBuildingTemplates[5].getPriceOfBuilding().gold <= listOfPlayers[currentPlayerI].getStats().amountOfGold)
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
    case actionTypes::actionBuildWall:
        if (listOfBuildingTemplates[7].getPriceOfBuilding().food <= listOfPlayers[currentPlayerI].getStats().amountOfFood && listOfBuildingTemplates[7].getPriceOfBuilding().wood <= listOfPlayers[currentPlayerI].getStats().amountOfWood && listOfBuildingTemplates[7].getPriceOfBuilding().stone <= listOfPlayers[currentPlayerI].getStats().amountOfStone && listOfBuildingTemplates[7].getPriceOfBuilding().gold <= listOfPlayers[currentPlayerI].getStats().amountOfGold)
        {
            currentGame.setBuildingType(7);
            currentGame.setIsPlacingBuilding();
        }
        else
        {
            std::stringstream errortext;
            errortext << "Not enough resources to build a wall! Cost Food: " << listOfBuildingTemplates[7].getPriceOfBuilding().food << " Wood: " << listOfBuildingTemplates[7].getPriceOfBuilding().wood << " Stone: " << listOfBuildingTemplates[7].getPriceOfBuilding().stone << " Gold: " << listOfBuildingTemplates[7].getPriceOfBuilding().gold;
            gameText.addNewMessage(errortext.str(), 1);
        }
        break;
    case actionTypes::actionMakeGate:
        // Todo decide if this should cost something
        gameDirector.addCommand(
            {
                currentGame.elapsedTimeMS,
                listOfPlayers[currentPlayerI].getTeam(),
                this->actorOrBuildingId,
                false,
                false,
                {0,0},
                worldObject::building,
                stackOrderTypes::none,
                actionTypes::actionMakeGate
            }
        );
        break;
    case actionTypes::actionOpenGate:
        gameDirector.addCommand(
            {
                currentGame.elapsedTimeMS,
                listOfPlayers[currentPlayerI].getTeam(),
                this->actorOrBuildingId,
                false,
                false,
                {0,0},
                worldObject::building,
                stackOrderTypes::none,
                actionTypes::actionOpenGate
            }
        );
        break;
    case actionTypes::actionCloseGate:
        gameDirector.addCommand(
            {
                currentGame.elapsedTimeMS,
                listOfPlayers[currentPlayerI].getTeam(),
                this->actorOrBuildingId,
                false,
                false,
                {0,0},
                worldObject::building,
                stackOrderTypes::none,
                actionTypes::actionCloseGate
            }
        );
        break;
    case actionTypes::actionShowCivilBuildings:
        currentGame.setToolbarSubMenu(1);
        break;
    case actionTypes::actionShowMilitairyBuildings:
        currentGame.setToolbarSubMenu(2);
        break;
    case actionTypes::actionShowBack:
        currentGame.setToolbarSubMenu(0);
        break;
    case actionTypes::actionAttackMove:
        currentGame.setAttackMove();
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
    case spriteTypes::spriteWall: //wall
        xOffSet = 0;
        yOffset = 384;
        break;
    case spriteTypes::spriteGate:
        xOffSet = 0;
        yOffset = 448;
        break;
    case spriteTypes::spriteOpenGate:
        xOffSet = 192;
        yOffset = 64;
        break;
    case spriteTypes::spriteCivilBuilding:
        xOffSet = 192;
        yOffset = 128;
        break;
    case spriteTypes::spriteMilitairyBuilding:
        xOffSet = 192;
        yOffset = 192;
        break;
    case spriteTypes::spriteGoBack:
        xOffSet = 192;
        yOffset = 256;
        break;
    case spriteTypes::spriteAttackMove:
        xOffSet = 192;
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

namespace buttons {
    bool requirementForButtonIsMet(actionTypes actionType, int unitOrBuildingId, int playerId) {

        switch (actionType) {
        case actionTypes::actionBuildTownCenter:
            return true;
            break;
        case actionTypes::actionBuildHouse:
            return true;
            break;
        case actionTypes::actionUnitSelect:
            return true;
            break;
        case actionTypes::actionMakeVillager:
            return true;
            break;
        case actionTypes::actionCancelBuilding:
            return true;
            break;
        case actionTypes::actionCancelProduction:
            return true;
            break;
        case actionTypes::actionBuildMill:
            return true;
            break;
        case actionTypes::actionBuildLumberCamp:
            return true;
            break;
        case actionTypes::actionBuildBarracks:
            return true;
            break;
        case actionTypes::actionMakeSwordsman:
            return true;
            break;
        case actionTypes::actionBuildMiningCamp:
            return true;
            break;
        case actionTypes::actionBuildWall:
            return true;
            break;
        case actionTypes::actionMakeGate:
            if (listOfBuildings[unitOrBuildingId].canBeGate() && !listOfBuildings[unitOrBuildingId].getIsGate()) {
                return true;
            }
            return false;
            break;
        case actionTypes::actionOpenGate:
            if (listOfBuildings[unitOrBuildingId].getIsGate() && !listOfBuildings[unitOrBuildingId].getGateIsOpen()) {
                return true;
            }
            return false;
            break;
        case actionTypes::actionCloseGate:
            if (listOfBuildings[unitOrBuildingId].getIsGate() && listOfBuildings[unitOrBuildingId].getGateIsOpen()) {
                return true;
            }
            return false;
            break;
        case actionTypes::actionShowBack:
            return true;
            break;
        case actionTypes::actionShowCivilBuildings:
            return true;
            break;
        case actionTypes::actionShowMilitairyBuildings:
            return true;
            break;
        case actionTypes::actionAttackMove:
            return true;
        }
        return false;
    }
}