#ifndef OBJECTS_H
#define OBJECTS_H
#include <vector>
#include <string>
#include "globalfunctions.h"
#include "resourceids.h"

class objects
{

public:
    objects(){}
    objects(objectTypes type, int startXlocation, int startYLocation, int buildingId);
    std::string     getName();
    objectTypes     getType();
    void            drawObjectFootprint(objectTypes type, int mouseWorldX, int mouseWorldY);
    void            drawObject(int i, int j);
    void            drawObjectSprite(int spriteNumber, int i, int j);
    resourceTypes   getTypeOfResource();
    void            substractResource();
    mouseWorldCord  getLocation();
    int             amountOfResourcesLeft();
    std::string     nameOfResource();

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