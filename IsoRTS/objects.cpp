#include "objects.h"
#include "gamestate.h"

std::vector<objects> listOfObjects;

objects::objects(objectTypes type, int startXlocation, int startYLocation, int objectId)
{
    this->objectType = type;
    this->locationX = startXlocation;
    this->locationY = startYLocation;
    this->objectId = objectId;
    switch(type)
    {
    case objectTypes::objectCactus:
        this->typeOfResource = resourceTypes::resourceWood; 
        break;
    case objectTypes::objectCypress:
        this->typeOfResource = resourceTypes::resourceWood;
        break;
    case objectTypes::objectMaple:
        this->typeOfResource = resourceTypes::resourceWood;
        break;
    case objectTypes::objectPine:
        this->typeOfResource = resourceTypes::resourceWood;
        break;
    case objectTypes::objectStone:
        this->typeOfResource = resourceTypes::resourceStone;
        break;
    case objectTypes::objectGold:
        this->typeOfResource = resourceTypes::resourceGold;
        break;
    case objectTypes::objectBerry:
        this->typeOfResource = resourceTypes::resourceFood;
        break;
    }

    switch(this->typeOfResource)
    {
    case resourceTypes::resourceWood:
        this->resourceLeft = 200;
        break;
    case resourceTypes::resourceFood:
        this->resourceLeft = 500;
        break;
    case resourceTypes::resourceStone:
        this->resourceLeft = 2000;
        break;
    case resourceTypes::resourceGold:
        this->resourceLeft = 1500;
        break;
    }
    currentGame.objectLocationList[startXlocation][startYLocation] = objectId;
}

void objects::substractResource()
{
    this->resourceLeft -= 1;
    if(resourceLeft <= 0)
    {
        currentGame.objectLocationList[this->locationX][this->locationY] = -1;
    }
}

cords objects::getLocation() const {
    return { this->locationX, this->locationY };
}

void objects::drawObject(int i, int j) const
{
    drawObjectSprite(this->objectType, i, j);
}

resourceTypes objects::getTypeOfResource() const
{
    return this->typeOfResource;
}

void objects::drawObjectSprite(objectTypes spriteNumber, int i, int j) const
{
    switch(spriteNumber)
    {
    case objectTypes::objectCactus:
        currentGame.spriteCactusTile.setPosition(static_cast<float>(worldSpace(i,j,true)), static_cast<float>(worldSpace(i,j,false)));
        window.draw(currentGame.spriteCactusTile);
        break;
    case objectTypes::objectCypress:
        currentGame.spriteCypressTrileTile.setPosition(static_cast<float>(worldSpace(i,j,true)), static_cast<float>(worldSpace(i,j,false)));
        window.draw(currentGame.spriteCypressTrileTile);
        break;
    case objectTypes::objectMaple:
        currentGame.spriteNormalTreeTile.setPosition(static_cast<float>(worldSpace(i,j,true)), static_cast<float>(worldSpace(i,j,false)));
        window.draw(currentGame.spriteNormalTreeTile);
        break;
    case objectTypes::objectPine:
        currentGame.spritePineTreeTile.setPosition(static_cast<float>(worldSpace(i,j,true)), static_cast<float>(worldSpace(i,j,false)));
        window.draw(currentGame.spritePineTreeTile);
        break;

    case objectTypes::objectStone:
        currentGame.spriteStone.setPosition(static_cast<float>(worldSpace(i,j,true)), static_cast<float>(worldSpace(i,j,false)));
        window.draw(currentGame.spriteStone);
        break;
    case objectTypes::objectGold:
        currentGame.spriteGold.setPosition(static_cast<float>(worldSpace(i,j,true)), static_cast<float>(worldSpace(i,j,false)));
        window.draw(currentGame.spriteGold);
        break;
    case objectTypes::objectBerry:
        currentGame.spriteBerryBush.setPosition(static_cast<float>(worldSpace(i,j,true)), static_cast<float>(worldSpace(i,j,false)));
        window.draw(currentGame.spriteBerryBush);
        break;
    }
}

std::string objects::getName() const
{
    switch(this->objectType)
    {
    case objectTypes::objectCactus:
        return "Cactus";
        break;
    case objectTypes::objectCypress:
        return "Cypress";
        break;
    case objectTypes::objectMaple:
        return "Maple";
        break;
    case objectTypes::objectPine:
        return "Pine";
        break;
    case objectTypes::objectStone:
        return "Stone";
        break;
    case objectTypes::objectGold:
        return "Gold";
        break;
    case objectTypes::objectBerry:
        return "Berry bush";
        break;
    default:
        return "Undefined";
    }
}

objectTypes objects::getType() const
{
    return this->objectType;
}

int objects::amountOfResourcesLeft() const
{
    return this->resourceLeft;
}

std::string objects::nameOfResource() const
{
    switch(this->typeOfResource)
    {
    case resourceTypes::resourceWood:
        return "Wood";
        break;
    case resourceTypes::resourceFood:
        return "Food";
        break;
    case resourceTypes::resourceStone:
        return "Stone";
        break;
    case resourceTypes::resourceGold:
        return "Gold";
        break;
    default:
        return "undefined";
    }
}

void objects::drawObjectFootprint(objectTypes type, int mouseWorldX, int mouseWorldY) const
{
    if(!(mouseWorldX < 0) && !(mouseWorldY < 0) && !(mouseWorldX >= MAP_WIDTH) && !(mouseWorldY >= MAP_HEIGHT))
    {
        if(currentGame.occupiedByBuildingList[mouseWorldX][mouseWorldY] == -1 && currentGame.objectLocationList[mouseWorldX][mouseWorldY] == -1)
        {
            currentGame.drawMousePosition(mouseWorldX, mouseWorldY, true);
        }
        else
        {
            currentGame.drawMousePosition(mouseWorldX, mouseWorldY, false);
        }
        drawObjectSprite(type, mouseWorldX, mouseWorldY);
    }
}
