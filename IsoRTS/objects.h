#ifndef OBJECTS_H
#define OBJECTS_H
#include <vector>
#include <string>
#include "globalfunctions.h"
#include "humanReadableNames.h"
#include "objectTemplates.h"

class objects
{

public:
    objects(){
        objectId =0;
        objectType = objectTypes::objectBerry;
        location = { 0, 0 };
        typeOfResource = resourceTypes::resourceFood;
        resourceLeft = 0;
    }
    objects(objectTypes type, cords location, int objectId);

    void            destroyObject();
    void            drawObjectFootprint(objectTypes type, cords mouseWorld) const;
    void            drawObject(int i, int j) const;
    void            drawObjectSprite(objectTypes spriteNumber, int i, int j) const;
    void            substractResource();
    int             amountOfResourcesLeft() const;
    std::string     nameOfResource() const;

    cords           getLocation() const;
    std::string     getName() const;
    objectTypes     getType() const;
    resourceTypes   getTypeOfResource() const;

private:
    int           objectId;
    objectTypes   objectType;
    cords         location;
    resourceTypes typeOfResource;
    int           resourceLeft;

};
extern std::vector<objects> listOfObjects;

#endif // OBJECTS_H