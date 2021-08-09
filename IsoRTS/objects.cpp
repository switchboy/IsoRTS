#include "objects.h"
#include "gamestate.h"

std::vector<objects> listOfObjects;

objects::objects(objectTypes type, cords location, int objectId)
{
    this->objectType = type;
    this->location = location;
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
        this->resourceLeft = 5000;
        break;
    case resourceTypes::resourceStone:
        this->resourceLeft = 2000;
        break;
    case resourceTypes::resourceGold:
        this->resourceLeft = 1500;
        break;
    }
    currentGame.objectLocationList[location.x][location.y] = objectId;
    currentGame.setObjectsHaveChanged();
}

void objects::substractResource()
{
    this->resourceLeft -= 1;
    if(resourceLeft <= 0)
    {
        this->destroyObject();
    }
}

cords objects::getLocation() const {
    return this->location;
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
        currentGame.spriteCactusTile.setPosition(static_cast<float>(worldSpace({ i,j }).x), static_cast<float>(worldSpace({ i, j }).y));
        window.draw(currentGame.spriteCactusTile);
        break;
    case objectTypes::objectCypress:
        currentGame.spriteCypressTrileTile.setPosition(static_cast<float>(worldSpace({ i,j }).x), static_cast<float>(worldSpace({ i, j }).y));
        window.draw(currentGame.spriteCypressTrileTile);
        break;
    case objectTypes::objectMaple:
        currentGame.spriteNormalTreeTile.setPosition(static_cast<float>(worldSpace({ i,j }).x), static_cast<float>(worldSpace({ i, j }).y));
        window.draw(currentGame.spriteNormalTreeTile);
        break;
    case objectTypes::objectPine:
        currentGame.spritePineTreeTile.setPosition(static_cast<float>(worldSpace({ i,j }).x), static_cast<float>(worldSpace({ i, j }).y));
        window.draw(currentGame.spritePineTreeTile);
        break;

    case objectTypes::objectStone:
        currentGame.spriteStone.setPosition(static_cast<float>(worldSpace({ i,j }).x), static_cast<float>(worldSpace({ i, j }).y));
        window.draw(currentGame.spriteStone);
        break;
    case objectTypes::objectGold:
        currentGame.spriteGold.setPosition(static_cast<float>(worldSpace({ i,j }).x), static_cast<float>(worldSpace({ i, j }).y));
        window.draw(currentGame.spriteGold);
        break;
    case objectTypes::objectBerry:
        currentGame.spriteBerryBush.setPosition(static_cast<float>(worldSpace({ i,j }).x), static_cast<float>(worldSpace({ i, j }).y));
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

void objects::destroyObject()
{
    currentGame.objectLocationList[this->location.x][this->location.y] = -1;
    currentGame.setObjectsHaveChanged();
}

void objects::drawObjectFootprint(objectTypes type, cords mouseWorld) const
{
    if(!(mouseWorld.x < 0) && !(mouseWorld.y < 0) && !(mouseWorld.x >= MAP_WIDTH) && !(mouseWorld.y >= MAP_HEIGHT))
    {
        if(currentGame.occupiedByBuildingList[mouseWorld.x][mouseWorld.y] == -1 && currentGame.objectLocationList[mouseWorld.x][mouseWorld.y] == -1)
        {
            currentGame.drawMousePosition(mouseWorld.x, mouseWorld.y, true);
        }
        else
        {
            currentGame.drawMousePosition(mouseWorld.x, mouseWorld.y, false);
        }
        drawObjectSprite(type, mouseWorld.x, mouseWorld.y);
    }
}
