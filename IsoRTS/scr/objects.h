#ifndef OBJECTS_H
#define OBJECTS_H
#include <vector>
#include <string>
#include "globalfunctions.h"
#include "humanReadableNames.h"
#include "objectTemplates.h"
#include <SFML/Graphics.hpp>
#include "Actors/actorStructs.h"

class objects
{

public:

    objects(objectTypes type, cords location, int objectId);

    void            destroyObject();
    void            drawObjectFootprint(objectTypes type, cords mouseWorld) const;
    void            drawObject(int i, int j);
    void            drawObjectSprite(objectTypes spriteNumber, int i, int j) const;
    void            substractResource();
    int             amountOfResourcesLeft() const;
    std::string     nameOfResource() const;
    sf::IntRect     getLastIntRect() const;

    int             getObjectId() const;
    cords           getLocation() const;
    std::string     getName() const;
    objectTypes     getType() const;
    resourceTypes   getTypeOfResource() const;
    bool            getIsInWorld() const;
    void            takeDamage(const int& amountOfDamage, const int& attackerId, const targetTypes attackerType);

private:
    sf::IntRect   _lastIntRect;
    int           _objectId;
    objectTypes   _objectType;
    cords         _location;
    resourceTypes _typeOfResource;
    int           _resourceLeft;
    bool          _isInWorld;
    int           _health = 250;
    int           _maxHealth = 250;
    
   

};
extern std::vector<objects> listOfObjects;

#endif // OBJECTS_H