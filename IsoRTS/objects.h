#ifndef OBJECTS_H
#define OBJECTS_H
#include <vector>
#include <string>
#include "globalfunctions.h"
#include "humanReadableNames.h"

class objects
{

public:
    objects(){}
    objects(objectTypes type, int startXlocation, int startYLocation, int buildingId);
    std::string     getName() const;
    objectTypes     getType() const;
    void            drawObjectFootprint(objectTypes type, int mouseWorldX, int mouseWorldY) const;
    void            drawObject(int i, int j) const;
    void            drawObjectSprite(int spriteNumber, int i, int j) const;
    resourceTypes   getTypeOfResource() const;
    void            substractResource();
    cords  getLocation() const;
    int             amountOfResourcesLeft() const;
    std::string     nameOfResource() const;


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