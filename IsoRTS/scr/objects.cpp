#include "objects.h"
#include "gamestate.h"
#include "Actors/actorStructs.h"

std::vector<objects> listOfObjects;

objects::objects(objectTypes type, cords location, int objectId)
{
    _objectType = type;
    _location = location;
    _objectId = objectId;
    _isInWorld = true;
    _typeOfResource = listOfObjectTemplates[static_cast<uint32_t>(type)].getTypeOfResource();
    _resourceLeft = listOfObjectTemplates[static_cast<uint32_t>(type)].getStartAmountOfResources();
    currentGame.objectLocationList[location.x][location.y] = objectId;
    currentGame.setObjectsHaveChanged();
}

void objects::substractResource()
{
    _resourceLeft -= 1;
    if(_resourceLeft <= 0)
    {
        destroyObject();
    }
}



int objects::getObjectId() const
{
    return _objectId;
}

cords objects::getLocation() const {
    return _location;
}

void objects::drawObject(int i, int j)
{
    if (currentGame.objectSelectedId == _objectId) {
        sf::CircleShape selectionCircle(static_cast<float>(32));
        selectionCircle.setFillColor(sf::Color(255, 255, 255, 0));
        selectionCircle.setOutlineThickness(3.f);
        selectionCircle.setOutlineColor(sf::Color(255, 234, 0));
        selectionCircle.scale(static_cast<float>(1.1), static_cast<float>(0.5));
        selectionCircle.setPosition(static_cast<float>(worldSpace({ i,j }).x), static_cast<float>(worldSpace({ i,j }).y));
        window.draw(selectionCircle);
    }
    drawObjectSprite(_objectType, i, j);
    _lastIntRect = static_cast<sf::IntRect>(listOfObjectTemplates[static_cast<int>(_objectType)].getSprite().getGlobalBounds());
}


resourceTypes objects::getTypeOfResource() const
{
    return _typeOfResource;
}

bool objects::getIsInWorld() const
{
    return _isInWorld;
}

void objects::takeDamage(const int& amountOfDamage, const int& attackerId, const targetTypes attackerType)
{
    _health -= amountOfDamage;
    if (_health <= 0) {
        _health = 0;
        destroyObject();
    }
}

void objects::drawObjectSprite(objectTypes spriteNumber, int i, int j) const
{
    listOfObjectTemplates[static_cast<uint32_t>(_objectType)].getSprite().setPosition(static_cast<float>(worldSpace({ i,j }).x), static_cast<float>(worldSpace({ i, j }).y));
    window.draw(listOfObjectTemplates[static_cast<uint32_t>(_objectType)].getSprite());

}

std::string objects::getName() const
{
    return listOfObjectTemplates[static_cast<uint32_t>(_objectType)].getRealName();
}

objectTypes objects::getType() const
{
    return _objectType;
}

int objects::amountOfResourcesLeft() const
{
    return _resourceLeft;
}

std::string objects::nameOfResource() const
{
    switch(_typeOfResource)
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

sf::IntRect objects::getLastIntRect() const
{
    return _lastIntRect;
}

void objects::destroyObject()
{
    currentGame.objectLocationList[_location.x][_location.y] = -1;
    currentGame.setObjectsHaveChanged();
    _isInWorld = false;
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
