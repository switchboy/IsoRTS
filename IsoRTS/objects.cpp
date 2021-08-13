#include "objects.h"
#include "gamestate.h"

std::vector<objects> listOfObjects;

objects::objects(objectTypes type, cords location, int objectId)
{
    this->objectType = type;
    this->location = location;
    this->objectId = objectId;
    this->typeOfResource = listOfObjectTemplates[static_cast<uint32_t>(type)].getTypeOfResource();
    this->resourceLeft = listOfObjectTemplates[static_cast<uint32_t>(type)].getStartAmountOfResources();
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
    if (currentGame.objectSelectedId == this->objectId) {
        currentGame.spriteSelectedTile.setPosition(static_cast<float>(worldSpace({ i, j }).x), static_cast<float>(worldSpace({ i, j }).y));
        window.draw(currentGame.spriteSelectedTile);
    }
    drawObjectSprite(this->objectType, i, j);
}

resourceTypes objects::getTypeOfResource() const
{
    return this->typeOfResource;
}

void objects::drawObjectSprite(objectTypes spriteNumber, int i, int j) const
{
    listOfObjectTemplates[static_cast<uint32_t>(this->objectType)].getSprite().setPosition(static_cast<float>(worldSpace({ i,j }).x), static_cast<float>(worldSpace({ i, j }).y));
    window.draw(listOfObjectTemplates[static_cast<uint32_t>(this->objectType)].getSprite());

}

std::string objects::getName() const
{
    return listOfObjectTemplates[static_cast<uint32_t>(this->objectType)].getRealName();
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
