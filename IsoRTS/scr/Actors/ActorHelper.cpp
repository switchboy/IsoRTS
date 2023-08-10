#include "ActorHelper.h"
#include "Actor.h"


int actorHelper::actorOrientation(int Xc, int Yc, int Xn, int Yn)
{

    //Orientation:
    //0 N       0   degrees     = x-1  y-1
    //1 NE      45  degrees     = x    y-1
    //2 E       90  degrees     = x+1  y-1
    //3 SE      135 degrees     = x+1  y
    //4 S       180 degrees     = x+1  y+1
    //5 SW      225 degrees     = x    y+1
    //6 W       270 degrees     = x-1  y+1
    //7 NW      315 degrees     = x-1  y
    int diffX = Xn - Xc;
    int diffY = Yn - Yc;

    if (diffX < 0) {
        diffX = -1;
    }
    if (diffX > 0) {
        diffX = 1;
    }

    if (diffY > 0) {
        diffY = 1;
    }

    if (diffY < 0) {
        diffY = -1;
    }

    switch (diffX)
    {
    case -1:
        switch (diffY)
        {
        case -1:
            return 0;
        case 0:
            return 7;
        case 1:
            return 6;
        }
    case 0:
        switch (diffY)
        {
        case -1:
            return 1;
        case 1:
            return 5;
        }
    case 1:
        switch (diffY)
        {
        case -1:
            return 2;
        case 0:
            return 3;
        case 1:
            return 4;
        }
    default:
        return 0;
    }
}

int actorHelper::newOrientation(int oldOrientation, int desiredOrientation)
{
    //int differenceInOrientation = (oldOrientation + desiredOrientation)- desiredOrientation;
    int output;
    int amountOfStepsRight;
    int amountOfStepsLeft;
    //calcualte amount of tik's going right en left
    if (oldOrientation < desiredOrientation)
    {
        amountOfStepsRight = desiredOrientation - oldOrientation;
        amountOfStepsLeft = (oldOrientation + 8) - desiredOrientation;
    }
    else if (oldOrientation > desiredOrientation)
    {
        amountOfStepsRight = (desiredOrientation + 8) - oldOrientation;
        amountOfStepsLeft = oldOrientation - desiredOrientation;
    }
    else
    {
        amountOfStepsRight = 0;
        amountOfStepsLeft = 0;
    }
    if (amountOfStepsLeft < amountOfStepsRight)
    {
        output = oldOrientation - 1;
    }
    else if (amountOfStepsLeft > amountOfStepsRight)
    {
        output = oldOrientation + 1;
    }
    else if (amountOfStepsLeft == amountOfStepsRight && amountOfStepsLeft != 0)
    {
        output = oldOrientation + 1;
    }
    else
    {
        output = oldOrientation;
    }
    if (output < 0)
    {
        output = 7;
    }
    if (output > 7)
    {
        output = 0;
    }
    return output;
}

int actorHelper::adjacentTileIsCorrectDropOffPoint(cords start, resourceTypes resourceGatherd, int team) {
    for (int xOffset = start.x - 1; xOffset < start.x + 2; xOffset++)
    {
        for (int yOffset = start.y - 1; yOffset < start.y + 2; yOffset++)
        {
            if (xOffset >= 0 && xOffset < MAP_WIDTH && yOffset >= 0 && yOffset < MAP_HEIGHT) {
                if (currentGame.occupiedByBuildingList[xOffset][yOffset] != -1)
                {
                    int i = currentGame.occupiedByBuildingList[xOffset][yOffset];
                    if ((listOfBuildings[i].getRecievesWhichResources() == resourceGatherd || listOfBuildings[i].getRecievesWhichResources() == resourceTypes::All) && listOfBuildings[i].getTeam() == team && listOfBuildings[i].getCompleted())
                    {
                        return i;
                    }
                }
            }
        }
    }
    return -1;
}

bool actorHelper::isReallyNextToResource(int unitX, int unitY, int resourceX, int resourceY) {
    int distX = unitX - resourceX;
    int distY = unitY - resourceY;
    if ((distX == 0 || distX == 1 || distX == -1) && (distY == 0 || distY == 1 || distY == -1)) {
        return true;
    }
    else {
        return false;
    }
}

bool actorHelper::checkIfBuildingIsThereAndIncomplete(const cords& buildinCords, const int& buildingId, const int& actorTeam)
{
    if (buildinCords.x < 0 || buildinCords.x > MAP_WIDTH || buildinCords.y < 0 || buildinCords.y > MAP_HEIGHT) {
        return false;
    }
    if (currentGame.occupiedByBuildingList[buildinCords.x][buildinCords.y] == -1) {
        return false;
    }
    if (listOfBuildings[buildingId].getCompleted() ||
        listOfBuildings[buildingId].getTeam() != actorTeam) {
        return false;
    }
    return true;
}