#pragma once

#include "../buildings.h"

namespace actorHelper
{
    int actorOrientation(int Xc, int Yc, int Xn, int Yn);
    int newOrientation(int oldOrientation, int desiredOrientation);
    int adjacentTileIsCorrectDropOffPoint(cords start, resourceTypes resourceGatherd, int team);
    bool isReallyNextToResource(int unitX, int unitY, int resourceX, int resourceY);
    bool checkIfBuildingIsThereAndIncomplete(const cords& buildinCords, const int& buildingId, const int& actorTeam);
    bool isNextToTarget(const cords& a, const cords& b, const int  range);
}