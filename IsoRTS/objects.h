#ifndef OBJECTS_H
#define OBJECTS_H
#include <vector>
#include <string>
#include "globalfunctions.h"
#include "humanReadableNames.h"

class objects
{

public:
    objects(){
        objectId =0;
        objectType = objectTypes::objectBerry;
        locationX = 0;
        locationY = 0;
        typeOfResource = resourceTypes::resourceFood;
        resourceLeft = 0;
    }
    objects(objectTypes type, int startXlocation, int startYLocation, int buildingId);

    void            drawObjectFootprint(objectTypes type, int mouseWorldX, int mouseWorldY) const;
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
    int           locationX;
    int           locationY;
    resourceTypes typeOfResource;
    int           resourceLeft;

};
extern std::vector<objects> listOfObjects;

#endif // OBJECTS_H