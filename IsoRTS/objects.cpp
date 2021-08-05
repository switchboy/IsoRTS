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
    case objectCactus:
        this->typeOfResource = resourceWood; 
        break;
    case objectCypress:
        this->typeOfResource = resourceWood;
        break;
    case objectMaple:
        this->typeOfResource = resourceWood;
        break;
    case objectPine:
        this->typeOfResource = resourceWood;
        break;
    case objectStone:
        this->typeOfResource = resourceStone;
        break;
    case objectGold:
        this->typeOfResource = resourceGold;
        break;
    case objectBerry:
        this->typeOfResource = resourceFood; 
        break;
    }

    switch(this->typeOfResource)
    {
    case resourceWood:
        this->resourceLeft = 200;
        break;
    case resourceFood:
        this->resourceLeft = 500;
        break;
    case resourceStone:
        this->resourceLeft = 2000;
        break;
    case resourceGold:
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

cords objects::getLocation() {

    return { this->locationX, this->locationY };
}

void objects::drawObject(int i, int j)
{
    drawObjectSprite(this->objectType, i, j);
}

resourceTypes objects::getTypeOfResource()
{
    return this->typeOfResource;
}

void objects::drawObjectSprite(int spriteNumber, int i, int j)
{
    switch(spriteNumber)
    {
    case 0:
        currentGame.spriteCactusTile.setPosition(worldSpace(i,j,true), worldSpace(i,j,false));
        window.draw(currentGame.spriteCactusTile);
        break;
    case 1:
        currentGame.spriteCypressTrileTile.setPosition(worldSpace(i,j,true), worldSpace(i,j,false));
        window.draw(currentGame.spriteCypressTrileTile);
        break;
    case 2:
        currentGame.spriteNormalTreeTile.setPosition(worldSpace(i,j,true), worldSpace(i,j,false));
        window.draw(currentGame.spriteNormalTreeTile);
        break;
    case 3:
        currentGame.spritePineTreeTile.setPosition(worldSpace(i,j,true), worldSpace(i,j,false));
        window.draw(currentGame.spritePineTreeTile);
        break;

    case 4:
        currentGame.spriteStone.setPosition(worldSpace(i,j,true), worldSpace(i,j,false));
        window.draw(currentGame.spriteStone);
        break;
    case 5:
        currentGame.spriteGold.setPosition(worldSpace(i,j,true), worldSpace(i,j,false));
        window.draw(currentGame.spriteGold);
        break;
    case 6:
        currentGame.spriteBerryBush.setPosition(worldSpace(i,j,true), worldSpace(i,j,false));
        window.draw(currentGame.spriteBerryBush);
        break;
    }
}

std::string objects::getName()
{
    switch(this->objectType)
    {
    case objectCactus:
        return "Cactus";
        break;
    case objectCypress:
        return "Cypress";
        break;
    case objectMaple:
        return "Maple";
        break;
    case objectPine:
        return "Pine";
        break;
    case objectStone:
        return "Stone";
        break;
    case objectGold:
        return "Gold";
        break;
    case objectBerry:
        return "Berry bush";
        break;
    default:
        return "Undefined";
    }
}

objectTypes objects::getType()
{
    return this->objectType;
}

int objects::amountOfResourcesLeft()
{
    return this->resourceLeft;
}

std::string objects::nameOfResource()
{
    switch(this->typeOfResource)
    {
    case resourceWood:
        return "Wood";
        break;
    case resourceFood:
        return "Food";
        break;
    case resourceStone:
        return "Stone";
        break;
    case resourceGold:
        return "Gold";
        break;
    default:
        return "undefined";
    }
}

void objects::drawObjectFootprint(objectTypes type, int mouseWorldX, int mouseWorldY)
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

