//#include <future>
#include <iostream>
#include <string>
#include "actors.h"
#include "buildings.h"
#include "gametext.h"
#include "globalfunctions.h"
#include "objects.h"
#include "player.h"
#include "projectile.h"
#include "simpleAI.h"

std::vector<actorOrBuildingPrice> priceOfActor;
std::vector<int> listOfActorsWhoNeedAPath;
std::vector <actors> listOfActors;
std::mutex listOfActorsMutex;

namespace
{
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
        int diffX = Xn - Xc;
        int diffY = Yn - Yc;

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

    int newOrientation(int oldOrientation, int desiredOrientation)
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

    int adjacentTileIsCorrectDropOffPoint(cords start, resourceTypes resourceGatherd, int team) {
        for (int xOffset = start.x - 1; xOffset < start.x + 2; xOffset++)
        {
            for (int yOffset = start.y - 1; yOffset < start.y + 2; yOffset++)
            {
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
        return -1;
    }

    bool isReallyNextToResource(int unitX, int unitY, int resourceX, int resourceY) {
        int distX = unitX - resourceX;
        int distY = unitY - resourceY;
        if ((distX == 0 || distX == 1 || distX == -1) && (distY == 0 || distY == 1 || distY == -1)) {
            return true;
        }
        else {
            return false;
        }
    }
}

void updateCells(int goalId, int startId, std::vector<Cells>& cellsList)
{
    int n = 0;
    for (int i = 0; i < MAP_WIDTH; i++)
    {
        for (int j = 0; j < MAP_HEIGHT; j++)
        {
            cellsList.push_back(Cells());
            cellsList[n].position.x = i;
            cellsList[n].position.y = j;
            if (n == goalId || n == startId)
            {
                cellsList[n].obstacle = false;
            }
            else if (!currentGame.isPassable({ i, j }))
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

void Cells::addNeighbours(const std::vector<Cells> &cellsList)
{
    if(this->position.x > 0)
    {
        //de cell links kan toegevoegd worden
        if(!cellsList[this->cellId-MAP_HEIGHT].obstacle)
        {
            this->neighbours.push_back(this->cellId-MAP_HEIGHT);
        }
    }
    if(this->position.x < MAP_WIDTH-1)
    {
        //de cell rechts kan toegevoegd worden
        if(!cellsList[this->cellId+MAP_HEIGHT].obstacle)
        {
            this->neighbours.push_back(this->cellId+MAP_HEIGHT);
        }
    }
    if(this->position.y > 0)
    {
        //de cell erboven kan toegevogd worden
        if(!cellsList[this->cellId-1].obstacle)
        {
            this->neighbours.push_back(this->cellId-1);
        }
    }
    if(this->position.y != MAP_HEIGHT-1)
    {
        //de cell eronder kan toegevoegd worden
        if(!cellsList[this->cellId+1].obstacle)
        {
            this->neighbours.push_back(this->cellId+1);
        }
    }
    //schuin gaan...
    if(this->position.y != MAP_HEIGHT-1 && this->position.x < MAP_WIDTH-1)
    {
        //de cell rechtsonder kan toegevoegd worden
        if(!cellsList[this->cellId+1+MAP_HEIGHT].obstacle )
        {
            if(!(cellsList[this->cellId+1].obstacle && cellsList[this->cellId+MAP_HEIGHT].obstacle))
            {
                this->neighbours.push_back(this->cellId+1+MAP_HEIGHT);
            }
        }
    }
    if(this->position.y >0 && this->position.x < MAP_WIDTH-1)
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
    if(this->position.y != MAP_HEIGHT-1 && this->position.x > 0)
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
    if(this->position.y >0 && this->position.x > 0)
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

actors::actors(int type, cords location, int actorTeam, int actorId)
{
    currentGame.occupiedByActorList[location.x][location.y] = actorId;
    listOfPlayers[actorTeam].addToCurrentPopulation(1);
    this->actorType = type;
    this->actorTeam = actorTeam;
    this->actorId = actorId;
    this->actorCords = location;
    this->timeLastRetry = 0;
    this->isIdle = true;
    this->actorCommandGoal = { 0,0 };
    this->actorRealGoal = { 0, 0 };
    this->buildingId = 0;
    this->commonGoal = false;
    this->currentFrame = 0;
    this->idOfTarget = 0;
    this->isFindingAlternative = false;
    this->isRangedAttacking = false;
    this->movedMoreThanHalf = false;
    this->noPathPossible = false;
    this->realPath = false;
    this->timeLastPathTry = currentGame.getTime();
    this->timeLastUpdate = currentGame.getTime();
    this->timeStartedGatheringRecource = 0.0f;
    this->waitForAmountOfFrames = 0;
    this->reachedUnloadingPoint = false;
    this->hasStartedSearchingForAlternatives = false;
    this->timeLastRetry = 0.0f;
    this->actorAlive = true;
    this->actorGoal = location;
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
    this->hasMoved = false;
    this->initialized = true;
    this->ResourceBeingGatherd = resourceTypes::resourceFood;
    this->actorAlive = true;
    this->carriesRecources = false;
    this->dropOffTile = { 0,0,0,0,false };
    this->actionPreformedOnTile = { 0, 0 };
    this->hasUnloaded = false;
    this->isAtCarryCapacity = false;
    this->isWalkingToUnloadingPoint = false;
    this->isMeleeAttacking = false;


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
    default:
        this->actorHealth = 0;
        this->hitPoints = 0;
        this->meleeDamage = 0;
        this->range = 0;
        this->rangedDamage = 0;
        this->timeBetweenShots = 0.0f;
        this->splashDamage = 0;
        this->projectileType = 0;
        this->doesRangedDamage = false;
        this->rateOfFire = 0;
        break;
    }

}

actors::~actors()
{
    //dtor
}

void actors::chaseTarget() {
    if (this->idOfTarget >= 0) {
        if (listOfActors[this->idOfTarget].isAlive()) {
            if (listOfActors[this->idOfTarget].getGoal().x != listOfActors[this->idOfTarget].getActorCords().x || listOfActors[this->idOfTarget].getGoal().y != listOfActors[this->idOfTarget].getActorCords().y) {
                //target will move
                //check if this move is allready been caught
                if (!(this->actionPreformedOnTile.x == listOfActors[this->idOfTarget].getGoal().x && this->actionPreformedOnTile.y == listOfActors[this->idOfTarget].getGoal().y)) {
                    //update goal
                    this->updateGoal(listOfActors[this->idOfTarget].getGoal(), 0);
                    this->setIsDoingAttack();
                }
            }
        }
        else {
            this->isMeleeAttacking = false;
            this->isRangedAttacking = false;
            this->clearRoute();
            this->pathFound = false;
            this->realPath = false;
            this->routeNeedsPath = false;
            this->isIdle = true;
        }
    }
    else if (-currentGame.occupiedByBuildingList[this->actorGoal.x][this->actorGoal.y] != this->idOfTarget) {
        //Building destroyed!
        this->isMeleeAttacking = false;
        this->isRangedAttacking = false;
        this->clearRoute();
        this->pathFound = false;
        this->realPath = false;
        this->routeNeedsPath = false;
        this->isIdle = true;
    }
}

void actors::update()
{
    if (this->actorAlive)
    {
        if (this->isIdle) {
            doNextStackedCommand();
        }
        else {
            if ((this->isMeleeAttacking || this->isRangedAttacking)) {
                this->chaseTarget();
            }
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
                    if (!actorInListForPathfinding){ 
                        this->retryWalkingOrChangeGoal();
                    }
                }
                this->houseKeeping();
            }
            else {
                this->searchAltetnative();
            }
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
                this->findNearestSimilarResource();
            }
        }
        else {
            this->isFindingAlternative = false;
            if (this->hasToUnloadResource) {
                this->isWalkingToUnloadingPoint = true;
                this->dropOffTile = listOfDropOffLocations.front();
            }
            this->listOfDropOffLocations.clear();
            this->listOfResourceLocations.clear();
        }
    }
}

std::string actors::getResources() const {
    std::stringstream output;
    output << "Wood: " << this->amountOfWood << " Food: " << this->amountOfFood << " Stone: " << this->amountOfStone << " Gold: " << this->amountOfGold;
    return output.str();
}

void actors::getNewBuildingTileForSameBuilding() {
    nearestBuildingTile tempTile = findNearestBuildingTile(this->buildingId, this->actorId);
    if (tempTile.isSet)
    {
        this->updateGoal(tempTile.location, 0); //this also releases the old building tile, but does not reset the 'buildingId', so we can still use it! But we have to claim new building tile...
        listOfBuildings[this->buildingId].claimFreeBuiildingTile(tempTile.buildingId, this->actorId);
        this->setIsBuildingTrue(this->buildingId, tempTile.actionLocation);
    }
    else {
        //This building is not reachable for this actor! Cancel the current task
        this->clearRoute();
        this->pathFound = false;
        this->realPath = false;
        this->routeNeedsPath = false;
        this->isIdle = true;
        this->isFindingAlternative = false;
        this->isBuilding = false;
        this->isGatheringRecources = false;
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
        if (currentGame.occupiedByActorList[this->actionPreformedOnTile.x][this->actionPreformedOnTile.y] != -1) {
            projectile newProjectile(this->actorCords.x, this->actorCords.y, this->actionPreformedOnTile.x, this->actionPreformedOnTile.y, this->projectileType, this->rangedDamage, this->splashDamage, this->actorId);
            listOfProjectiles.push_back(newProjectile);
        }
        else {
            this->isRangedAttacking = false;
        }
    }
}

bool actors::idle() const
{
    return this->isIdle;
}

bool actors::isAlive() const
{
    return this->actorAlive;
}

cords actors::getActorCords() const
{
    return this->actorCords;
}

void actors::setIsDoingAttack()
{
    if (this->doesRangedDamage) {
        this->isRangedAttacking = true;
    }
    else {
        this->isMeleeAttacking = true;
    }
    this->actionPreformedOnTile = this->actorGoal;
    if (currentGame.occupiedByActorList[this->actorGoal.x][this->actorGoal.y] != -1) {
        this->idOfTarget = currentGame.occupiedByActorList[this->actorGoal.x][this->actorGoal.y];
    }
    else if (currentGame.occupiedByBuildingList[this->actorGoal.x][this->actorGoal.y] != -1) {
        this->idOfTarget = -currentGame.occupiedByActorList[this->actorGoal.x][this->actorGoal.y];
    }
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
        if (currentGame.occupiedByActorList[this->actionPreformedOnTile.x][this->actionPreformedOnTile.y] != -1) {
           if (listOfActors[currentGame.occupiedByActorList[this->actionPreformedOnTile.x][this->actionPreformedOnTile.y]].getTeam() != this->actorTeam) 
           {
                listOfActors[currentGame.occupiedByActorList[this->actionPreformedOnTile.x][this->actionPreformedOnTile.y]].takeDamage(this->meleeDamage, this->actorId);
           }
        }
        else if (currentGame.occupiedByBuildingList[this->actionPreformedOnTile.x][this->actionPreformedOnTile.y] != -1)
        {
            if (listOfBuildings[currentGame.occupiedByBuildingList[this->actionPreformedOnTile.x][this->actionPreformedOnTile.y]].getTeam() != this->actorTeam) 
            {
                listOfBuildings[currentGame.occupiedByBuildingList[this->actionPreformedOnTile.x][this->actionPreformedOnTile.y]].takeDamage(this->meleeDamage);
            }
        }
        else {
            this->isMeleeAttacking = false;
        }
    }
}

const std::list<cords>& actors::getRejectedTargetsList() const
{
    return listOfTargetsToRejectUntilSuccesfullMovement;
}

void actors::takeDamage(int amountOfDamage, int idOfAttacker)
{
    this->actorHealth -= amountOfDamage;
    if (this->actorHealth <= 0) {
        this->killActor();
    }
    this->fightOrFlight(idOfAttacker);
}

void actors::killActor() {
    this->actorAlive = false;
    for (auto& rows : currentGame.occupiedByActorList)
    {
        for (auto& elem : rows)
        {
            if (elem == this->actorId) {
                elem = -1;
            }
        }
    }
}

int actors::getType() const
{
    return this->actorType;
}

std::string actors::nameOfActor() const
{
    switch(this->actorType)
    {
    case 0:
        return  "Villager";
        break;
    case 1:
        return "Swordsman";
        break;
    default:
        return "NameOfActorNotSet";
        break;
    }
}

void actors::updateGoal(cords location, int waitTime)
{
    //check if values are in bounds
    if(location.x >= 0 && location.x < MAP_WIDTH && location.y >= 0 && location.y < MAP_HEIGHT)
    {
        if(this->buildingId != -1)
        {
            listOfBuildings[this->buildingId].removeActorFromBuildingTile(this->actorId);
        }
        this->actorGoal = location;
        this->actorCommandGoal = location;
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
        this->actorCords = this->actorGoal;
    }
    else if (this->busyWalking && (currentGame.elapsedTime - this->timeLastUpdate) > 0.5f && !this->movedMoreThanHalf)
    {

        currentGame.occupiedByActorList[this->actorCords.x][this->actorCords.y] = -1;
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
    this->actorGoal = this->route.back().position;

    currentGame.occupiedByActorList[this->route.back().position.x][this->route.back().position.y] = this->actorId;

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
                this->actorGoal = this->actorCommandGoal;
                this->routeNeedsPath = true;
                this->pathFound = false;
                this->realPath = false;
                listOfActorsWhoNeedAPath.push_back(this->actorId);
                this->retries += +1;
        }
        else if (this->hasToUnloadResource || this->isGatheringRecources || this->isBuilding)
        {
            this->listOfTargetsToRejectUntilSuccesfullMovement.push_back(this->actorCommandGoal);
            this->clearRoute();
            this->routeNeedsPath = false;
            this->pathFound = false;
            this->realPath = false;
            this->isFindingAlternative = true;
        }
        else
        {
            this->listOfTargetsToRejectUntilSuccesfullMovement.push_back(this->actorCommandGoal);
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
    if (actorOrientation(this->actorCords.x, this->actorCords.y, this->route.back().position.x, this->route.back().position.y) == this->orientation)
    {
        if ((this->isGatheringRecources || this->isMeleeAttacking) && this->route.size() == 1)
        {
            //if (!this->isWalkingToUnloadingPoint) {
                this->clearRoute();
            //}
        }
        else if (this->isRangedAttacking && distEuclidean(this->actorCords.x, this->actorCords.y, this->actorGoal.x, this->actorGoal.y) <= this->range)
        {
            this->clearRoute();
        }
        else
        {
            //De actor staat met zijn neus de goede kant op en kan dus gaan lopen als de tegel vrij is!
            if (currentGame.occupiedByActorList[this->route.back().position.x][this->route.back().position.y] == -1)
            {
                this->listOfTargetsToRejectUntilSuccesfullMovement.clear();
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
        this->orientation = newOrientation(this->orientation, actorOrientation(this->actorCords.x, this->actorCords.y, this->route.back().position.x, this->route.back().position.y));
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
        if (this->orientation == actorOrientation(this->actorCords.x, this->actorCords.y, this->actionPreformedOnTile.x, this->actionPreformedOnTile.y)) {
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
            this->orientation = newOrientation(this->orientation, actorOrientation(this->actorCords.x, this->actorCords.y, this->actionPreformedOnTile.x, this->actionPreformedOnTile.y));
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
    if(this->ResourceBeingGatherd == resourceTypes::resourceWood)
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
    if(this->ResourceBeingGatherd == resourceTypes::resourceWood)
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
        this->offSetY = static_cast<float>(-northSouth);
        break;
    case 1:
        this->offSetX = static_cast<float>(diagonalX);
        this->offSetY = static_cast<float>(-diagonalY);
        break;
    case 2:
        this->offSetX = static_cast<float>(eastWest);
        this->offSetY = 0;
        break;
    case 3:
        this->offSetX = static_cast<float>(diagonalX);
        this->offSetY = static_cast<float>(diagonalY);
        break;
    case 4:
        this->offSetX = 0;
        this->offSetY = static_cast<float>(northSouth);
        break;
    case 5:
        this->offSetX = static_cast<float>(-diagonalX);
        this->offSetY = static_cast<float>(diagonalY);
        break;
    case 6:
        this->offSetX = static_cast<float>(-eastWest);
        this->offSetY = 0;
        break;
    case 7:
        this->offSetX = static_cast<float>(-diagonalX);
        this->offSetY = static_cast<float>(-diagonalY);
        break;

    }
}

void actors::animateWalkingToResource()
{
    int northSouth;
    int eastWest;
    int diagonalX;
    int diagonalY;
    if(this->ResourceBeingGatherd == resourceTypes::resourceWood)
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
    if (isReallyNextToResource(this->actorCords.x, this->actorCords.y, this->actionPreformedOnTile.x, this->actionPreformedOnTile.y)) {
        if (currentGame.objectLocationList[this->actionPreformedOnTile.x][this->actionPreformedOnTile.y] != -1)
        {
            if (currentGame.elapsedTime - this->timeStartedGatheringRecource > 2)
            {
                switch (this->ResourceBeingGatherd)
                {
                case resourceTypes::resourceWood:  //wood
                    this->amountOfWood += +1;
                    break;
                case resourceTypes::resourceFood: //food
                    this->amountOfFood += +1;
                    break;
                case resourceTypes::resourceStone: //stone
                    this->amountOfStone += +1;
                    break;
                case resourceTypes::resourceGold: // gold
                    this->amountOfGold += +1;
                    break;
                }
                listOfObjects[currentGame.objectLocationList[this->actionPreformedOnTile.x][this->actionPreformedOnTile.y]].substractResource();
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
    case resourceTypes::resourceWood:
        //recieves only wood
        listOfPlayers[this->actorTeam].addResources(resourceTypes::resourceWood, this->amountOfWood);
        this->amountOfWood = 0;
        break;
    case resourceTypes::resourceFood:
        //recieves only food
        listOfPlayers[this->actorTeam].addResources(resourceTypes::resourceFood, this->amountOfFood);
        this->amountOfFood = 0;
        break;
    case resourceTypes::resourceStone:
        //recieves only stone
        listOfPlayers[this->actorTeam].addResources(resourceTypes::resourceStone, this->amountOfStone);
        this->amountOfStone = 0;
        break;
    case resourceTypes::resourceGold:
        //recieves only gold
        listOfPlayers[this->actorTeam].addResources(resourceTypes::resourceGold, this->amountOfGold);
        this->amountOfGold = 0;
        break;
    case resourceTypes::All:
        //recieves all the resources!
        listOfPlayers[this->actorTeam].addResources(resourceTypes::resourceWood, this->amountOfWood);
        this->amountOfWood = 0;
        listOfPlayers[this->actorTeam].addResources(resourceTypes::resourceFood, this->amountOfFood);
        this->amountOfFood = 0;
        listOfPlayers[this->actorTeam].addResources(resourceTypes::resourceStone, this->amountOfStone);
        this->amountOfStone = 0;
        listOfPlayers[this->actorTeam].addResources(resourceTypes::resourceGold, this->amountOfGold);
        this->amountOfGold = 0;
        break;
    }
    if (this->actionPreformedOnTile.x >= 0 && this->actionPreformedOnTile.x < MAP_WIDTH && this->actionPreformedOnTile.y >= 0 && this->actionPreformedOnTile.y < MAP_HEIGHT) {
        if (currentGame.objectLocationList[this->actionPreformedOnTile.x][this->actionPreformedOnTile.y] != -1)
        {
            this->updateGoal(this->actionPreformedOnTile, 0);
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
            this->actionPreformedOnTile = this->actorGoal;
            this->isWalkingToUnloadingPoint = false;
            this->isAtCarryCapacity = false;
            this->carriesRecources = false;
            this->isAtRecource = false;
            this->hasToUnloadResource = false;
            this->timeStartedWalkingToRecource = 0.0f;
            this->isFindingAlternative = true;
        }
    }
    else {
        this->routeNeedsPath = false;
        this->pathFound = false;
        this->actionPreformedOnTile = this->actorGoal;
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
    if (flag && currentGame.objectLocationList[this->actorGoal.x][this->actorGoal.y] != -1) 
    {
        this->ResourceBeingGatherd = listOfObjects[currentGame.objectLocationList[this->actorGoal.x][this->actorGoal.y]].getTypeOfResource();
        this->actionPreformedOnTile = this->actorGoal;
    }
}

void actors::setCommonGoalTrue()
{
    this->commonGoal = true;
}

void actors::findNearestSimilarResource()
{
    cords newResourceCords = findResource(this->ResourceBeingGatherd, this->actorId);
    if (newResourceCords.x != -1) {
        this->actorGoal = newResourceCords;
        this->actorCommandGoal = newResourceCords;
        this->actionPreformedOnTile = this->actorGoal;
        this->waitForAmountOfFrames = 0;
        this->routeNeedsPath = true;
        listOfActorsWhoNeedAPath.push_back(this->actorId);
    }
    else {
        this->clearRoute();
        this->pathFound = false;
        this->realPath = false;
        this->routeNeedsPath = false;
        this->isIdle = true;
        this->isFindingAlternative = false;
        this->isBuilding = false;
        this->isGatheringRecources = false;
    }
}

void actors::findNearestDropOffPoint()
{
    if (!this->routeNeedsPath) {
        //Bugfix for edge case where the actor is ocuping a tile adjacent to a dropoff building
        int adjacentBuildingAvailableId = adjacentTileIsCorrectDropOffPoint(this->actorCords, this->ResourceBeingGatherd, this->actorTeam);
        if (adjacentBuildingAvailableId != -1) {
            listOfDropOffLocations.push_back({ 0, this->actorCords.x, this->actorCords.y, adjacentBuildingAvailableId, true, 1 });
            this->actorGoal = this->actorCords;
            this->actorCommandGoal = this->actorCords;
            this->waitForAmountOfFrames = 0;
            this->routeNeedsPath = true;
            listOfActorsWhoNeedAPath.push_back(this->actorId);
        }
        else {
            if (listOfDropOffLocations.empty())
            {
                for (int i = 0; i < listOfBuildings.size(); i++)
                {
                    if ((listOfBuildings[i].getRecievesWhichResources() == this->ResourceBeingGatherd || listOfBuildings[i].getRecievesWhichResources() == resourceTypes::All) && listOfBuildings[i].getTeam() == this->actorTeam && listOfBuildings[i].getCompleted())
                    {
                        std::vector<adjacentTile> tileList = listOfBuildings[i].getDropOffTiles();
                        for (int j = 1; j < tileList.size(); j++)
                        {
                            float tempDeltaDistance = static_cast<float>(dist(this->actorCords.x, this->actorCords.y, tileList[j].goal.x, tileList[j].goal.y));
                            listOfDropOffLocations.push_back({ tempDeltaDistance, tileList[j].tileCords, tileList[j].goal.x, tileList[j].goal.y, i, true, tileList[j].tileId });
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
                this->updateGoal(listOfDropOffLocations.front().actionLocation, 0);
                listOfBuildings[this->buildingId].removeActorFromBuildingTile(this->actorId);
                this->buildingId = listOfDropOffLocations.front().buildingId;
                listOfBuildings[this->buildingId].claimFreeBuiildingTile(listOfDropOffLocations.front().tileId, this->actorId);
                listOfDropOffLocations.pop_front();
            }
        }
    }
}

int actors::getTeam() const
{
    return this->actorTeam;
}

bool actors::isInitialized() const
{
    return this->initialized;
}

void actors::calculateRoute()
{
    if(this->routeNeedsPath)
    {
        this->route.clear();
        this->actorRealGoal = this->actorGoal;
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
    int startCell = (actorCords.x*MAP_HEIGHT)+actorCords.y; //eigen positie
    int endCell = (actorGoal.x*MAP_HEIGHT)+actorGoal.y; //doel positie
    updateCells(endCell, startCell, cellsList);
    std::list<Cells*> listToCheck;
    std::list<Cells*> checkedList;
    bool endReached = false;

    //check of de doelcel niet 1 hokje weg is 
    if(((actorCords.x-actorGoal.x == 0) ||(actorCords.x-actorGoal.x == -1) ||(actorCords.x-actorGoal.x == 1)) && ((actorCords.y-actorGoal.y == 0) ||(actorCords.y-actorGoal.y == -1) ||(actorCords.y-actorGoal.y == 1)))
    {
        //en geen obstakel is of het hokje van de actor zelf is
        if(!cellsList[endCell].obstacle || (actorCords.x == actorGoal.x && actorCords.y == actorGoal.y))
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
        cellsList[startCell].totalCostGuess = dist(cellsList[startCell].position.x, cellsList[startCell].position.y, cellsList[endCell].position.x, cellsList[endCell].position.y);
        cellsList[startCell].visited = true;
        cellsList[startCell].parentCellId =  -1;
        cellsList[startCell].costToGoal = dist(cellsList[startCell].position.x, cellsList[startCell].position.y, cellsList[endCell].position.x, cellsList[endCell].position.y);
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
                    cellsList[*iterator].costToGoal = dist(cellsList[*iterator].position.x,cellsList[*iterator].position.y,cellsList[endCell].position.x,cellsList[endCell].position.y);
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
                this->actorGoal = cellsList[(*checkedList.front()).cellId].position;
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

void actors::clearCommandStack()
{
    this->listOfOrders.clear();
}

void actors::fightOrFlight(int idOfAttacker)
{
    if (idOfAttacker >= 0) {
        //This unit is being Attaked! Whut do?

        //How many attacks does it take to kill me?
        int hitsUntilDead = 0;
        if (listOfActors[idOfAttacker].getMeleeDMG() > listOfActors[idOfAttacker].getRangedDMG()) {
            hitsUntilDead = this->actorHealth / listOfActors[idOfAttacker].getMeleeDMG();
        }
        else {
            hitsUntilDead = this->actorHealth / listOfActors[idOfAttacker].getRangedDMG();
        }

        //How many hits do I need?
        int hitsToWin = 0;
        if (this->meleeDamage > this->rangedDamage) {
            hitsToWin = listOfActors[idOfAttacker].getHealth().first / this->meleeDamage;
        }
        else {
            hitsToWin = listOfActors[idOfAttacker].getHealth().first / this->rangedDamage;
        }

        if (hitsToWin < hitsUntilDead) {
            //Fight
            this->updateGoal(listOfActors[idOfAttacker].getActorCords(), 0);
            this->setIsDoingAttack();
        }
        else {
            //Flight
            int moveX = 0;
            int moveY = 0;
            if (this->actorCords.x < listOfActors[idOfAttacker].getActorCords().x) {
                //We want to go to a lower x
                moveX = -1;
            }
            else if (this->actorCords.x > listOfActors[idOfAttacker].getActorCords().x) {
                //We want to go to a higher x
                moveX = 1;
            }
            if (this->actorCords.x < listOfActors[idOfAttacker].getActorCords().y) {
                //We want to go to a lower Y
                moveY = -1;
            }
            else if (this->actorCords.x > listOfActors[idOfAttacker].getActorCords().y) {
                //We want to go to a higher Y
                moveY = 1;
            }

            if (currentGame.isPassable({ this->actorCords.x + moveX, this->actorCords.y + moveY })) {
                //Then do it!
                this->updateGoal({ this->actorCords.x + moveX, this->actorCords.y + moveY }, 0);
            }
            else {
                //try only x
                if (currentGame.isPassable({ this->actorCords.x + moveX, this->actorCords.y })) {
                    //Then do it!
                    this->updateGoal({ this->actorCords.x + moveX, this->actorCords.y }, 0);
                }
                else {
                    //try only Y
                    if (currentGame.isPassable({ this->actorCords.x, this->actorCords.y + moveY })) {
                        //Then do it!
                        this->updateGoal({ this->actorCords.x, this->actorCords.y + moveY }, 0);
                    }
                    else {
                        //Well, not good, just two options left!
                        if (currentGame.isPassable({ this->actorCords.x - moveX, this->actorCords.y + moveY })) {
                            this->updateGoal({ this->actorCords.x - moveX, this->actorCords.y + moveY }, 0);
                        }
                        else if (currentGame.isPassable({ this->actorCords.x + moveX, this->actorCords.y - moveY })) {
                            this->updateGoal({ this->actorCords.x + moveX, this->actorCords.y - moveY }, 0);
                        }
                        else {
                            //We are doomed, so fight till the last breath! Out in a blaze of glory and all of that
                            this->updateGoal(listOfActors[idOfAttacker].getActorCords(), 0);
                            this->setIsDoingAttack();
                        }
                    }
                }
            }
        }
    }
}

void actors::doNextStackedCommand()
{
    if (!this->listOfOrders.empty()) {
        switch (this->listOfOrders.front().orderType) {
        case stackOrderTypes::stackActionMove:
            this->updateGoal(this->listOfOrders.front().goal, 0);
            break;
        case stackOrderTypes::stackActionGather:
            this->updateGoal(this->listOfOrders.front().goal, 0);
            this->setGatheringRecource(true);
            break;
        case stackOrderTypes::stackActionBuild:
            if (currentGame.occupiedByBuildingList[this->listOfOrders.front().goal.x][this->listOfOrders.front().goal.y] != -1) {
                if (!listOfBuildings[currentGame.occupiedByBuildingList[this->listOfOrders.front().goal.x][this->listOfOrders.front().goal.y]].getCompleted()
                    && listOfBuildings[currentGame.occupiedByBuildingList[this->listOfOrders.front().goal.x][this->listOfOrders.front().goal.y]].getTeam() == this->actorTeam) {
                    nearestBuildingTile tempTile = findNearestBuildingTile(currentGame.occupiedByBuildingList[this->listOfOrders.front().goal.x][this->listOfOrders.front().goal.y], this->actorId);
                    this->updateGoal(tempTile.location, 0);
                    listOfBuildings[currentGame.occupiedByBuildingList[this->listOfOrders.front().goal.x][this->listOfOrders.front().goal.y]].claimFreeBuiildingTile(tempTile.buildingId, this->actorId);
                    this->setIsBuildingTrue(currentGame.occupiedByBuildingList[this->listOfOrders.front().goal.x][this->listOfOrders.front().goal.y], this->listOfOrders.front().goal);
                }
            }
            break;
        case stackOrderTypes::stackActionAttack:
            this->updateGoal(this->listOfOrders.front().goal, 0);
            this->setIsDoingAttack();
            break;
        }
        listOfOrders.pop_front();
    }
}

void actors::stackOrder(cords goal, const stackOrderTypes orderType)
{
    this->listOfOrders.push_back({ goal, orderType });
}

void actors::routing(std::vector<Cells>& cellsList, int endCell, int startCell, bool endReached)
{
    //Zet de tegel waarnaartoe gelopen wordt in de lijst
    this->route.push_back({ cellsList[endCell].position, cellsList[endCell].visited, cellsList[endCell].parentCellId });
    while (!endReached)
    {
        if (route.back().visited == true)
        {
            //Voeg voor iedere tegel de parent toe aan de lijst (dit was de tegel met de korste weg naar deze tegel)
            this->route.push_back({ cellsList[route.back().parentCellId].position, cellsList[route.back().parentCellId].visited, cellsList[route.back().parentCellId].parentCellId });
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
                if (this->route.back().position.x == 0 && this->route.back().position.x == 1) {
                    //Logische parent; doe niets!
                }
                else if(this->route.back().position.x == 0 && this->route.back().position.x == 1) {
                    //Logische parent; doe niets!
                }
                else if (this->route.back().position.x == 1 && this->route.back().position.x == 1) {
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
                if(i !=  this->actorCords.x && j != this->actorCords.y)
                {
                    currentGame.occupiedByActorList[i][j] = -1;
                }
            }
        }
    }
    currentGame.occupiedByActorList[this->actorCords.x][this->actorCords.y] = this->actorId;
}

std::pair<int, int> actors::getHealth() const
{
    return {this->actorHealth, this->hitPoints};
}

int actors::getMeleeDMG() const
{
    return this->meleeDamage;
}

int actors::getRangedDMG() const
{
    return this->rangedDamage;
}

void actors::drawActor()
{
    int i = this->actorCords.x;
    int j = this->actorCords.y;
    int x = this->actorGoal.x;
    int y = this->actorGoal.y;
    cords position = worldSpace({ i,j });
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
            cords nPosition = worldSpace({ x, y });
            int deltaX = position.x - nPosition.x;
            int deltaY = position.y - nPosition.y;
            float deltaTime = currentGame.elapsedTime - this->timeLastUpdate;
            float deltaXCompleted = deltaX * deltaTime;
            float deltaYCompleted = deltaY * deltaTime;
            position = { position.x - static_cast<int>(deltaXCompleted),  position.y - static_cast<int>(deltaYCompleted) };
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
        if(this->ResourceBeingGatherd == resourceTypes::resourceWood)
        {
            spriteOffset = 128;
        }
        else if(this->ResourceBeingGatherd == resourceTypes::resourceFood || this->ResourceBeingGatherd == resourceTypes::resourceStone)
        {
            spriteOffset = 256;
        }
        else if(this->ResourceBeingGatherd == resourceTypes::resourceGold)
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
    position = { position.x + static_cast<int>(this->offSetX), position.y + static_cast<int>(this->offSetY) };

    bool drawHealth = false;

    if(currentGame.isInSelectedActors(this->actorId))
    {
        currentGame.spriteUnitSelectedTile.setPosition(static_cast<float>(position.x), static_cast<float>(position.y));
        window.draw(currentGame.spriteUnitSelectedTile);
        drawHealth = true;
        if (!this->listOfOrders.empty() && this->actorTeam == currentPlayer.getTeam()) {
            for (const orderStack order : this->listOfOrders) {
                currentGame.spriteFlag.setPosition(static_cast<float>(worldSpace(order.goal).x), static_cast<float>(worldSpace(order.goal).y));
                window.draw(currentGame.spriteFlag);
            }
        }
    }

    if (this->actorHealth < this->hitPoints) {
        drawHealth = true;
    }


    switch (this->actorType) 
    {
    case 0:
        currentGame.spriteVillager.setPosition(static_cast<float>(position.x), static_cast<float>(position.y));
        currentGame.spriteVillager.setTextureRect(sf::IntRect(spriteXoffset, spriteYoffset, 16, 32));
        window.draw(currentGame.spriteVillager);
        break;
    case 1:
        currentGame.spriteSwordsman.setPosition(static_cast<float>(position.x), static_cast<float>(position.y));
        currentGame.spriteSwordsman.setTextureRect(sf::IntRect(spriteXoffset, spriteYoffset, 16, 32));
        window.draw(currentGame.spriteSwordsman);
    }

    if (drawHealth) {
        currentGame.healthBarBackground.setFillColor(sf::Color(255, 0, 0));
        currentGame.healthBarBackground.setSize(sf::Vector2f(32, 3));
        currentGame.healthBarGreenBar.setFillColor(sf::Color(0, 255, 0));
        currentGame.healthBarGreenBar.setSize(sf::Vector2f((static_cast<float>(this->actorHealth) / static_cast<float>(this->hitPoints)) * 32, 3));
        currentGame.healthBarBackground.setPosition(static_cast<float>(position.x+16), static_cast<float>(position.y-28));
        currentGame.healthBarGreenBar.setPosition(static_cast<float>(position.x+16), static_cast<float>(position.y-28));
        window.draw(currentGame.healthBarBackground);
        window.draw(currentGame.healthBarGreenBar);

    }


}

void actors::buildBuilding()
{
    if(currentGame.occupiedByBuildingList[this->actionPreformedOnTile.x][this->actionPreformedOnTile.y] != -1)
    {
        if(!listOfBuildings[currentGame.occupiedByBuildingList[this->actionPreformedOnTile.x][this->actionPreformedOnTile.y]].getCompleted())
        {
            if(currentGame.elapsedTime - this->timeStartedGatheringRecource > 1)
            {
                listOfBuildings[currentGame.occupiedByBuildingList[this->actionPreformedOnTile.x][this->actionPreformedOnTile.y]].addBuildingPoint();
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

int actors::getActorId()  const
{
    return this->actorId;
}

bool actors::isGathering() const
{
    return this->isGatheringRecources;
}

bool actors::getIsBuilding() const
{
    return this->isBuilding;
}

cords actors::getGoal() const
{
    return { this->actorGoal.x, this->actorGoal.y };
}

int actors::getBuildingId() const
{
    return this->buildingId;
}

resourceTypes actors::getResourceGathered() const
{
    return this->ResourceBeingGatherd;
}

void actors::setIsBuildingTrue(int buildingId, cords goal)
{
    this->isBuilding = true;
    this->buildingId = buildingId;
    this->isGatheringRecources = false;
    this->ResourceBeingGatherd = resourceTypes::resourceFood;
    this->actionPreformedOnTile = goal;
}

void actors::renderPath()
{
    std::list<routeCell>::iterator it;
    for(it = route.begin(); it!=route.end(); it++)
    {
        currentGame.spriteSelectedTileForPath.setPosition(static_cast<float>(worldSpace(it->position).x), static_cast<float>(worldSpace(it->position).y));
        window.draw(currentGame.spriteSelectedTileForPath);
    }
}
