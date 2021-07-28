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
std::vector<int> listOfActorsWhoNeedAPath;
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
    this->timeLastRetry = 0;
    this->isIdle = true;
    hasStartedSearchingForAlternatives = false;
    switch(type)
    {
    case 0://villager
        this->actorHealth = 25;
        this->hitPoints = 25;
        this->meleeDamage = 3;
        this->range = 3;
        this->rangedDamage = 1;
        this->timeBetweenShots = 2.0f;
        this->splashDamage = 0;
        this->projectileType = 0;
        this->doesRangedDamage = false;
        this->rateOfFire = 2;
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
        this->rateOfFire = 3;
        break;
    }
    this->timeLastRetry = 0.0f;
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
    if (this->actorAlive)
    {
        if (!this->isFindingAlternative) {
            if (this->goalNeedsUpdate)
            {
                this->updateGoalPath();
            }
            else if (!this->routeNeedsPath)
            {
                this->moveActorIfWalking();
                this->doTaskIfNotWalking();
            }
            else {
                bool actorInListForPathfinding = false;
                if (!listOfActorsWhoNeedAPath.empty()) {
                    for (int i = 0; i < listOfActorsWhoNeedAPath.size(); i++) {
                        if (listOfActorsWhoNeedAPath[i] == this->actorId) {
                            actorInListForPathfinding = true;
                        }
                    }
                }
                if (actorInListForPathfinding) {
                }
                else {
                    this->retryWalkingOrChangeGoal();
                }
            }
            this->houseKeeping();
        }
        else  {
            this->searchAltetnative();
        }
    }
}
void actors::searchAltetnative() {
    if (!this->routeNeedsPath){
        if (!this->pathFound) {
            if (this->isBuilding) {
                this->getNewBuildingTileForSameBuilding();
            }
            else if (this->hasToUnloadResource) {
                this->findNearestDropOffPoint();
            }
            else if (this->isGatheringRecources) {
                this->findNearestSimilairResource();
            }
        }
        else {
            this->isFindingAlternative = false;
            if (this->hasToUnloadResource) {
                this->isWalkingToUnloadingPoint = true;
                this->dropOffTile = listOfDropOffLocations.front();
            }
            else if (this->isGatheringRecources) {
                this->actionPreformedOnTile[0] = this->listOfResourceLocations.front().locationX;
                this->actionPreformedOnTile[1] = this->listOfResourceLocations.front().locationY;
            }
            this->listOfDropOffLocations.clear();
            this->listOfResourceLocations.clear();
        }
    }
}

std::string actors::getRecources() {
    std::stringstream output;
    output << "Wood: " << this->amountOfWood << " Food: " << this->amountOfFood << " Stone: " << this->amountOfStone << " Gold: " << this->amountOfGold;
    return output.str();
}

void actors::getNewBuildingTileForSameBuilding() {
    nearestBuildingTile tempTile = findNearestBuildingTile(this->buildingId, this->actorId);
    if (tempTile.isSet)
    {
        this->updateGoal(tempTile.locationX, tempTile.locationY, 0); //this also releases the old building tile, but does not reset the 'buildingId', so we can still use it! But we have to claim new building tile...
        listOfBuildings[this->buildingId].claimFreeBuiildingTile(tempTile.buildingId, this->actorId);
        this->setIsBuildingTrue(this->buildingId, tempTile.actionLocationX, tempTile.actionLocationY);
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
        else if (this->isGatheringRecources && (!this->routeNeedsPath) && this->route.empty())
        {
            this->handleResourceGathering();
        }
        else if (this->isBuilding && (!this->routeNeedsPath) && this->route.empty())
        {
            this->handleBuilding();
        }
        else {
            this->isIdle = true;
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

bool actors::idle()
{
    return this->isIdle;
}


bool actors::isAlive()
{
    return this->actorAlive;
}

cords actors::getActorCords()
{
    cords blah;
    blah.x = this->actorCords[0];
    blah.y = this->actorCords[1];
    return blah;
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
    if (currentGame.elapsedTime - this->timeStartedGatheringRecource > this->rateOfFire) 
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
        if(this->buildingId != -1)
        {
            listOfBuildings[this->buildingId].removeActorFromBuildingTile(this->actorId);
        }
        this->actorGoal[0] = i;
        this->actorGoal[1] = j;
        this->actorCommandGoal[0] = i;
        this->actorCommandGoal[1] = j;
        this->waitForAmountOfFrames = 0;// waitTime;
        this->goalNeedsUpdate = true;
        this->busyWalking = false;
        this->pathFound = false;
        this->realPath = false;
        this->isAtRecource = false;
        this->isBackAtOwnSquare = true;
        this->isRangedAttacking = false;
        this->isMeleeAttacking = false;
        this->offSetX = 0.0f;
        this->offSetY = 0.0f;
        this->timeStartedGatheringRecource = 0.0f;
        this->isFindingAlternative = false;
        this->isIdle = false;
    }
}

int actorOrientation(int& Xc, int& Yc, int& Xn, int& Yn)
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
    if (this->timeLastRetry + 0.5 < currentGame.getTime()) {
        this->timeLastRetry = currentGame.getTime();
        if (this->retries < 30)
        {
                this->actorGoal[0] = this->actorCommandGoal[0];
                this->actorGoal[1] = this->actorCommandGoal[1];
                this->routeNeedsPath = true;
                this->pathFound = false;
                this->realPath = false;
                listOfActorsWhoNeedAPath.push_back(this->actorId);
                this->retries += +1;
        }
        else if (this->hasToUnloadResource || this->isGatheringRecources || this->isBuilding)
        {
            this->routeNeedsPath = false;
            this->pathFound = false;
            this->realPath = false;
            this->isFindingAlternative = true;
        }
        else
        {
            this->clearRoute();
            this->pathFound = false;
            this->realPath = false;
            this->routeNeedsPath = false;
            this->isIdle = true;
        }
    }
}

void actors::walkToNextSquare() {
    // Deze actor heeft een doel, dit doel is nog niet bereikt en is klaar met het vorige stuk lopen!
    if (actorOrientation(this->actorCords[0], this->actorCords[1], this->route.back().positionX, this->route.back().positionY) == this->orientation)
    {
        if ((this->isGatheringRecources || this->isMeleeAttacking) && this->route.size() == 1)
        {
            //if (!this->isWalkingToUnloadingPoint) {
                this->clearRoute();
            //}
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
                this->routeNeedsPath = false;
                this->pathFound = false;
                this->realPath = false;
                this->reachedUnloadingPoint = false;
                this->isFindingAlternative = true;
            }
            else if (this->reachedUnloadingPoint)
            {
                this->unloadAndReturnToGathering();
            }
        }
    }
    else
    {
        if (this->realPath) {
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
}

void actors::handleBuilding()
{
    if (this->realPath) {
        if (this->orientation == actorOrientation(this->actorCords[0], this->actorCords[1], this->actionPreformedOnTile[0], this->actionPreformedOnTile[1])) {
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
        else {
            //eerst de actor in de juiste orientatie zetten
            this->orientation = newOrientation(this->orientation, actorOrientation(this->actorCords[0], this->actorCords[1], this->actionPreformedOnTile[0], this->actionPreformedOnTile[1]));
        }
    }
    else {
        this->getNewBuildingTileForSameBuilding();
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

bool isRealyNextToRecource(int& unitX, int& unitY, int& resourceX, int& resourceY) {
    int distX = unitX - resourceX;
    int distY = unitY - resourceY;
    if ((distX == 0 || distX == 1 || distX == -1) && (distY == 0 || distY == 1 || distY == -1)) {
        return true;
    }
    else {
        return false;
    }
}

void actors::gatherResource()
{
    if (isRealyNextToRecource(this->actorCords[0], this->actorCords[1], this->actionPreformedOnTile[0], this->actionPreformedOnTile[1])) {
        if (currentGame.objectLocationList[this->actionPreformedOnTile[0]][this->actionPreformedOnTile[1]] != -1)
        {
            if (currentGame.elapsedTime - this->timeStartedGatheringRecource > 2)
            {
                switch (this->ResourceBeingGatherd)
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
                if ((this->amountOfFood == 10) || (this->amountOfWood == 10) || (this->amountOfStone == 10) || (this->amountOfGold == 10))
                {
                    this->hasToUnloadResource = true;
                    this->isAtRecource = false;
                    this->realPath = false;
                }
                this->timeStartedGatheringRecource = currentGame.elapsedTime;
            }
        }
        else
        {
            //resource not here!
            this->hasToUnloadResource = true;
            this->isAtRecource = false;
            this->realPath = false;
        }
    }
    else {
        this->carriesRecources = true;
        this->hasToUnloadResource = true;
        this->isAtRecource = false;
        this->realPath = false;
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
        this->routeNeedsPath = false;
        this->pathFound = false;
        this->actionPreformedOnTile[0] = this->actorGoal[0];
        this->actionPreformedOnTile[1] = this->actorGoal[1];
        this->isWalkingToUnloadingPoint = false;
        this->isAtCarryCapacity = false;
        this->carriesRecources = false;
        this->isAtRecource = false;
        this->hasToUnloadResource = false;
        this->timeStartedWalkingToRecource = 0.0f;
        this->isFindingAlternative = true;
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

void actors::findNearestSimilairResource()
{
    if (this->listOfResourceLocations.empty()) {
        if (this->hasStartedSearchingForAlternatives == false) {
            this->hasStartedSearchingForAlternatives = true;

            for (int i = 0; i < listOfObjects.size(); i++) {
                if (listOfObjects[i].getTypeOfResource() == this->ResourceBeingGatherd) {
                    float tempDeltaDistance = distEuclidean(this->actorCords[0], this->actorCords[1], listOfObjects[i].getLocation().x, listOfObjects[i].getLocation().y);
                    listOfResourceLocations.push_back({ tempDeltaDistance, listOfObjects[i].getLocation().x, listOfObjects[i].getLocation().y, i, true });
                }
            }
            /*
            int lowSearchLimitX = this->actorCords[0] - 30;
            if (lowSearchLimitX < 0)
            {
                lowSearchLimitX = 0;
            }
            int lowSearchLimitY = this->actorCords[1] - 30;
            if (lowSearchLimitY < 0)
            {
                lowSearchLimitY = 0;
            }
            int highSearchLimitX = this->actorCords[0] + 30;
            if (highSearchLimitX > MAP_WIDTH)
            {
                highSearchLimitX = MAP_WIDTH;
            }
            int highSearchLimitY = this->actorCords[1] + 30;
            if (highSearchLimitY > MAP_HEIGHT)
            {
                highSearchLimitY = MAP_HEIGHT;
            }
            for (int i = lowSearchLimitX; i < highSearchLimitX; i++)
            {
                for (int j = lowSearchLimitY; j < highSearchLimitY; j++)
                {
                    if (currentGame.objectLocationList[i][j] != -1)
                    {
                        if (listOfObjects[currentGame.objectLocationList[i][j]].getTypeOfResource() == this->ResourceBeingGatherd)
                        {
                            float tempDeltaDistance = dist(this->actorCords[0], this->actorCords[1], i, j);
                            this->listOfResourceLocations.push_back({ tempDeltaDistance, i, j, currentGame.objectLocationList[i][j], true });
                        }
                    }
                }
            }
            */
        }
        else {
            //make villager idle, because search was ineffective
            this->clearRoute();
            this->pathFound = false;
            this->realPath = false;
            this->routeNeedsPath = false;
            this->isIdle = true;
            this->isGatheringRecources = false;
            this->isBuilding = false;
            return;
        }
        if (!this->listOfResourceLocations.empty())
        {
            this->listOfResourceLocations.sort([](const nearestBuildingTile& f, const nearestBuildingTile& s)
                {
                    return f.deltaDistance < s.deltaDistance;
                });
        }
        this->pathFound = false;
    }
    else {
        this->listOfResourceLocations.pop_front();
    }

    if(!this->pathFound && !this->listOfResourceLocations.empty())
    {
        this->actorGoal[0] = this->listOfResourceLocations.front().locationX;
        this->actorGoal[1] = this->listOfResourceLocations.front().locationY;
        this->actorCommandGoal[0] = this->listOfResourceLocations.front().locationX;
        this->actorCommandGoal[1] = this->listOfResourceLocations.front().locationY;
        this->actionPreformedOnTile[0] = this->actorGoal[0];
        this->actionPreformedOnTile[1] = this->actorGoal[1];
        this->waitForAmountOfFrames = 0;
        this->routeNeedsPath = true;
        listOfActorsWhoNeedAPath.push_back(this->actorId);
    }
}

cords actors::getLocation(){
    return {this->actorCords[0], this->actorCords[1]};
}

int adjacentTileIsCorrectDropOffPoint(int& x, int& y, int& resourceGatherd, int& team) {
    for (int xOffset = x - 1; xOffset < x + 2; xOffset++) 
    {
        for (int yOffset = y - 1; yOffset < y + 2; yOffset++) 
        {
            if (currentGame.occupiedByBuildingList[xOffset][yOffset] != -1) 
            {
                int i = currentGame.occupiedByBuildingList[xOffset][yOffset];
                if ((listOfBuildings[i].getRecievesWhichResources() == resourceGatherd || listOfBuildings[i].getRecievesWhichResources() == 4) && listOfBuildings[i].getTeam() == team && listOfBuildings[i].getCompleted()) 
                {
                    return i;
                }
            }
        }
    }
    return -1;
}

void actors::findNearestDropOffPoint()
{
    if (!this->routeNeedsPath) {
        //Bugfix for edge case where the actor is ocuping a tile adjacent to a dropoff building
        int adjacentBuildingAvailableId = adjacentTileIsCorrectDropOffPoint(this->actorCords[0], this->actorCords[1], this->ResourceBeingGatherd, this->actorTeam);
        if (adjacentBuildingAvailableId != -1) {
            listOfDropOffLocations.push_back({ 0, this->actorCords[0], this->actorCords[1], adjacentBuildingAvailableId, true, 1 });
            this->actorGoal[0] = this->actorCords[0];
            this->actorGoal[1] = this->actorCords[1];
            this->actorCommandGoal[0] = this->actorCords[0];
            this->actorCommandGoal[1] = this->actorCords[1];
            this->waitForAmountOfFrames = 0;
            this->routeNeedsPath = true;
            listOfActorsWhoNeedAPath.push_back(this->actorId);
        }
        else {
            if (listOfDropOffLocations.empty())
            {
                for (int i = 0; i < listOfBuildings.size(); i++)
                {
                    if ((listOfBuildings[i].getRecievesWhichResources() == this->ResourceBeingGatherd || listOfBuildings[i].getRecievesWhichResources() == 4) && listOfBuildings[i].getTeam() == this->actorTeam && listOfBuildings[i].getCompleted())
                    {
                        std::vector<adjacentTile> tileList = listOfBuildings[i].getDropOffTiles();
                        for (int j = 1; j < tileList.size(); j++)
                        {
                            float tempDeltaDistance = dist(this->actorCords[0], this->actorCords[1], tileList[j].goalX, tileList[j].goalY);
                            listOfDropOffLocations.push_back({ tempDeltaDistance, tileList[j].tileX, tileList[j].tileY, tileList[j].goalX, tileList[j].goalY, i, true, tileList[j].tileId });
                        }
                    }
                }
                if (!listOfDropOffLocations.empty())
                {
                    listOfDropOffLocations.sort([](const nearestBuildingTile& f, const nearestBuildingTile& s)
                        {
                            return f.deltaDistance < s.deltaDistance;
                        });
                }
                this->pathFound = false;
            }
            if (!this->realPath && !listOfDropOffLocations.empty())
            {
                this->updateGoal(listOfDropOffLocations.front().actionLocationX, listOfDropOffLocations.front().actionLocationY, 0);
                listOfBuildings[this->buildingId].removeActorFromBuildingTile(this->actorId);
                this->buildingId = listOfDropOffLocations.front().buildingId;
                listOfBuildings[this->buildingId].claimFreeBuiildingTile(listOfDropOffLocations.front().tileId, this->actorId);
                listOfDropOffLocations.pop_front();
            }
        }
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
        this->route.clear();
        this->actorRealGoal[0] = this->actorGoal[0];
        this->actorRealGoal[1] = this->actorGoal[1];
        this->realPath = false;
        this->routeNeedsPath = false;
        this->pathAStar();
        if (!this->realPath && this->pathFound) {
            this->timeLastPathTry = currentGame.elapsedTime;
        }
    }
    else {
        std::cout << "Route did not need a path?" << std::endl;
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
    std::list<Cells*> checkedList;
    bool endReached = false;


    //check of de doelcel niet 1 hokje weg is 
    if(((actorCords[0]-actorGoal[0] == 0) ||(actorCords[0]-actorGoal[0] == -1) ||(actorCords[0]-actorGoal[0] == 1)) && ((actorCords[1]-actorGoal[1] == 0) ||(actorCords[1]-actorGoal[1] == -1) ||(actorCords[1]-actorGoal[1] == 1)))
    {
        //en geen obstakel is of het hokje van de actor zelf is
        if(!cellsList[endCell].obstacle || (actorCords[0] == actorGoal[0] && actorCords[1] == actorGoal[1]))
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
        //De startcell initialiseren
        this->pathFound = false;
        cellsList[startCell].addNeighbours(cellsList);
        cellsList[startCell].totalCostGuess = dist(cellsList[startCell].positionX, cellsList[startCell].positionY, cellsList[endCell].positionX, cellsList[endCell].positionY);
        cellsList[startCell].visited = true;
        cellsList[startCell].parentCellId =  -1;
        cellsList[startCell].costToGoal = dist(cellsList[startCell].positionX, cellsList[startCell].positionY, cellsList[endCell].positionX, cellsList[endCell].positionY);
        //De startcel in de lijst van te checken cellen zetten om te kunnen beginnen
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
        else if(!listToCheck.empty())
        {
            //Loop door de lijst van "buurcellen van de te checken cell"
            for (std::vector<int>::const_iterator iterator =  (*listToCheck.front()).neighbours.begin(), end =  (*listToCheck.front()).neighbours.end(); iterator != end; ++iterator)
            {
                //Kijk of deze cell eerder bezocht is
                if(!cellsList[*iterator].visited)
                {
                    //Deze cell heeft geen parent is is dus nooit eerder gevonden! De buren moeten dus toegevoegd worden!
                    cellsList[*iterator].addNeighbours(cellsList);
                    //De cell waarvan we de neighbours onderzoeken is uiteraard tot nu toe de kortste route hiernaartoe
                    cellsList[*iterator].parentCellId = (*listToCheck.front()).cellId;
                    //Nu moeten de kosten voor de route hiernatoe uitgerekend worden (Dit zijn de kosten van naar de parent gaan +1)
                    cellsList[*iterator].cummulativeCost = (*listToCheck.front()).cummulativeCost+1;
                    //Als laatste zetten we de cell in de lijst met cellen die gecheckt moet worden
                    listToCheck.push_back(&cellsList[*iterator]);
                    //En we schatten vanaf deze cell de kosten naar het doel
                    cellsList[*iterator].costToGoal = dist(cellsList[*iterator].positionX,cellsList[*iterator].positionY,cellsList[endCell].positionX,cellsList[endCell].positionY);
                    //Waardoor we dus de totale kosten kunnen schatten
                    cellsList[*iterator].totalCostGuess = cellsList[*iterator].costToGoal + cellsList[*iterator].cummulativeCost;
                    //We zijn voor nu klaar met deze tegel en laten een vlag achter dat de tegel al eens bekeken is
                    cellsList[*iterator].visited = true;
                }
                else
                {
                    //Deze tegel is al eerder gevonden, de buren staan dus al in de te checken cell lijst en hoeft niet opnieuw gechecked te worden
                    //We moeten wel weten of de route waarmee deze tegel nu is aangedaan niet korter is dan een eerder gevonden route
                    if((*listToCheck.front()).cummulativeCost+1 < cellsList[*iterator].cummulativeCost)
                    {
                        //Er is een kortere route naar deze cell! Pas de parent cell dus aan en geef een nieuwe cummulative Cost;
                        cellsList[*iterator].parentCellId = (*listToCheck.front()).cellId;
                        cellsList[*iterator].cummulativeCost = (*listToCheck.front()).cummulativeCost+1;
                        //Uiteraard verranderd dit dan ook de geschatte totale kosten vanaf deze tegel
                        cellsList[*iterator].totalCostGuess = cellsList[*iterator].costToGoal + cellsList[*iterator].cummulativeCost;
                    }
                }
            }
            //zet de tegel op de bezochte tegellijst
            checkedList.push_back(&cellsList[(*listToCheck.front()).cellId]);
            //en haal hem van de te bezoeken tegellijst
            listToCheck.pop_front();
        }
    }
    //Alle tegels die te bereiken zijn zijn bekeken of er is een route gevonden.
    //Maak een route door de parent cells achter elkaar te zetten tot de begin cell
    this->route.clear();
    if(this->pathFound)
    {
        //Er is een pad naar het doel gevonden! Stippel de route maar uit!
        routing(cellsList, endCell, startCell, endReached);
        this->realPath = true; //Vlaggetje dat de route naar het eindpunt gaat en er in princiepe geen herbereking nodig is
    }
    else {
        //Check of de actor niet naar een opdracht moet lopen. Is dat het geval wil ik niet naar de dichtsbijzijnde tegel lopen maar de actor een nieuw doel laten kiezen
        if (!(this->isGatheringRecources || this->isBuilding)) {
            //We kunnen niet bij het doel komen en hebben geen bouw of verzamel opdracht, dan gaan we maar naar de geschatte dichtsbijzijnde cell waar we naartoe kunnen!
            if (!checkedList.empty()) {
                checkedList.sort([](const Cells* f, const Cells* s)
                    {
                        return f->costToGoal < s->costToGoal;
                    });
                //Een bereikbare tegel met de laagst geschatte totale kosten staat nu vooraan in de rij van de bezochte tegels
                this->actorGoal[0] = cellsList[(*checkedList.front()).cellId].positionX;
                this->actorGoal[1] = cellsList[(*checkedList.front()).cellId].positionY;
                if ((*checkedList.front()).parentCellId != -1) {
                    //Actor staat niet op de dichtsbijzijnde tegel: Stippel de route hiernaartoe uit
                    routing(cellsList, (*checkedList.front()).cellId, startCell, endReached);
                }
                else {
                    //Actor staat al op de dichtsbijzijnde tegel er is dus geen route nodig
                }
                this->pathFound = true; //Er is geen pad naar het doel maar wel een pad naar de dichtsbijzijnde plek, de actor moet dus wel gaan lopen
                this->realPath = false; //Vlaggetje op false laten staan zodat op een later tijdstip er opnieuw geprobeerd kan worden of er nu wél een pad naar het doel is
            }
        }
        else {
            this->isFindingAlternative = true;
        }
    }
}

void actors::routing(std::vector<Cells>& cellsList, int& endCell, int& startCell, bool& endReached)
{
    //Zet de tegel waarnaartoe gelopen wordt in de lijst
    this->route.push_back({ cellsList[endCell].positionX, cellsList[endCell].positionY, cellsList[endCell].visited, cellsList[endCell].parentCellId });
    while (!endReached)
    {
        if (route.back().visited == true)
        {
            //Voeg voor iedere tegel de parent toe aan de lijst (dit was de tegel met de korste weg naar deze tegel)
            this->route.push_back({ cellsList[route.back().parentCellId].positionX, cellsList[route.back().parentCellId].positionY, cellsList[route.back().parentCellId].visited, cellsList[route.back().parentCellId].parentCellId });
            if (this->route.back().parentCellId == startCell) {
                    //Als de nu volgende parentCell de tegel is waar de actor op staat zijn we klaar.
                    endReached = true;
            }
            if (this->route.back().parentCellId == -1) {
                endReached = true;
            }
            //Fix voor een bug waarin op een of andere manier géén parent cell ingevult staat en dus nul zou zijn. (al zou dat niet moeten kunnen)
            if (this->route.back().parentCellId == 0) {
                //We gaan kijken of het een logische buur kan zijn, gelukkig hoeven we maar drie posities te controleren:
                if (this->route.back().positionX == 0 && this->route.back().positionX == 1) {
                    //Logische parent; doe niets!
                }
                else if(this->route.back().positionX == 0 && this->route.back().positionX == 1) {
                    //Logische parent; doe niets!
                }
                else if (this->route.back().positionX == 1 && this->route.back().positionX == 1) {
                    //Logische parent; doe niets!
                }
                else {
                    //Onlogisch! breek de route af zonder de parent cell toe te voegen
                    endReached = true;
                }
            }
        }
        else
        {
            //Blijkbaar is deze tegel nooit geevalueerd of het is de starttegel; onlogisch! breek het route maken af!
            endReached = true;
        }
    }
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

    bool drawHealth = false;

    if(currentGame.isInSelectedActors(this->actorId))
    {
        currentGame.spriteUnitSelectedTile.setPosition(xPosition, yPosition);
        window.draw(currentGame.spriteUnitSelectedTile);
        drawHealth = true;
    }

    if (this->actorHealth < this->hitPoints) {
        drawHealth = true;
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

    if (drawHealth) {
        currentGame.healthBarBackground.setFillColor(sf::Color(255, 0, 0));
        currentGame.healthBarBackground.setSize(sf::Vector2f(32, 3));
        currentGame.healthBarGreenBar.setFillColor(sf::Color(0, 255, 0));
        currentGame.healthBarGreenBar.setSize(sf::Vector2f(((float)this->actorHealth / (float)this->hitPoints) * 32, 3));
        currentGame.healthBarBackground.setPosition(xPosition+16, yPosition-28);
        currentGame.healthBarGreenBar.setPosition(xPosition+16, yPosition-28);
        window.draw(currentGame.healthBarBackground);
        window.draw(currentGame.healthBarGreenBar);

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
            this->walkBackToOwnSquare();
            listOfBuildings[this->buildingId].removeActorFromBuildingTile(this->actorId);
        }
    }
    else
    {
        //het gebouw is er niet meer
        this->walkBackToOwnSquare();
        listOfBuildings[this->buildingId].removeActorFromBuildingTile(this->actorId);
    }
}

int actors::getActorId()
{
    return this->actorId;
}

bool actors::isGathering()
{
    return this->isGatheringRecources;
}

bool actors::getIsBuilding()
{
    return this->isBuilding;
}

int actors::getBuildingId()
{
    return this->buildingId;
}

int actors::getResourceGathered()
{
    return this->ResourceBeingGatherd;
}

void actors::setIsBuildingTrue(int buildingId, int& goalX, int& goalY)
{
    this->isBuilding = true;
    this->buildingId = buildingId;
    this->isGatheringRecources = false;
    this->ResourceBeingGatherd = 1;
    this->actionPreformedOnTile[0] = goalX;
    this->actionPreformedOnTile[1] = goalY;
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
