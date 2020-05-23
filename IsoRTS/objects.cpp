#include "objects.h"
#include "gamestate.h"

std::vector<objects> listOfObjects;

objects::objects(int type, int startXlocation, int startYLocation, int objectId)
{
    this->objectType = type;
    this->locationX = startXlocation;
    this->locationY = startYLocation;
    this->objectId =objectId;
    switch(type)
    {
    case 0:
        this->typeOfResource = 0; //wood
        break;
    case 1:
        this->typeOfResource = 0;
        break;
    case 2:
        this->typeOfResource = 0;
        break;
    case 3:
        this->typeOfResource = 0;
        break;
    case 4:
        this->typeOfResource = 2;//stone
        break;
    case 5:
        this->typeOfResource = 3;//gold
        break;
    case 6:
        this->typeOfResource = 1; //food
        break;
    }

    switch(this->typeOfResource)
    {
    case 0:
        this->resourceLeft = 200;
        break;
    case 1:
        this->resourceLeft = 500;
        break;
    case 2:
        this->resourceLeft = 2000;
        break;
    case 3:
        this->resourceLeft = 1500;
        break;
    }
    currentGame.objectLocationList[startXlocation][startYLocation] = objectId;
}

void objects::substractResource()
{
    this->resourceLeft -= 1;
    if(resourceLeft < 0)
    {
        currentGame.objectLocationList[this->locationX][this->locationY] = -1;
    }
}

mouseWorldCord  objects::getLocation() {

    return { this->locationX, this->locationY };
}

void objects::drawObject(int i, int j)
{
    drawObjectSprite(this->objectType, i, j);
}

int objects::getTypeOfResource()
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
    case 0:
        return "Cactus";
        break;
    case 1:
        return "Cypress";
        break;
    case 2:
        return "Maple";
        break;
    case 3:
        return "Pine";
        break;
    case 4:
        return "Stone";
        break;
    case 5:
        return "Gold";
        break;
    case 6:
        return "Berry bush";
        break;
    default:
        return "Undefined";
    }
}

int objects::getType()
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
    case 0:
        return "Wood";
        break;
    case 1:
        return "Food";
        break;
    case 2:
        return "Stone";
        break;
    case 3:
        return "Gold";
        break;
    default:
        return "undefined";
    }
}

void objects::drawObjectFootprint(int type, int mouseWorldX, int mouseWorldY)
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

