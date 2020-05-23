#ifndef OBJECTS_H
#define OBJECTS_H
#include <vector>
#include <string>
#include "globalfunctions.h"

class objects
{

public:
    objects(){}
    objects(int type, int startXlocation, int startYLocation, int buildingId);
    std::string     getName();
    int             getType();
    void            drawObjectFootprint(int type, int mouseWorldX, int mouseWorldY);
    void            drawObject(int i, int j);
    void            drawObjectSprite(int spriteNumber, int i, int j);
    int             getTypeOfResource();
    void            substractResource();
    mouseWorldCord  getLocation();
    int             amountOfResourcesLeft();
    std::string     nameOfResource();

private:
    int     objectId;
    int     objectType;
    int     locationX;
    int     locationY;
    int     typeOfResource;
    int     resourceLeft;

};
extern std::vector<objects> listOfObjects;

#endif // OBJECTS_H
