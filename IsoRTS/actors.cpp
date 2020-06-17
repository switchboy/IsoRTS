#include "actors.h"
#include "objects.h"
#include "player.h"
#include "buildings.h"
#include <iostream>
#include <future>
#include <string>
#include "globalfunctions.h"
#include "gametext.h"
#include "projectile.h"

std::vector<actorOrBuildingPrice> priceOfActor;
std::list<int> listOfActorsWhoNeedAPath;
std::vector <actors> listOfActors;
std::mutex listOfActorsMutex;

void updateCells(int goalId, int startId, std::vector<Cells> &cellsList)
{
    int n = 0;
    for(int i = 0; i < MAP_WIDTH; i++)
    {
        for(int j = 0; j < MAP_HEIGHT; j++)
        {
            Cells test;
            cellsList.push_back(test);
            cellsList[n].positionX = i;
            cellsList[n].positionY = j;
            if(n == goalId || n == startId)
            {
                cellsList[n].obstacle = false;
            }
            else if(!currentGame.isPassable(i, j))
            {
                cellsList[n].obstacle = true;
            }
            else
            {
                cellsList[n].obstacle = false;
            }
            cellsList[n].costToGoal = NULL;
            cellsList[n].visited = false;
            cellsList[n].parentCellId = NULL;
            cellsList[n].cummulativeCost = NULL;
            cellsList[n].totalCostGuess = NULL;
            cellsList[n].cellId = n;
            n++;
        }
    }
}

void Cells::addNeighbours(std::vector<Cells> &cellsList)
{
    if(this->positionX > 0)
    {
        //de cell links kan toegevoegd worden
        if(!cellsList[this->cellId-MAP_HEIGHT].obstacle)
        {
            this->neighbours.push_back(this->cellId-MAP_HEIGHT);
        }
    }
    if(this->positionX < MAP_WIDTH-1)
    {
        //de cell rechts kan toegevoegd worden
        if(!cellsList[this->cellId+MAP_HEIGHT].obstacle)
        {
            this->neighbours.push_back(this->cellId+MAP_HEIGHT);
        }
    }
    if(this->positionY > 0)
    {
        //de cell erboven kan toegevogd worden
        if(!cellsList[this->cellId-1].obstacle)
        {
            this->neighbours.push_back(this->cellId-1);
        }
    }
    if(this->positionY != MAP_HEIGHT-1)
    {
        //de cell eronder kan toegevoegd worden
        if(!cellsList[this->cellId+1].obstacle)
        {
            this->neighbours.push_back(this->cellId+1);
        }
    }
    //schuin gaan...
    if(this->positionY != MAP_HEIGHT-1 && this->positionX < MAP_WIDTH-1)
    {
        //de cell rechtsonder kan toegevoegd worden
        if(!cellsList[this->cellId+1+MAP_HEIGHT].obstacle )
        {
            if(!(cellsList[this->cellId+1].obstacle  && cellsList[this->cellId+MAP_HEIGHT].obstacle))
            {
                this->neighbours.push_back(this->cellId+1+MAP_HEIGHT);
            }
        }
    }
    if(this->positionY >0 && this->positionX < MAP_WIDTH-1)
    {
        //de cell rechtsboven kan toegevoegd worden
        if(!cellsList[this->cellId-1+MAP_HEIGHT].obstacle)
        {
            if(!(cellsList[this->cellId-1].obstacle && cellsList[this->cellId+MAP_HEIGHT].obstacle))
            {
                this->neighbours.push_back(this->cellId-1+MAP_HEIGHT);
            }
        }
    }
    if(this->positionY != MAP_HEIGHT-1 && this->positionX > 0)
    {
        //de cell linksonder kan toegevoegd worden
        if(!cellsList[this->cellId+1-MAP_HEIGHT].obstacle)
        {
            if(!(cellsList[this->cellId+1].obstacle && cellsList[this->cellId-MAP_HEIGHT].obstacle))
            {
                this->neighbours.push_back(this->cellId+1-MAP_HEIGHT);
            }
        }
    }
    if(this->positionY >0 && this->positionX > 0)
    {
        //de cell rechtsboven kan toegevoegd worden
        if(!cellsList[this->cellId-1-MAP_HEIGHT].obstacle)
        {
            if(!(cellsList[this->cellId-MAP_HEIGHT].obstacle && cellsList[this->cellId-1].obstacle))
            {
                this->neighbours.push_back(this->cellId-1-MAP_HEIGHT);
            }
        }
    }
}

double dist(double x1, double y1, double x2, double y2)
{
//Manhattan distance
    return fabs(x2 - x1) + fabs(y2 - y1);
}

double distEuclidean(double x1, double y1, double x2, double y2)
{
    //Euclidean distance
    return sqrt(pow((x1 - x2), 2) + pow((y1 - y2), 2));
}

actors::actors(int type, int actorX, int actorY, int actorTeam, int actorId)
{
    currentGame.occupiedByActorList[actorX][actorY] = actorId;
    listOfPlayers[actorTeam].addToCurrentPopulation(1);
    this->actorType = type;
    this->actorTeam = actorTeam;
    this->actorId = actorId;
    this->actorCords[0] = actorX;
    this->actorCords[1] = actorY;
    switch(type)
    {
    case 0://villager
        this->actorHealth = 25;
        this->hitPoints = 25;
        this->meleeDamage = 9;
        this->range = 0;
        this->rangedDamage = 0;
        this->timeBetweenShots = 2.0f;
        this->splashDamage = 0;
        this->projectileType = 0;
        this->doesRangedDamage = false;
        break;
    case 1://swordsman
        this->actorHealth = 60;
        this->hitPoints = 60;
        this->meleeDamage = 12;
        this->range = 0;
        this->rangedDamage = 0;
        this->timeBetweenShots = 2.0f;
        this->splashDamage = 0;
        this->projectileType = 0;
        this->doesRangedDamage = false;
        break;
    }
    this->actorAlive = true;
    this->actorGoal[0] = actorX;
    this->actorGoal[1] = actorY;
    this->orientation = 0;
    this->spriteYOffset = 0;
    this->retries = 0;
    this->amountOfFood = 0;
    this->amountOfGold = 0;
    this->amountOfStone = 0;
    this->amountOfWood = 0;
    this->timeLastAttempt = 1.0f;
    this->timeLastOffsetChange = 0.0f;
    this->timeStartedWalkingToRecource = 0.0f;
    this->goalNeedsUpdate = false;
    this->routeNeedsPath = false;
    this->isBuilding = false;
    this->busyWalking = false;
    this->pathFound = false;
    this->isAtRecource = false;
    this->isGatheringRecources = false;
    this->hasToUnloadResource = false;
    this->isBackAtOwnSquare = false;
    this->offSetX = 0.0f;
    this->offSetY = 0.0f;
    this->hasMoved= false;
    this->initialized = true;
    this->ResourceBeingGatherd = false;
    this->actorAlive = true;
    this->carriesRecources = false;
    this->dropOffTile = {0,0,0,0,false };
    this->actionPreformedOnTile[0] = 0;
    this->actionPreformedOnTile[1] = 0;
    this->hasUnloaded = false;
    this->isAtCarryCapacity = false;
    this->isWalkingToUnloadingPoint = false;
    this->isMeleeAttacking = false;
}

actors::~actors()
{
    //dtor
}

void actors::update()
{
    if (this->initialized && this->actorAlive)
    {
        if (this->goalNeedsUpdate)
        {
            this->updateGoalPath();
        }
        else if (!this->routeNeedsPath)
        {
            this->moveActorIfWalking();
            this->doTaskIfNotWalking();
        }
        this->houseKeeping();
    }
}

void actors::doTaskIfNotWalking()
{
    if (!this->busyWalking) {
        if (this->pathFound && !this->route.empty())
        {
            this->walkToNextSquare();
        }
        else if (this->isMeleeAttacking) {
            this->doMeleeDamage();
        }
        else if (this->isRangedAttacking) {
            this->shootProjectile();
        }
        else if (this->isGatheringRecources && (!this->busyWalking) && this->route.empty())
        {
            this->handleResourceGathering();
        }
        else if (this->isBuilding && (!this->busyWalking) && this->route.empty())
        {
            this->handleBuilding();
        }
    }
}

void actors::shootProjectile()
{
    if (this->timeStartedGatheringRecource + this->timeBetweenShots < currentGame.getTime()) {
        this->timeStartedGatheringRecource = currentGame.getTime();
        if (currentGame.occupiedByActorList[this->actionPreformedOnTile[0]][this->actionPreformedOnTile[1]] != -1) {
            projectile newProjectile(this->actorCords[0], this->actorCords[1], this->actionPreformedOnTile[0], this->actionPreformedOnTile[1], this->projectileType, this->rangedDamage, this->splashDamage);
            listOfProjectiles.push_back(newProjectile);
        }
        else {
            this->isRangedAttacking = false;
        }
    }
}

void actors::setIsDoingAttack()
{
    if (this->doesRangedDamage) {
        this->isRangedAttacking = true;
    }
    else {
        this->isMeleeAttacking = true;
    }
    this->actionPreformedOnTile[0] = this->actorGoal[0];
    this->actionPreformedOnTile[1] = this->actorGoal[1];
}

void actors::doMeleeDamage()
{
    if (this->timeStartedGatheringRecource == 0.0f) 
    {
        gameText.addNewMessage("We have engaged the Enemy!", 1);    
    }
    if (currentGame.elapsedTime - this->timeStartedGatheringRecource > 1) 
    {
        this->timeStartedGatheringRecource = currentGame.elapsedTime;
        if (currentGame.occupiedByActorList[this->actionPreformedOnTile[0]][this->actionPreformedOnTile[1]] != -1) {
           if (listOfActors[currentGame.occupiedByActorList[this->actionPreformedOnTile[0]][this->actionPreformedOnTile[1]]].getTeam() != this->actorTeam) 
           {
                listOfActors[currentGame.occupiedByActorList[this->actionPreformedOnTile[0]][this->actionPreformedOnTile[1]]].takeDamage(this->meleeDamage);
           }
        }
        else if (currentGame.occupiedByBuildingList[this->actionPreformedOnTile[0]][this->actionPreformedOnTile[1]] != -1)
        {
            if (listOfBuildings[currentGame.occupiedByBuildingList[this->actionPreformedOnTile[0]][this->actionPreformedOnTile[1]]].getTeam() != this->actorTeam) 
            {
                listOfBuildings[currentGame.occupiedByBuildingList[this->actionPreformedOnTile[0]][this->actionPreformedOnTile[1]]].takeDamage(this->meleeDamage);
            }
        }
        else {
            this->isMeleeAttacking = false;
        }
    }
}

void actors::takeDamage(int amountOfDamage)
{
    this->actorHealth -= amountOfDamage;
    if (this->actorHealth <= 0) {
        this->killActor();
    }
}

void actors::killActor() {
    this->actorAlive = false;
    currentGame.occupiedByActorList[this->actorCords[0]][this->actorCords[1]] = -1;
}

bool actors::canTargetBeReached()
{
    int sourceX = this->actorCords[0];
    int sourceY = this->actorCords[1];
    int targetX = this->actorGoal[0];
    int targetY = this->actorGoal[1];
    int maxCellId = MAP_HEIGHT * MAP_WIDTH;
    std::vector<std::vector<int>> cellsList;
    std::list <islandCell> toCheckListFromSource;
    std::list <islandCell> toCheckListFromTarget;
    cellsList.assign(maxCellId, std::vector <int>(2, 0));;
    int startCell = (sourceX * MAP_HEIGHT) + sourceY;
    int endCell = (targetX * MAP_HEIGHT) + targetY;
    int collisionCellId = 0;
    cellsList[startCell][0] = 1;
    cellsList[endCell][1] = 1;
    toCheckListFromSource.push_back({ sourceX, sourceY, startCell, 0, 0 });
    toCheckListFromTarget.push_back({ targetX, targetY, endCell, 0, 0 });
    bool cellIsInBothLists = false;
    int cellScore = 0;
    while (!toCheckListFromSource.empty() && !toCheckListFromTarget.empty() && !cellIsInBothLists)
    {
        for (int i = 0; i < 8; i++)
        {
            int tempSourceCellId;
            int tempSourceX;
            int tempSourceY;
            int tempTargetCellId;
            int tempTargetX;
            int tempTargetY;
            switch (i)
            {
            case 0:
                //north
                tempSourceX = toCheckListFromSource.front().positionX;
                tempSourceY = toCheckListFromSource.front().positionY - 1;
                tempTargetX = toCheckListFromTarget.front().positionX;
                tempTargetY = toCheckListFromTarget.front().positionY - 1;
                break;
            case 1:

                //NorthEast
                tempSourceX = toCheckListFromSource.front().positionX + 1;
                tempSourceY = toCheckListFromSource.front().positionY - 1;
                tempTargetX = toCheckListFromTarget.front().positionX + 1;
                tempTargetY = toCheckListFromTarget.front().positionY - 1;
                break;
            case 2:
                //East
                tempSourceX = toCheckListFromSource.front().positionX + 1;
                tempSourceY = toCheckListFromSource.front().positionY;
                tempTargetX = toCheckListFromTarget.front().positionX + 1;
                tempTargetY = toCheckListFromTarget.front().positionY;
                break;
            case 3:
                //SouthEast
                tempSourceX = toCheckListFromSource.front().positionX + 1;
                tempSourceY = toCheckListFromSource.front().positionY + 1;
                tempTargetX = toCheckListFromTarget.front().positionX + 1;
                tempTargetY = toCheckListFromTarget.front().positionY + 1;
                break;
            case 4:
                //South
                tempSourceX = toCheckListFromSource.front().positionX;
                tempSourceY = toCheckListFromSource.front().positionY + 1;
                tempTargetX = toCheckListFromTarget.front().positionX;
                tempTargetY = toCheckListFromTarget.front().positionY + 1;
                break;
            case 5:
                //SouthWest
                tempSourceX = toCheckListFromSource.front().positionX - 1;
                tempSourceY = toCheckListFromSource.front().positionY + 1;
                tempTargetX = toCheckListFromTarget.front().positionX - 1;
                tempTargetY = toCheckListFromTarget.front().positionY + 1;
                break;
            case 6:
                //West
                tempSourceX = toCheckListFromSource.front().positionX - 1;
                tempSourceY = toCheckListFromSource.front().positionY;
                tempTargetX = toCheckListFromTarget.front().positionX - 1;
                tempTargetY = toCheckListFromTarget.front().positionY;
                break;
            case 7:
                //NorthWest
                tempSourceX = toCheckListFromSource.front().positionX - 1;
                tempSourceY = toCheckListFromSource.front().positionY - 1;
                tempTargetX = toCheckListFromTarget.front().positionX - 1;
                tempTargetY = toCheckListFromTarget.front().positionY - 1;
                break;
            }
            tempSourceCellId = (tempSourceX * MAP_HEIGHT) + tempSourceY;
            tempTargetCellId = (tempTargetX * MAP_HEIGHT) + tempTargetY;
            if (tempSourceCellId >= 0 && tempSourceCellId < maxCellId)
            {
                if (currentGame.isPassable(tempSourceX, tempSourceY))
                {
                    if (cellsList[tempSourceCellId][0] == 0)
                    {
                        toCheckListFromSource.push_back({ tempSourceX, tempSourceY, tempSourceCellId, cellScore, toCheckListFromSource.front().cellId });
                        cellsList[tempSourceCellId][0] = 1;
                        if (cellsList[tempSourceCellId][1] == 1)
                        {
                            cellIsInBothLists = true;
                            collisionCellId = tempSourceCellId;
                            i = 8;
                        }
                    }
                }
            }
            if (tempTargetCellId >= 0 && tempTargetCellId < maxCellId)
            {
                if (currentGame.isPassable(tempTargetX, tempTargetY))
                {
                    if (cellsList[tempTargetCellId][1] == 0)
                    {
                        toCheckListFromTarget.push_back({ tempTargetX, tempTargetY, tempSourceCellId, cellScore, toCheckListFromTarget.front().cellId });
                        cellsList[tempTargetCellId][1] = 1;
                        if (cellsList[tempTargetCellId][0] == 1)
                        {
                            cellIsInBothLists = true;
                            collisionCellId = tempTargetCellId;
                            i = 8;
                        }
                    }
                }
            }
        }
        toCheckListFromSource.pop_front();
        toCheckListFromTarget.pop_front();
        cellScore += +1;
    }
    return cellIsInBothLists;
}

int actors::getType()
{
    return this->actorType;
}

std::string actors::nameOfActor()
{
    switch(this->actorType)
    {
    case 0:
        return  "Villager";
        break;
    case 1:
        return "Swordsman";
        break;
    }
}

void actors::updateGoal(int i, int j, int waitTime)
{
    //check if values are in bounds
    if((i >= 0 && i < MAP_WIDTH) && (j >= 0 && j < MAP_HEIGHT))
    {
        if(this->isBuilding)
        {
            listOfBuildings[this->buildingId].removeActorFromBuildingTile(this->actorId);
        }
        this->actorGoal[0] = i;
        this->actorGoal[1] = j;
        this->waitForAmountOfFrames = waitTime;
        this->goalNeedsUpdate = true;
        listOfActorsWhoNeedAPath.push_back(this->actorId);
        this->busyWalking = false;
        this->pathFound = false;
        this->isAtRecource = false;
        this->isBackAtOwnSquare = false;
        this->isRangedAttacking = false;
        this->isMeleeAttacking = false;
        this->offSetX = 0.0f;
        this->offSetY = 0.0f;
        this->timeStartedGatheringRecource = 0.0f;
    }
}

int actorOrientation(int Xc, int Yc, int Xn, int Yn)
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
    int diffX = Xn-Xc;
    int diffY = Yn-Yc;

    switch(diffX)
    {
    case -1:
        switch(diffY)
        {
        case -1:
            return 0;
        case 0:
            return 7;
        case 1:
            return 6;
        }
    case 0:
        switch(diffY)
        {
        case -1:
            return 1;
        case 1:
            return 5;
        }
    case 1:
        switch(diffY)
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

int newOrientation(int oldOrientation, int desiredOrientation)
{
    int differenceInOrientation = (oldOrientation + desiredOrientation)- desiredOrientation;
    int output;
    int amountOfStepsRight;
    int amountOfStepsLeft;
    //calcualte amount of tik's going right en left
    if(oldOrientation < desiredOrientation)
    {
        amountOfStepsRight = desiredOrientation - oldOrientation;
        amountOfStepsLeft = (oldOrientation+8) - desiredOrientation;
    }
    else if(oldOrientation > desiredOrientation)
    {
        amountOfStepsRight = (desiredOrientation+8) - oldOrientation;
        amountOfStepsLeft = oldOrientation - desiredOrientation;
    }
    else
    {
        amountOfStepsRight = 0;
        amountOfStepsLeft = 0;
    }
    if(amountOfStepsLeft < amountOfStepsRight)
    {
        output = oldOrientation -1;
    }
    else if (amountOfStepsLeft > amountOfStepsRight)
    {
        output = oldOrientation +1;
    }
    else if(amountOfStepsLeft == amountOfStepsRight && amountOfStepsLeft != 0)
    {
        output = oldOrientation +1;
    }
    else
    {
        output = oldOrientation;
    }
    if(output < 0)
    {
        output = 7;
    }
    if(output > 7)
    {
        output = 0;
    }
    return output;
}

void actors::updateGoalPath()
{
    this->pathFound = false;
    this->busyWalking = false;
    this->route.clear();
    this->retries = 0;
    if (this->waitForAmountOfFrames == 0)
    {
        this->routeNeedsPath = true;
        listOfActorsWhoNeedAPath.push_back(this->actorId);
        this->goalNeedsUpdate = false;
    }
    else
    {
        this->waitForAmountOfFrames += -1;
    }
}

void actors::moveActorIfWalking()
{
    if (this->busyWalking && (currentGame.elapsedTime - this->timeLastUpdate) > 1.0f)
    {
        this->busyWalking = false;
        this->movedMoreThanHalf = false;
        this->actorCords[0] = this->actorGoal[0];
        this->actorCords[1] = this->actorGoal[1];
    }
    else if (this->busyWalking && (currentGame.elapsedTime - this->timeLastUpdate) > 0.5f && !this->movedMoreThanHalf)
    {

        currentGame.occupiedByActorList[this->actorCords[0]][this->actorCords[1]] = -1;
        this->movedMoreThanHalf = true;
    }
}

void actors::clearRoute()
{
    this->pathFound = false;
    this->commonGoal = false;
    this->route.clear();
    if (this->isWalkingToUnloadingPoint)
    {
        this->reachedUnloadingPoint = true;
    }
}

void actors::startWalking() 
{
    this->hasMoved = true;
    this->retries = 0;
    this->busyWalking = true;
    this->timeLastUpdate = currentGame.elapsedTime;
    this->actorGoal[0] = this->route.back().positionX;
    this->actorGoal[1] = this->route.back().positionY;

    currentGame.occupiedByActorList[this->route.back().positionX][this->route.back().positionY] = this->actorId;

    this->route.pop_back();
    if (route.empty())
    {
        this->pathFound = false;
        this->commonGoal = false;
        if (this->isWalkingToUnloadingPoint)
        {
            this->reachedUnloadingPoint = true;
        }
    }
}

void actors::retryWalkingOrChangeGoal() {
    if (this->retries < 100)
    {
        if (currentGame.elapsedTime - this->timeLastAttempt > 1)
        {
            //there is a problem; do nothing this frame but calculate an alternative route!
            //this->actorGoal[0] = this->route.front().positionX;
            //this->actorGoal[1] = this->route.front().positionY;
            this->routeNeedsPath = true;
            listOfActorsWhoNeedAPath.push_back(this->actorId);
            this->retries += +1;
            this->timeLastAttempt = currentGame.elapsedTime;
        }
    }
    else if (this->isWalkingToUnloadingPoint)
    {
        //find new drop off point
        this->dropOffTile = findNearestDropOffPoint(this->ResourceBeingGatherd);
        if (this->dropOffTile.isSet)
        {
            updateGoal(this->dropOffTile.locationX, this->dropOffTile.locationY, 0);
            this->isWalkingToUnloadingPoint = true;
            this->reachedUnloadingPoint = false;
            this->retries = 0;
        }
        else
        {
            this->clearRoute();
        }
    }
    else if (this->isGatheringRecources)
    {
        if (this->findNearestSimilairResource())
        {
            this->updateGoal(this->actionPreformedOnTile[0], this->actionPreformedOnTile[1], 0);
            this->isWalkingToUnloadingPoint = false;
            this->isAtCarryCapacity = false;
            this->carriesRecources = false;
            this->isAtRecource = false;
            this->hasToUnloadResource = false;
            this->timeStartedWalkingToRecource = 0.0f;
            this->retries = 0;
        }
    }
    else 
    {
        this->clearRoute();
        this->isGatheringRecources = false;
        this->isBuilding = false;
    }
}

void actors::walkToNextSquare() {
    // Deze actor heeft een doel, dit doel is nog niet bereikt en is klaar met het vorige stuk lopen!
    if (actorOrientation(this->actorCords[0], this->actorCords[1], this->route.back().positionX, this->route.back().positionY) == this->orientation)
    {
        if ((this->isGatheringRecources || this->isBuilding || this->isMeleeAttacking) && this->route.size() == 1)
        {
            this->clearRoute();
        }
        else if (this->isRangedAttacking && distEuclidean(this->actorCords[0], this->actorCords[1], this->actorGoal[0], this->actorGoal[1]) <= this->range)
        {
            this->clearRoute();
        }
        else
        {
            //De actor staat met zijn neus de goede kant op en kan dus gaan lopen als de tegel vrij is!
            if (currentGame.occupiedByActorList[this->route.back().positionX][this->route.back().positionY] == -1)
            {
                this->startWalking();
            }
            else {
                this->retryWalkingOrChangeGoal();
            }
        }
    }
    else
    {
        //De actor moet eerst draaien voordat hij kan gaan lopen
        this->orientation = newOrientation(this->orientation, actorOrientation(this->actorCords[0], this->actorCords[1], this->route.back().positionX, this->route.back().positionY));
    }
}

void actors::handleResourceGathering()
{
    if (this->hasToUnloadResource)
    {
        if (!this->isBackAtOwnSquare)
        {
            this->walkBackToOwnSquare();
        }
        else
        {
            if (!this->isWalkingToUnloadingPoint)
            {
                //search the nearest unloading point an walk there
                this->dropOffTile = findNearestDropOffPoint(this->ResourceBeingGatherd);
                if (this->dropOffTile.isSet)
                {
                    updateGoal(this->dropOffTile.locationX, this->dropOffTile.locationY, 0);
                    this->retries = 0;
                    this->isWalkingToUnloadingPoint = true;
                    this->reachedUnloadingPoint = false;
                }
                else
                {
                    //There is no dropoff point!
                }
            }
            else if (this->reachedUnloadingPoint)
            {
                this->unloadAndReturnToGathering();
            }
        }
    }
    else
    {
        //verzamel recources
        if (this->isAtRecource)
        {
            this->gatherResource();
        }
        else if (this->timeStartedWalkingToRecource == 0.0f)
        {
            this->timeStartedWalkingToRecource = currentGame.elapsedTime;
        }
        else if (currentGame.elapsedTime - this->timeStartedWalkingToRecource < 0.5f)
        {
            this->animateWalkingToResource();
        }
        else
        {
            this->startGatheringAnimation();
        }
    }
}

void actors::handleBuilding()
{
    //villager is aangekomen bij te bouwen gebouw en kan na verplaatst te zijn gaan bouwen!
    if (this->isAtRecource)
    {
        this->buildBuilding();
    }
    else if (this->timeStartedWalkingToRecource == 0.0f)
    {
        this->timeStartedWalkingToRecource = currentGame.elapsedTime;
    }
    else if (currentGame.elapsedTime - this->timeStartedWalkingToRecource < 0.5f)
    {
        this->animateWalkingToResource();
    }
    else
    {
        this->startGatheringAnimation();
    }
}

void actors::houseKeeping()
{
    if (!this->busyWalking && this->route.empty() && this->hasMoved)
    {
        this->cleanUp();
        this->hasMoved = false;
    }
}

void actors::walkBackToOwnSquare()
{
    int northSouth;
    int eastWest;
    int diagonalX;
    int diagonalY;
    if(this->ResourceBeingGatherd == 0)
    {
        northSouth = 22;
        eastWest = 55;
        diagonalX = 21;
        diagonalY = 12;
    }
    else
    {
        northSouth = 11;
        eastWest = 27;
        diagonalX = 11;
        diagonalY = 6;
    }
    if(this->timeStartedWalkingToRecource == 0.0f)
    {
        this->timeStartedWalkingToRecource = currentGame.elapsedTime;
    }
    else if(currentGame.elapsedTime - this->timeStartedWalkingToRecource < 0.5f)
    {
        //0 N       0   degrees     = x-1  y-1
        //1 NE      45  degrees     = x    y-1
        //2 E       90  degrees     = x+1  y-1
        //3 SE      135 degrees     = x+1  y
        //4 S       180 degrees     = x+1  y+1
        //5 SW      225 degrees     = x    y+1
        //6 W       270 degrees     = x-1  y+1
        //7 NW      315 degrees     = x-1  y
        switch(this->orientation)
        {
        case 0:
            this->offSetX = 0;
            this->offSetY = -northSouth+((currentGame.elapsedTime - this->timeStartedWalkingToRecource) / 0.5f)*northSouth;
            break;
        case 1:
            this->offSetX = diagonalX-((currentGame.elapsedTime - this->timeStartedWalkingToRecource) / 0.5f)*diagonalX;
            this->offSetY = -diagonalY+((currentGame.elapsedTime - this->timeStartedWalkingToRecource) / 0.5f)*diagonalY;
            break;
        case 2:
            this->offSetX = eastWest-((currentGame.elapsedTime - this->timeStartedWalkingToRecource) / 0.5f)*eastWest;
            this->offSetY = 0;
            break;
        case 3:
            this->offSetX = diagonalX-((currentGame.elapsedTime - this->timeStartedWalkingToRecource) / 0.5f)*diagonalX;
            this->offSetY = diagonalY-((currentGame.elapsedTime - this->timeStartedWalkingToRecource) / 0.5f)*diagonalY;
            break;
        case 4:
            this->offSetX = 0;
            this->offSetY = northSouth-((currentGame.elapsedTime - this->timeStartedWalkingToRecource) / 0.5f)*northSouth;
            break;
        case 5:
            this->offSetX = -diagonalX+((currentGame.elapsedTime - this->timeStartedWalkingToRecource) / 0.5f)*diagonalX;
            this->offSetY = diagonalY-((currentGame.elapsedTime - this->timeStartedWalkingToRecource) / 0.5f)*diagonalY;
            break;
        case 6:
            this->offSetX = -eastWest+((currentGame.elapsedTime - this->timeStartedWalkingToRecource) / 0.5f)*eastWest;
            this->offSetY = 0;
            break;
        case 7:
            this->offSetX = -diagonalX+((currentGame.elapsedTime - this->timeStartedWalkingToRecource) / 0.5f)*diagonalX;
            this->offSetY = -diagonalY+((currentGame.elapsedTime - this->timeStartedWalkingToRecource) / 0.5f)*diagonalY;
            break;
        }
    }
    else
    {
        this->isBackAtOwnSquare = true;
        this->isAtRecource = false;
        this->isBuilding = false;
        this->timeStartedWalkingToRecource = 0.0f;
        this->timeStartedGatheringRecource = currentGame.elapsedTime;
        this->offSetX = 0;
        this->offSetY = 0;
    }
}

void actors::startGatheringAnimation()
{
    int northSouth;
    int eastWest;
    int diagonalX;
    int diagonalY;
    if(this->ResourceBeingGatherd == 0)
    {
        northSouth = 22;
        eastWest = 55;
        diagonalX = 21;
        diagonalY = 12;
    }
    else
    {
        northSouth = 11;
        eastWest = 27;
        diagonalX = 11;
        diagonalY = 6;
    }
    this->isAtRecource = true;
    this->timeStartedWalkingToRecource = 0.0f;
    this->timeStartedGatheringRecource = currentGame.elapsedTime;
    switch(this->orientation)
    {
    case 0:
        this->offSetX = 0;
        this->offSetY = -northSouth;
        break;
    case 1:
        this->offSetX = diagonalX;
        this->offSetY = -diagonalY;
        break;
    case 2:
        this->offSetX = eastWest;
        this->offSetY = 0;
        break;
    case 3:
        this->offSetX = diagonalX;
        this->offSetY = diagonalY;
        break;
    case 4:
        this->offSetX = 0;
        this->offSetY = northSouth;
        break;
    case 5:
        this->offSetX = -diagonalX;
        this->offSetY = diagonalY;
        break;
    case 6:
        this->offSetX = -eastWest;
        this->offSetY = 0;
        break;
    case 7:
        this->offSetX = -diagonalX;
        this->offSetY = -diagonalY;
        break;

    }
}

void actors::animateWalkingToResource()
{
    int northSouth;
    int eastWest;
    int diagonalX;
    int diagonalY;
    if(this->ResourceBeingGatherd == 0)
    {
        northSouth = 22;
        eastWest = 55;
        diagonalX = 21;
        diagonalY = 12;
    }
    else
    {
        northSouth = 11;
        eastWest = 27;
        diagonalX = 11;
        diagonalY = 6;
    }
    //0 N       0   degrees     = x-1  y-1
    //1 NE      45  degrees     = x    y-1
    //2 E       90  degrees     = x+1  y-1
    //3 SE      135 degrees     = x+1  y
    //4 S       180 degrees     = x+1  y+1
    //5 SW      225 degrees     = x    y+1
    //6 W       270 degrees     = x-1  y+1
    //7 NW      315 degrees     = x-1  y
    switch(this->orientation)
    {
    case 0:
        this->offSetX = 0;
        this->offSetY = ((currentGame.elapsedTime - this->timeStartedWalkingToRecource) / 0.5f)*northSouth*-1;
        break;
    case 1:
        this->offSetX = ((currentGame.elapsedTime - this->timeStartedWalkingToRecource) / 0.5f)*diagonalX;
        this->offSetY = ((currentGame.elapsedTime - this->timeStartedWalkingToRecource) / 0.5f)*diagonalY*-1;
        break;
    case 2:
        this->offSetX = ((currentGame.elapsedTime - this->timeStartedWalkingToRecource) / 0.5f)*eastWest;
        this->offSetY = 0;
        break;
    case 3:
        this->offSetX = ((currentGame.elapsedTime - this->timeStartedWalkingToRecource) / 0.5f)*diagonalX;
        this->offSetY = ((currentGame.elapsedTime - this->timeStartedWalkingToRecource) / 0.5f)*diagonalY;
        break;
    case 4:
        this->offSetX = 0;
        this->offSetY = ((currentGame.elapsedTime - this->timeStartedWalkingToRecource) / 0.5f)*northSouth;
        break;
    case 5:
        this->offSetX = ((currentGame.elapsedTime - this->timeStartedWalkingToRecource) / 0.5f)*diagonalX*-1;
        this->offSetY = ((currentGame.elapsedTime - this->timeStartedWalkingToRecource) / 0.5f)*diagonalY;
        break;
    case 6:
        this->offSetX = ((currentGame.elapsedTime - this->timeStartedWalkingToRecource) / 0.5f)*eastWest*-1;
        this->offSetY = 0;
        break;
    case 7:
        this->offSetX = ((currentGame.elapsedTime - this->timeStartedWalkingToRecource) / 0.5f)*diagonalX*-1;
        this->offSetY = ((currentGame.elapsedTime - this->timeStartedWalkingToRecource) / 0.5f)*diagonalY*-1;
        break;
    }
}

void actors::gatherResource()
{
    if(currentGame.objectLocationList[this->actionPreformedOnTile[0]][this->actionPreformedOnTile[1]] != -1)
    {
        if(currentGame.elapsedTime - this->timeStartedGatheringRecource > 2)
        {
            switch(this->ResourceBeingGatherd)
            {
            case 0:  //wood
                this->amountOfWood += +1;
                break;
            case 1: //food
                this->amountOfFood += +1;
                break;
            case 2: //stone
                this->amountOfStone += +1;
                break;
            case 3: // gold
                this->amountOfGold += +1;
                break;
            }
            listOfObjects[currentGame.objectLocationList[this->actionPreformedOnTile[0]][this->actionPreformedOnTile[1]]].substractResource();
            this->carriesRecources = true;
            if((this->amountOfFood == 10) || (this->amountOfWood == 10) ||(this->amountOfStone == 10) ||(this->amountOfGold == 10) )
            {
                this->hasToUnloadResource = true;
                this->isAtRecource = false;
            }
            this->timeStartedGatheringRecource = currentGame.elapsedTime;
        }
    }
    else
    {
        //resource not here!
        this->hasToUnloadResource = true;
        this->isAtRecource = false;
    }
}

void actors::unloadAndReturnToGathering()
{
    switch(listOfBuildings[this->dropOffTile.buildingId].getRecievesWhichResources())
    {
    case 0:
        //recieves only wood
        listOfPlayers[this->actorTeam].addResources(0, this->amountOfWood);
        this->amountOfWood = 0;
        break;
    case 1:
        //recieves only food
        listOfPlayers[this->actorTeam].addResources(1, this->amountOfFood);
        this->amountOfFood = 0;
        break;
    case 2:
        //recieves only stone
        listOfPlayers[this->actorTeam].addResources(2, this->amountOfStone);
        this->amountOfStone = 0;
        break;
    case 3:
        //recieves only gold
        listOfPlayers[this->actorTeam].addResources(3, this->amountOfGold);
        this->amountOfGold = 0;
        break;
    case 4:
        //recieves all the resources!
        listOfPlayers[this->actorTeam].addResources(0, this->amountOfWood);
        this->amountOfWood = 0;
        listOfPlayers[this->actorTeam].addResources(1, this->amountOfFood);
        this->amountOfFood = 0;
        listOfPlayers[this->actorTeam].addResources(2, this->amountOfStone);
        this->amountOfStone = 0;
        listOfPlayers[this->actorTeam].addResources(3, this->amountOfGold);
        this->amountOfGold = 0;
        break;
    }
    if(currentGame.objectLocationList[this->actionPreformedOnTile[0]][this->actionPreformedOnTile[1]] != -1)
    {
        this->updateGoal(this->actionPreformedOnTile[0], this->actionPreformedOnTile[1], 0);
        this->isWalkingToUnloadingPoint = false;
        this->isAtCarryCapacity = false;
        this->carriesRecources = false;
        this->isAtRecource = false;
        this->hasToUnloadResource = false;
        this->timeStartedWalkingToRecource = 0.0f;
    }
    else
    {
        if(this->findNearestSimilairResource())
        {
            this->updateGoal(this->actionPreformedOnTile[0], this->actionPreformedOnTile[1], 0);
            this->isWalkingToUnloadingPoint = false;
            this->isAtCarryCapacity = false;
            this->carriesRecources = false;
            this->isAtRecource = false;
            this->hasToUnloadResource = false;
            this->timeStartedWalkingToRecource = 0.0f;
        }
    }
}

void actors::setGatheringRecource(bool flag)
{
    this->isGatheringRecources = flag;
    this->isBuilding = false;
    if(flag)
    {
        this->ResourceBeingGatherd = listOfObjects[currentGame.objectLocationList[this->actorGoal[0]][this->actorGoal[1]]].getTypeOfResource();
        this->actionPreformedOnTile[0] = this->actorGoal[0];
        this->actionPreformedOnTile[1] = this->actorGoal[1];
    }
}

void actors::setCommonGoalTrue()
{
    this->commonGoal = true;
}

bool actors::findNearestSimilairResource()
{
    std::list <nearestBuildingTile> listOfResourceLocations;
    int lowSearchLimitX = this->actorCords[0]-30;
    if(lowSearchLimitX < 0)
    {
        lowSearchLimitX = 0;
    }
    int lowSearchLimitY = this->actorCords[1]-30;
    if(lowSearchLimitY < 0)
    {
        lowSearchLimitY = 0;
    }
    int highSearchLimitX = this->actorCords[0]+30;
    if(highSearchLimitX > MAP_WIDTH)
    {
        highSearchLimitX = MAP_WIDTH;
    }
    int highSearchLimitY = this->actorCords[0]+30;
    if(highSearchLimitY > MAP_HEIGHT)
    {
        highSearchLimitY = MAP_HEIGHT;
    }
    for(int i = lowSearchLimitX; i < highSearchLimitX; i++)
    {
        for(int j = lowSearchLimitY; j < highSearchLimitY; j++)
        {
            if(currentGame.objectLocationList[i][j] != -1)
            {
                if(listOfObjects[currentGame.objectLocationList[i][j]].getTypeOfResource() == this->ResourceBeingGatherd)
                {
                    float tempDeltaDistance = dist(this->actorCords[0], this->actorCords[1], i, j);
                    listOfResourceLocations.push_back({tempDeltaDistance, i, j, currentGame.objectLocationList[i][j], true});
                }
            }
        }
    }

    if(!listOfResourceLocations.empty())
    {
        listOfResourceLocations.sort([](const nearestBuildingTile  &f, const nearestBuildingTile &s)
        {
            return f.deltaDistance < s.deltaDistance;
        });
    }
    this->pathFound = false;
    while(!this->pathFound && !listOfResourceLocations.empty())
    {
        this->actorGoal[0] = listOfResourceLocations.front().locationX;
        this->actorGoal[1] =listOfResourceLocations.front().locationY;
        this->noPathPossible = false;
        std::thread pathfinding(&actors::pathAStar, &listOfActors[this->actorId]);
        if(!canTargetBeReached())
        {
            this->noPathPossible = true;
        }
        pathfinding.join();
        if(!this->pathFound)
        {
            listOfResourceLocations.pop_front();
        }
    }

    if(!listOfResourceLocations.empty())
    {
        this->actionPreformedOnTile[0] = listOfResourceLocations.front().locationX;
        this->actionPreformedOnTile[1] = listOfResourceLocations.front().locationY;
        return true;
    }
    else
    {
        this->isGatheringRecources = false;
        return false;
    }
}

cords actors::getLocation(){
    return {this->actorCords[0], this->actorCords[1]};
}

nearestBuildingTile actors::findNearestDropOffPoint(int Resource)
{
    std::list <nearestBuildingTile> listOfDropOffLocations;
    for(int i = 0; i < listOfBuildings.size(); i++)
    {
        if((listOfBuildings[i].getRecievesWhichResources() == Resource || listOfBuildings[i].getRecievesWhichResources() == 4) && listOfBuildings[i].getTeam() == this->actorTeam && listOfBuildings[i].getCompleted())
        {
            std::vector<adjacentTile> tileList = listOfBuildings[i].getDropOffTiles();
            for(int j = 0; j < tileList.size(); j++)
            {
                float tempDeltaDistance = dist(this->actorCords[0], this->actorCords[1], tileList[j].goalX, tileList[j].goalY);
                listOfDropOffLocations.push_back({tempDeltaDistance, tileList[j].goalX, tileList[j].goalY, listOfBuildings[i].getBuildingId(), true});
            }
        }
    }
    if(!listOfDropOffLocations.empty())
    {
        listOfDropOffLocations.sort([](const nearestBuildingTile  &f, const nearestBuildingTile &s)
        {
            return f.deltaDistance < s.deltaDistance;
        });
    }
    this->pathFound = false;
    while(!this->pathFound && !listOfDropOffLocations.empty())
    {
        this->actorGoal[0] = listOfDropOffLocations.front().locationX;
        this->actorGoal[1] = listOfDropOffLocations.front().locationY;
        this->noPathPossible = false;
        std::thread pathfinding(&actors::pathAStar, &listOfActors[this->actorId]);
        if(!canTargetBeReached())
        {
            this->noPathPossible = true;
        }
        pathfinding.join();
        if(!this->pathFound)
        {
            listOfDropOffLocations.pop_front();
        }
    }

    if(!listOfDropOffLocations.empty())
    {
        return listOfDropOffLocations.front();
    }
    else
    {
        return {0, 0, 0, 0, false};
    }


}

int actors::getTeam()
{
    return this->actorTeam;
}

bool actors::isInitialized()
{
    return this->initialized;
}

void actors::calculateRoute()
{
    if(this->routeNeedsPath)
    {
        this->noPathPossible = false;
        std::thread pathfinding(&actors::pathAStar, &listOfActors[this->actorId]);
        if (!canTargetBeReached())
        {
            this->noPathPossible = true;
        }
        pathfinding.join();
        this->routeNeedsPath = false;
    }
}

void actors::pathAStar()
{
    std::vector<Cells> cellsList;
    cellsList.reserve(MAP_HEIGHT*MAP_WIDTH);
    int startCell = (actorCords[0]*MAP_HEIGHT)+actorCords[1]; //eigen positie
    int endCell = (actorGoal[0]*MAP_HEIGHT)+actorGoal[1]; //doel positie
    updateCells(endCell, startCell, cellsList);
    std::list<Cells*> listToCheck;
    bool endReached = false;

    //check of de doelcel niet 1 hokje weg is
    if(((actorCords[0]-actorGoal[0] == 0) ||(actorCords[0]-actorGoal[0] == -1) ||(actorCords[0]-actorGoal[0] == 1)) && ((actorCords[1]-actorGoal[1] == 0) ||(actorCords[1]-actorGoal[1] == -1) ||(actorCords[1]-actorGoal[1] == 1)))
    {
        if(!cellsList[endCell].obstacle)
        {
            this->pathFound = true;
            endReached = true;
        }
        else
        {
            this->pathFound = false;
        }
    }
    else
    {
        this->pathFound = false;
        cellsList[startCell].addNeighbours(cellsList);
        cellsList[startCell].visited = true;
        listToCheck.push_back(&cellsList[startCell]);
    }

    while(!listToCheck.empty() && startCell != endCell)
    {
        //sorteer de lijst en zet de cell met de laagste cost to goal bovenaan om het eerst te testen
        listToCheck.sort([](const Cells* f, const Cells* s)
        {
            return f->totalCostGuess < s->totalCostGuess;
        });

        //Check of de te checken cell het doel is. Als dat zo is zijn we klaar
        if((*listToCheck.front()).cellId == endCell)
        {
            listToCheck.clear();
            this->pathFound = true;
        }
        else if(this->noPathPossible)
        {
            listToCheck.clear();
        }
        else if(!listToCheck.empty())
        {
            for (std::vector<int>::const_iterator iterator =  (*listToCheck.front()).neighbours.begin(), end =  (*listToCheck.front()).neighbours.end(); iterator != end; ++iterator)
            {
                //We have found neighbours!
                //check if neighbours was found before
                if(!cellsList[*iterator].visited)
                {
                    //Deze cell heeft geen parent is is dus nooit eerder gevonden! De buren moeten dus toegevoegd worden!
                    cellsList[*iterator].addNeighbours(cellsList);
                    //De cell waarvan we de neighbours onderzoeken is dus automagisch tot nu toe de kortste route hiernaartoe
                    cellsList[*iterator].parentCellId = (*listToCheck.front()).cellId;
                    //Nu moeten de kosten voor de route hiernatoe uitgerekend worden (Dit zijn de kosten van naar de buurman gaan +1
                    cellsList[*iterator].cummulativeCost = (*listToCheck.front()).cummulativeCost+1;
                    //Als laatste zetten we de cell in de lijst met cellen die gecheckt moet worden
                    listToCheck.push_back(&cellsList[*iterator]);
                    //Bereken de afstand naar het doel
                    cellsList[*iterator].costToGoal = dist(cellsList[*iterator].positionX,cellsList[*iterator].positionY,cellsList[endCell].positionX,cellsList[endCell].positionY);
                    cellsList[*iterator].totalCostGuess = cellsList[*iterator].costToGoal + cellsList[*iterator].cummulativeCost;
                    cellsList[*iterator].visited = true;
                }
                else
                {
                    //Deze cell is al eerder gevonden, staat dus al in de te checken cell lijst
                    if((*listToCheck.front()).cummulativeCost+1 < cellsList[*iterator].cummulativeCost)
                    {
                        //Er is een kortere route naar deze cell! Pas de parent cell dus aan en geef een nieuwe cummulative Cost;
                        cellsList[*iterator].parentCellId = (*listToCheck.front()).cellId;
                        cellsList[*iterator].cummulativeCost = (*listToCheck.front()).cummulativeCost+1;
                        cellsList[*iterator].totalCostGuess = cellsList[*iterator].costToGoal + cellsList[*iterator].cummulativeCost;
                    }
                }
            }
            listToCheck.pop_front();
        }
    }
    //Zet de te lopen route in een lijst
    this->route.clear();
    this->route.push_back({cellsList[endCell].positionX, cellsList[endCell].positionY, cellsList[endCell].visited, cellsList[endCell].parentCellId});
    if(this->pathFound)
    {
        while(!endReached)
        {
            if(route.back().visited == true)
            {
                this->route.push_back({cellsList[route.back().parentCellId].positionX, cellsList[route.back().parentCellId].positionY, cellsList[route.back().parentCellId].visited, cellsList[route.back().parentCellId].parentCellId});
                if(this->route.back().parentCellId == startCell)
                {
                    endReached = true;
                }
            }
            else
            {
                endReached = true;
            }
        }
    }
}

void actors::pathAStarBiDi()
{
    std::vector<Cells> cellsList;
    cellsList.reserve(MAP_HEIGHT*MAP_WIDTH);
    int startCell = (actorCords[0]*MAP_HEIGHT)+actorCords[1]; //eigen positie
    int endCell = (actorGoal[0]*MAP_HEIGHT)+actorGoal[1]; //doel positie
    int collisionCell = -1;
    updateCells(endCell, startCell, cellsList);
    std::list<Cells*> listToCheck;
    std::list<Cells*> listToCheckBack;
    bool endReached = false;

    //check of de doelcel niet 1 hokje weg is
    if(((actorCords[0]-actorGoal[0] == 0) ||(actorCords[0]-actorGoal[0] == -1) ||(actorCords[0]-actorGoal[0] == 1)) && ((actorCords[1]-actorGoal[1] == 0) ||(actorCords[1]-actorGoal[1] == -1) ||(actorCords[1]-actorGoal[1] == 1)))
    {
        if(!cellsList[endCell].obstacle)
        {
            this->pathFound = true;
            endReached = true;
        }
        else
        {
            this->pathFound = false;
        }
    }
    else
    {
        this->pathFound = false;
        cellsList[startCell].addNeighbours(cellsList);
        cellsList[endCell].addNeighbours(cellsList);
        cellsList[startCell].visited = true;
        cellsList[endCell].visitedBack = true;
        listToCheck.push_back(&cellsList[startCell]);
        listToCheckBack.push_back(&cellsList[endCell]);
        std::cout << endCell << " - " << (*listToCheckBack.front()).cellId << " | " << startCell << " - " << (*listToCheck.front()).cellId << "\n";
    }


    while((!listToCheck.empty() && !listToCheckBack.empty()) && startCell != endCell)
    {
        //sorteer de lijst en zet de cell met de laagste cost to goal bovenaan om het eerst te testen


        listToCheck.sort([](const Cells* f, const Cells* s)
        {
            return f->totalCostGuess < s->totalCostGuess;
        });

        listToCheckBack.sort([](const Cells* f, const Cells* s)
        {
            return f->totalCostGuess < s->totalCostGuess;
        });

        if(collisionCell != -1)
        {
            listToCheck.clear();
            listToCheckBack.clear();
            this->pathFound = true;
        } //Check of de te checken cell het doel is. Als dat zo is zijn we klaar
        else if((*listToCheck.front()).cellId == endCell)
        {
            listToCheck.clear();
            listToCheckBack.clear();
            this->pathFound = true;
        }
        else if((*listToCheckBack.front()).cellId == startCell)
        {
            listToCheck.clear();
            listToCheckBack.clear();
            this->pathFound = true;
        }
        //Check of de floodfill niet al heeft vastgesteld dat de route niet kan
        else if(this->noPathPossible)
        {
            listToCheck.clear();
        }
        else if(!listToCheck.empty() && !listToCheckBack.empty())
        {
            for (std::vector<int>::const_iterator iterator =  (*listToCheck.front()).neighbours.begin(), end =  (*listToCheck.front()).neighbours.end(); iterator != end; ++iterator)
            {
                //check if neighbours was found before
                if(!cellsList[*iterator].visited)
                {
                    if(!cellsList[*iterator].visitedBack)
                    {
                        //Deze cell heeft geen parent is is dus nooit eerder gevonden! De buren moeten dus toegevoegd worden!
                        cellsList[*iterator].addNeighbours(cellsList);
                        //De cell waarvan we de neighbours onderzoeken is dus automagisch tot nu toe de kortste route hiernaartoe
                        cellsList[*iterator].parentCellId = (*listToCheck.front()).cellId;
                        //Nu moeten de kosten voor de route hiernatoe uitgerekend worden (Dit zijn de kosten van naar de buurman gaan +1
                        cellsList[*iterator].cummulativeCost = (*listToCheck.front()).cummulativeCost+1;
                        //Als laatste zetten we de cell in de lijst met cellen die gecheckt moet worden
                        listToCheck.push_back(&cellsList[*iterator]);
                        //Bereken de afstand naar het doel
                        cellsList[*iterator].costToGoal = dist(cellsList[*iterator].positionX,cellsList[*iterator].positionY,cellsList[endCell].positionX,cellsList[endCell].positionY);
                        cellsList[*iterator].totalCostGuess = cellsList[*iterator].costToGoal + cellsList[*iterator].cummulativeCost;
                        cellsList[*iterator].visited = true;
                    }
                    else
                    {
                        //deze cell is door de andere kant gevonden!
                        collisionCell = *iterator;
                        cellsList[*iterator].visited = true;
                        cellsList[*iterator].visitedBack = true;
                        cellsList[*iterator].parentCellId = (*listToCheck.front()).cellId;
                    }

                }
                else
                {
                    //Deze cell is al eerder gevonden, staat dus al in de te checken cell lijst
                    if((*listToCheck.front()).cummulativeCost+1 < cellsList[*iterator].cummulativeCost)
                    {
                        //Er is een kortere route naar deze cell! Pas de parent cell dus aan en geef een nieuwe cummulative Cost;
                        cellsList[*iterator].parentCellId = (*listToCheck.front()).cellId;
                        cellsList[*iterator].cummulativeCost = (*listToCheck.front()).cummulativeCost+1;
                        cellsList[*iterator].totalCostGuess = cellsList[*iterator].costToGoal + cellsList[*iterator].cummulativeCost;
                    }
                }
            }

            for (std::vector<int>::const_iterator iterator =  (*listToCheckBack.front()).neighbours.begin(), end =  (*listToCheckBack.front()).neighbours.end(); iterator != end; ++iterator)
            {
                //check if neighbours was found before
                if(!cellsList[*iterator].visitedBack)
                {
                    if(!cellsList[*iterator].visited)
                    {
                        //Deze cell heeft geen parent is is dus nooit eerder gevonden! De buren moeten dus toegevoegd worden!
                        cellsList[*iterator].addNeighbours(cellsList);
                        //De cell waarvan we de neighbours onderzoeken is dus automagisch tot nu toe de kortste route hiernaartoe
                        cellsList[*iterator].backParent = (*listToCheckBack.front()).cellId;
                        //Nu moeten de kosten voor de route hiernatoe uitgerekend worden (Dit zijn de kosten van naar de buurman gaan +1
                        cellsList[*iterator].cummulativeCost = (*listToCheckBack.front()).cummulativeCost+1;
                        //Als laatste zetten we de cell in de lijst met cellen die gecheckt moet worden
                        listToCheckBack.push_back(&cellsList[*iterator]);
                        //Bereken de afstand naar het doel
                        cellsList[*iterator].costToGoal = dist(cellsList[*iterator].positionX,cellsList[*iterator].positionY,cellsList[startCell].positionX,cellsList[startCell].positionY);
                        cellsList[*iterator].totalCostGuess = cellsList[*iterator].costToGoal + cellsList[*iterator].cummulativeCost;
                        cellsList[*iterator].visitedBack = true;
                    }
                    else
                    {
                        //deze cell is door de andere kant gevonden!
                        collisionCell = *iterator;
                        cellsList[*iterator].visited = true;
                        cellsList[*iterator].visitedBack = true;
                        cellsList[*iterator].backParent = (*listToCheckBack.front()).cellId;
                    }
                }
                else
                {
                    //Deze cell is al eerder gevonden, staat dus al in de te checken cell lijst
                    if((*listToCheck.front()).cummulativeCost+1 < cellsList[*iterator].cummulativeCost)
                    {
                        //Er is een kortere route naar deze cell! Pas de parent cell dus aan en geef een nieuwe cummulative Cost;
                        cellsList[*iterator].backParent = (*listToCheck.front()).cellId;
                        cellsList[*iterator].cummulativeCost = (*listToCheck.front()).cummulativeCost+1;
                        cellsList[*iterator].totalCostGuess = cellsList[*iterator].costToGoal + cellsList[*iterator].cummulativeCost;
                    }
                }
            }
            listToCheck.pop_front();
            listToCheckBack.pop_front();
        }
    }
    //Zet de te lopen route in een lijst
    this->route.clear();
    this->route.push_back({cellsList[collisionCell].positionX, cellsList[collisionCell].positionY, cellsList[collisionCell].visited, cellsList[collisionCell].parentCellId,  cellsList[collisionCell].backParent});
    if(this->pathFound)
    {
        while(!endReached)
        {
            if(route.back().visited == true)
            {
                this->route.push_back({cellsList[route.back().parentCellId].positionX, cellsList[route.back().parentCellId].positionY, cellsList[route.back().parentCellId].visited, cellsList[route.back().parentCellId].parentCellId, cellsList[route.front().backParent].backParent});
                if(this->route.back().parentCellId == startCell)
                {
                    endReached = true;
                }
            }
            else
            {
                endReached = true;
            }
        }
        endReached = false;
        while(!endReached)
        {
            if(route.front().visited == true)
            {
                this->route.push_front({cellsList[route.front().backParent].positionX, cellsList[route.front().backParent].positionY, cellsList[route.front().backParent].visitedBack, cellsList[route.front().parentCellId].parentCellId, cellsList[route.front().backParent].backParent});
                if(this->route.front().parentCellId == endCell)
                {
                    endReached = true;
                }
            }
            else
            {
                endReached = true;
            }
        }
    }
    this->route.pop_front();
}

void actors::cleanUp()
{
    for(int i =0; i < MAP_WIDTH; i++)
    {
        for(int j = 0; j < MAP_HEIGHT; j++)
        {
            if(currentGame.occupiedByActorList[i][j] == this->actorId)
            {
                if(i !=  this->actorCords[0] && j != this->actorCords[1])
                {
                    currentGame.occupiedByActorList[i][j] = -1;
                }
            }
        }
    }
    currentGame.occupiedByActorList[this->actorCords[0]][this->actorCords[1]] = this->actorId;
}

std::pair<int, int> actors::getHealth()
{
    return {this->actorHealth, this->hitPoints};
}

int actors::getMeleeDMG()
{
    return this->meleeDamage;
}

int actors::getRangedDMG()
{
    return this->rangedDamage;
}

void actors:: drawActor()
{
    int i = this->actorCords[0];
    int j = this->actorCords[1];
    int x = this->actorGoal[0];
    int y = this->actorGoal[1];
    int xPosition = worldSpace(i,j,true);
    int yPosition = worldSpace(i,j,false);
    int spriteXoffset = 0;
    int spriteYoffset = 0;

    if(this->busyWalking || this->timeStartedWalkingToRecource > 0.0f)
    {
        if(currentGame.elapsedTime-this->timeLastOffsetChange > 0.2f)
        {
            this->spriteYOffset = this->spriteYOffset + 32;
            if(this->spriteYOffset > 128)
            {
                this->spriteYOffset = 32;
            }
            this->timeLastOffsetChange = currentGame.elapsedTime;
        }
        if (this->busyWalking)
        {
            int nXPosition = worldSpace(x, y, true);
            int nYPosition = worldSpace(x, y, false);
            int deltaX = xPosition - nXPosition;
            int deltaY = yPosition - nYPosition;
            float deltaTime = currentGame.elapsedTime - this->timeLastUpdate;
            float deltaXCompleted = deltaX * deltaTime;
            float deltaYCompleted = deltaY * deltaTime;
            xPosition = xPosition - deltaXCompleted;
            yPosition = yPosition - deltaYCompleted;
        }
    }
    else if(this->isAtRecource)
    {
        if(currentGame.elapsedTime-this->timeLastOffsetChange > 0.2f)
        {
            this->spriteYOffset = this->spriteYOffset + 32;
            if(this->spriteYOffset > 128)
            {
                this->spriteYOffset = 0;
            }
            this->timeLastOffsetChange = currentGame.elapsedTime;
        }
    }
    else
    {
        this->spriteYOffset = 0;
    }
    switch(this->spriteYOffset)
    {
    case 0:
        spriteYoffset = 0;
        break;
    case 32:
        spriteYoffset = 32;
        break;
    case 64:
        spriteYoffset = 64;
        break;
    case 96:
        spriteYoffset = 96;
        break;
    case 128:
        spriteYoffset = 128;
        break;
    }
    int spriteOffset = 0;
    if(this->isAtRecource)
    {
        if(this->ResourceBeingGatherd == 0)
        {
            spriteOffset = 128;
        }
        else if(this->ResourceBeingGatherd == 2 || this->ResourceBeingGatherd == 3)
        {
            spriteOffset = 256;
        }
        else if(this->ResourceBeingGatherd == 1)
        {
            spriteOffset = 384;
        }
    }
    switch(this->orientation)
    {
    case 0:
        spriteXoffset = 80 + spriteOffset;
        break;
    case 1:
        spriteXoffset = 0 + spriteOffset;
        break;
    case 2:
        spriteXoffset = 96 + spriteOffset;
        break;
    case 3:
        spriteXoffset = 48 + spriteOffset;
        break;
    case 4:
        spriteXoffset = 64 + spriteOffset;
        break;
    case 5:
        spriteXoffset = 32 + spriteOffset;
        break;
    case 6:
        spriteXoffset = 112 + spriteOffset;
        break;
    case 7:
        spriteXoffset = 16 + spriteOffset;
        break;
    default:
        spriteXoffset = 0 + spriteOffset;
        break;
    }
    xPosition = xPosition + this->offSetX;
    yPosition = yPosition + this->offSetY;

    if(currentGame.isInSelectedActors(this->actorId))
    {
        currentGame.spriteUnitSelectedTile.setPosition(xPosition, yPosition);
        window.draw(currentGame.spriteUnitSelectedTile);
    }

    switch (this->actorType) 
    {
    case 0:
        currentGame.spriteVillager.setPosition(xPosition, yPosition);
        currentGame.spriteVillager.setTextureRect(sf::IntRect(spriteXoffset, spriteYoffset, 16, 32));
        window.draw(currentGame.spriteVillager);
        break;
    case 1:
        currentGame.spriteSwordsman.setPosition(xPosition, yPosition);
        currentGame.spriteSwordsman.setTextureRect(sf::IntRect(spriteXoffset, spriteYoffset, 16, 32));
        window.draw(currentGame.spriteSwordsman);
    }


}

void actors::buildBuilding()
{
    if(currentGame.occupiedByBuildingList[this->actionPreformedOnTile[0]][this->actionPreformedOnTile[1]] != -1)
    {
        if(!listOfBuildings[currentGame.occupiedByBuildingList[this->actionPreformedOnTile[0]][this->actionPreformedOnTile[1]]].getCompleted())
        {
            if(currentGame.elapsedTime - this->timeStartedGatheringRecource > 1)
            {
                listOfBuildings[currentGame.occupiedByBuildingList[this->actionPreformedOnTile[0]][this->actionPreformedOnTile[1]]].addBuildingPoint();
                this->timeStartedGatheringRecource = currentGame.elapsedTime;
            }
        }
        else
        {
            if(!this->isBackAtOwnSquare)
            {
                this->walkBackToOwnSquare();
            }
            listOfBuildings[this->buildingId].removeActorFromBuildingTile(this->actorId);
        }
    }
    else
    {
        //het gebouw is er niet meer
        if(!this->isBackAtOwnSquare)
        {
            this->walkBackToOwnSquare();
        }
        listOfBuildings[this->buildingId].removeActorFromBuildingTile(this->actorId);
    }
}

int actors::getActorId()
{
    return this->actorId;
}

void actors::setIsBuildingTrue(int buildingId)
{
    this->isBuilding = true;
    this->buildingId = buildingId;
    this->isGatheringRecources = false;
    this->ResourceBeingGatherd = 1;
    this->actionPreformedOnTile[0] = this->actorGoal[0];
    this->actionPreformedOnTile[1] = this->actorGoal[1];
}

void actors::renderPath()
{
    std::list<routeCell>::iterator it;
    for(it = route.begin(); it!=route.end(); it++)
    {
        currentGame.spriteSelectedTileForPath.setPosition(worldSpace(it->positionX, it->positionY, true), worldSpace(it->positionX, it->positionY, false));
        window.draw(currentGame.spriteSelectedTileForPath);
    }
}
