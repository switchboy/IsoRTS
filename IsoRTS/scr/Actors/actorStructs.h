#pragma once

#include "../globalfunctions.h"

struct nearestBuildingTile
{
    float deltaDistance;
    cords location;
    cords actionLocation;
    int buildingId;
    bool isSet;
    int tileId;
};

struct islandCell
{
    cords position;
    int cellId;
    int cellScore;
    int parentId;
};

struct routeCell
{
    cords position;
    bool visited;
    int parentCellId;
    int backParent;
};

struct orderStack
{
    cords goal;
    stackOrderTypes orderType;
};

struct unfinischedWalking {
    int timePassedSinceChangingOffset;
    int timeWalkingBackStarted;
    cords position;
    cords nPosition;
    int speedMultiplier;

    //calculations
    int startDeltaX = position.x - nPosition.x;
    int startDeltaY = position.y - nPosition.y;
    float deltaXCompleted = static_cast<float>(startDeltaX) * ((static_cast<float>(this->timePassedSinceChangingOffset) / 1000.f) * (static_cast<float>(this->speedMultiplier) / 1000.f));
    float deltaYCompleted = static_cast<float>(startDeltaY) * ((static_cast<float>(this->timePassedSinceChangingOffset) / 1000.f) * (static_cast<float>(this->speedMultiplier) / 1000.f));
};

struct drawXYOverride {
    bool isActive;
    cords newXY;
};

struct pathedRoute {
    std::list<routeCell> route;
    bool pathFound = false;
    bool realPath = false;
    int timeLastPathTry = 0;
};