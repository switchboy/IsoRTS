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
#include "cells.h"
#include "Actors/Actor.h"
#include "Actors/ActorHelper.h"

//std::vector<int> listOfActorsWhoNeedAPath;
//std::vector <Actors> listOfActors;


Actors::Actors(int type, cords location, int actorTeam, int actorId)
{
    currentGame.occupiedByActorList[location.x][location.y].push_back(actorId);
    listOfPlayers[actorTeam].addToCurrentPopulation(1);
    this->actorType = type;
    this->actorTeam = actorTeam;
    this->actorId = actorId;
    this->actorCords = location;
    this->timeLastRetry = 0;
    this->isIdle = true;
    this->actorCommandGoal = { 0,0 };
    this->actorRealGoal = { 0, 0 };
    this->buildingId = -1;
    this->commonGoal = false;
    this->currentFrame = 0;
    this->idOfTarget = 0;
    this->isFindingAlternative = false;
    this->isRangedAttacking = false;
    this->noPathPossible = false;
    this->realPath = false;
    this->timeLastPathTry = currentGame.getTime();
    this->timeLastUpdate = currentGame.getTime();
    this->timeStartedAction = 0;
    this->waitForAmountOfFrames = 0;
    this->reachedUnloadingPoint = false;
    this->timeLastRetry = 0;
    this->actorAlive = true;
    this->actorGoal = location;
    this->orientation = 0;
    this->spriteYOffset = 0;
    this->retries = 0;
    this->amountOfFood = 0;
    this->amountOfGold = 0;
    this->amountOfStone = 0;
    this->amountOfWood = 0;
    this->timeLastAttempt = 1;
    this->timeLastOffsetChange = 0;
    this->timeStartedWalkingToRecource = 0;
    this->wasAttackMove = false;
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
    this->lastTile = false;
    this->timePassedSinceChangingOffset = 0;
    this->actorHealth = listOfActorTemplates[type].getHitPoints();
    this->hitPoints = listOfActorTemplates[type].getHitPoints();
    this->meleeDamage = listOfActorTemplates[type].getMeleeDamage();
    this->range = listOfActorTemplates[type].getRange();
    this->rangedDamage = listOfActorTemplates[type].getRangedDamage();
    this->timeBetweenShots = listOfActorTemplates[type].getTimeBetweenShots();
    this->splashDamage = listOfActorTemplates[type].getSplashDamage();
    this->projectileType = listOfActorTemplates[type].getProjectileType();
    this->doesRangedDamage = listOfActorTemplates[type].getDoesRangedDamage();
    this->rateOfFire = listOfActorTemplates[type].getRateOfFire();
    this->timeToCrossOneTile = listOfActorTemplates[type].getTimeToCrossOneTile();
    this->isWalkingBackXYDrawOverride.isActive = false;
    this->isWalkingToMiddleOfSquare = false;
}

bool Actors::chaseTarget() {
    if (this->idOfTarget >= 0) {
        if (listOfActors[this->idOfTarget].getIsAlive()) {
            if (listOfActors[this->idOfTarget].getGoal().x != listOfActors[this->idOfTarget].getActorCords().x || listOfActors[this->idOfTarget].getGoal().y != listOfActors[this->idOfTarget].getActorCords().y && listOfActors[this->idOfTarget].getHasRoute()) {
                if (!(listOfActors[this->idOfTarget].getGoal().x == this->actorCords.x && listOfActors[this->idOfTarget].getGoal().y == this->actorCords.y)) {
                    if (this->lastChaseTime + 500 < currentGame.getTime()) {
                        this->lastChaseTime = currentGame.getTime();
                        if (this->actionPreformedOnTile.x != listOfActors[this->idOfTarget].getGoal().x || this->actionPreformedOnTile.y != listOfActors[this->idOfTarget].getGoal().y) {
                            if (this->wasAttackMove) {
                                this->updateGoal(listOfActors[this->idOfTarget].getGoal(), 0);
                                this->setIsDoingAttack(true);
                                this->wasAttackMove = true;
                            }
                            else {
                                this->updateGoal(listOfActors[this->idOfTarget].getGoal(), 0);
                                this->setIsDoingAttack(true);
                            }
                        }
                    }
                    return true;
                }
                else {
                    //The actor is fighting back! So no action is required
                    return false;
                }
            }
            else {
                return false;
            }
        }
        else if (this->wasAttackMove) {
            this->selectAndAttackNextTarget(30);
            return true;
        } else {
            this->isMeleeAttacking = false;
            this->isRangedAttacking = false;
            this->clearRoute();
            this->pathFound = false;
            this->realPath = false;
            this->routeNeedsPath = false;
            this->isIdle = true;
            return false;
        }
    }
    else if (-currentGame.occupiedByBuildingList[this->actorGoal.x][this->actorGoal.y] != this->idOfTarget) {

        if (this->wasAttackMove) {
            this->selectAndAttackNextTarget(30);
            return true;
        }
        else {
            //Building destroyed!
            this->isMeleeAttacking = false;
            this->isRangedAttacking = false;
            this->clearRoute();
            this->pathFound = false;
            this->realPath = false;
            this->routeNeedsPath = false;
            this->isIdle = true;
            return false;
        }
    }
    return false;
}

void Actors::anitmateWalkingBackAfterAbortedCommand() {
    int timeSinceWalkingBackStarted = currentGame.getTime() - this->dataOnPositionAbortedWalk.timeWalkingBackStarted;
    float deltaXCompleted = static_cast<float>(this->dataOnPositionAbortedWalk.startDeltaX) * (
        ((static_cast<float>(this->dataOnPositionAbortedWalk.timePassedSinceChangingOffset) - static_cast<float>(timeSinceWalkingBackStarted)) / 1000.f) * (static_cast<float>(this->dataOnPositionAbortedWalk.speedMultiplier) / 1000.f)
        );
    float deltaYCompleted = static_cast<float>(this->dataOnPositionAbortedWalk.startDeltaY) * (
        ((static_cast<float>(this->dataOnPositionAbortedWalk.timePassedSinceChangingOffset) - static_cast<float>(timeSinceWalkingBackStarted)) / 1000.f) * (static_cast<float>(this->dataOnPositionAbortedWalk.speedMultiplier) / 1000.f)
        );
    this->isWalkingBackXYDrawOverride.newXY = { this->dataOnPositionAbortedWalk.position.x - static_cast<int>(deltaXCompleted),  this->dataOnPositionAbortedWalk.position.y - static_cast<int>(deltaYCompleted) };
}


void Actors::walkBackAfterAbortedCommand() {
    int timeSinceWalkingBackStarted = currentGame.getTime() - this->dataOnPositionAbortedWalk.timeWalkingBackStarted;
    if (this->dataOnPositionAbortedWalk.timePassedSinceChangingOffset > timeSinceWalkingBackStarted) {
        this->orientation = actorHelper::newOrientation(this->orientation, actorHelper::actorOrientation(this->dataOnPositionAbortedWalk.nPosition.x, this->dataOnPositionAbortedWalk.nPosition.y, this->dataOnPositionAbortedWalk.position.x, this->dataOnPositionAbortedWalk.position.y));
        this->isWalkingBackXYDrawOverride.isActive = true;
    }
    else {
        this->isWalkingToMiddleOfSquare = false;
        this->isWalkingBackXYDrawOverride.isActive = false;
        this->busyWalking = false;
    }
}

void Actors::makeSureActorIsOnTheMap() {
    if (this->actorCords.x >= 0 && this->actorCords.x < MAP_WIDTH && this->actorCords.y >= 0 && this->actorCords.y < MAP_HEIGHT) {
        if (currentGame.occupiedByActorList[this->actorCords.x][this->actorCords.y].empty()) {
            currentGame.occupiedByActorList[this->actorCords.x][this->actorCords.y].push_back(this->actorId);
        }
        else {
            bool isOnList = false;
            for (int& id : currentGame.occupiedByActorList[this->actorCords.x][this->actorCords.y]) {
                if (id == this->actorId) { isOnList = true; }
            }
            if (!isOnList) {
                currentGame.occupiedByActorList[this->actorCords.x][this->actorCords.y].push_back(this->actorId);
            }
        }
    }
}


std::string boolAsText(bool it)
{
    std::stringstream converter;
    converter << std::boolalpha << it;
    return converter.str();
}

void Actors::printActorDebugText()
{
    std::stringstream actorDeclaringString;
    actorDeclaringString << "Showing Actor ID: " << actorId;
    gameText.addDebugMessage(actorDeclaringString.str(), 0);

    //bool actorAlive
    if (actorAlive) {
        std::stringstream textStream;
        textStream << "actorAlive: " << boolAsText(actorAlive);
        gameText.addDebugMessage(textStream.str(), 0);
    }
    else {
        std::stringstream textStream;
        textStream << "actorAlive: " << boolAsText(actorAlive);
        gameText.addDebugMessage(textStream.str(), 1);
    }

    //bool isIdle
    if (isIdle) {
        std::stringstream textStream;
        textStream << "isIdle: " << boolAsText(isIdle);
        gameText.addDebugMessage(textStream.str(), 0);
    }
    else {
        std::stringstream textStream;
        textStream << "isIdle: " << boolAsText(isIdle);
        gameText.addDebugMessage(textStream.str(), 1);
    }

    //bool doesRangedDamage
    if (doesRangedDamage) {
        std::stringstream textStream;
        textStream << "doesRangedDamage: " << boolAsText(doesRangedDamage);
        gameText.addDebugMessage(textStream.str(), 0);
    }
    else {
        std::stringstream textStream;
        textStream << "doesRangedDamage: " << boolAsText(doesRangedDamage);
        gameText.addDebugMessage(textStream.str(), 1);
    }

    //bool commonGoal
    if (commonGoal) {
        std::stringstream textStream;
        textStream << "commonGoal: " << boolAsText(commonGoal);
        gameText.addDebugMessage(textStream.str(), 0);
    }
    else {
        std::stringstream textStream;
        textStream << "commonGoal: " << boolAsText(commonGoal);
        gameText.addDebugMessage(textStream.str(), 1);
    }

    //bool goalNeedsUpdate
    if (goalNeedsUpdate) {
        std::stringstream textStream;
        textStream << "goalNeedsUpdate: " << boolAsText(goalNeedsUpdate);
        gameText.addDebugMessage(textStream.str(), 0);
    }
    else {
        std::stringstream textStream;
        textStream << "goalNeedsUpdate: " << boolAsText(goalNeedsUpdate);
        gameText.addDebugMessage(textStream.str(), 1);
    }

//bool isWalkingToMiddleOfSquare
    if (isWalkingToMiddleOfSquare) {
        std::stringstream textStream;
        textStream << "isWalkingToMiddleOfSquare: " << boolAsText(isWalkingToMiddleOfSquare) << " (canceld command)" ;
        gameText.addDebugMessage(textStream.str(), 0);
    }
    else {
        std::stringstream textStream;
        textStream << "isWalkingToMiddleOfSquare: " << boolAsText(isWalkingToMiddleOfSquare);
        gameText.addDebugMessage(textStream.str(), 1);
    }

//bool isBackAtOwnSquare
    if (isBackAtOwnSquare) {
        std::stringstream textStream;
        textStream << "isBackAtOwnSquare: " << boolAsText(isBackAtOwnSquare);
        gameText.addDebugMessage(textStream.str(), 0);
    }
    else {
        std::stringstream textStream;
        textStream << "isBackAtOwnSquare: " << boolAsText(isBackAtOwnSquare);
        gameText.addDebugMessage(textStream.str(), 1);
    }

//bool lastTile
    if (lastTile) {
        std::stringstream textStream;
        textStream << "lastTile: " << boolAsText(lastTile);
        gameText.addDebugMessage(textStream.str(), 0);
    }
    else {
        std::stringstream textStream;
        textStream << "lastTile: " << boolAsText(lastTile);
        gameText.addDebugMessage(textStream.str(), 1);
    }
//bool routeNeedsPath
    if (routeNeedsPath) {
        std::stringstream textStream;
        textStream << "routeNeedsPath: " << boolAsText(routeNeedsPath);
        gameText.addDebugMessage(textStream.str(), 0);
    }
    else {
        std::stringstream textStream;
        textStream << "routeNeedsPath: " << boolAsText(routeNeedsPath);
        gameText.addDebugMessage(textStream.str(), 1);
    }
//bool noPathPossible
    if (noPathPossible) {
        std::stringstream textStream;
        textStream << "noPathPossible: " << boolAsText(noPathPossible);
        gameText.addDebugMessage(textStream.str(), 0);
    }
    else {
        std::stringstream textStream;
        textStream << "noPathPossible: " << boolAsText(noPathPossible);
        gameText.addDebugMessage(textStream.str(), 1);
    }
//bool pathFound
    if (pathFound) {
        std::stringstream textStream;
        textStream << "pathFound: " << boolAsText(pathFound);
        gameText.addDebugMessage(textStream.str(), 0);
    }
    else {
        std::stringstream textStream;
        textStream << "pathFound: " << boolAsText(pathFound);
        gameText.addDebugMessage(textStream.str(), 1);
    }
//bool hasMoved
    if (hasMoved) {
        std::stringstream textStream;
        textStream << "hasMoved: " << boolAsText(hasMoved);
        gameText.addDebugMessage(textStream.str(), 0);
    }
    else {
        std::stringstream textStream;
        textStream << "hasMoved: " << boolAsText(hasMoved);
        gameText.addDebugMessage(textStream.str(), 1);
    }
//bool busyWalking
    if (busyWalking) {
        std::stringstream textStream;
        textStream << "busyWalking: " << boolAsText(busyWalking);
        gameText.addDebugMessage(textStream.str(), 0);
    }
    else {
        std::stringstream textStream;
        textStream << "busyWalking: " << boolAsText(busyWalking);
        gameText.addDebugMessage(textStream.str(), 1);
    }
//bool realPath
    if (realPath) {
        std::stringstream textStream;
        textStream << "realPath: " << boolAsText(realPath);
        gameText.addDebugMessage(textStream.str(), 0);
    }
    else {
        std::stringstream textStream;
        textStream << "realPath: " << boolAsText(realPath);
        gameText.addDebugMessage(textStream.str(), 1);
    }
//bool isGatheringRecources
    if (isGatheringRecources) {
        std::stringstream textStream;
        textStream << "isGatheringRecources: " << boolAsText(isGatheringRecources);
        gameText.addDebugMessage(textStream.str(), 0);
    }
    else {
        std::stringstream textStream;
        textStream << "isGatheringRecources: " << boolAsText(isGatheringRecources);
        gameText.addDebugMessage(textStream.str(), 1);
    }
//bool isAtRecource
    if (isAtRecource) {
        std::stringstream textStream;
        textStream << "isAtRecource: " << boolAsText(isAtRecource);
        gameText.addDebugMessage(textStream.str(), 0);
    }
    else {
        std::stringstream textStream;
        textStream << "isAtRecource: " << boolAsText(isAtRecource);
        gameText.addDebugMessage(textStream.str(), 1);
    }
//bool carriesRecources
    if (carriesRecources) {
        std::stringstream textStream;
        textStream << "carriesRecources: " << boolAsText(carriesRecources);
        gameText.addDebugMessage(textStream.str(), 0);
    }
    else {
        std::stringstream textStream;
        textStream << "carriesRecources: " << boolAsText(carriesRecources);
        gameText.addDebugMessage(textStream.str(), 1);
    }
//bool isAtCarryCapacity
    if (isAtCarryCapacity) {
        std::stringstream textStream;
        textStream << "isAtCarryCapacity: " << boolAsText(isAtCarryCapacity);
        gameText.addDebugMessage(textStream.str(), 0);
    }
    else {
        std::stringstream textStream;
        textStream << "isAtCarryCapacity: " << boolAsText(isAtCarryCapacity);
        gameText.addDebugMessage(textStream.str(), 1);
    }
//bool hasToUnloadResource
    if (hasToUnloadResource) {
        std::stringstream textStream;
        textStream << "hasToUnloadResource: " << boolAsText(hasToUnloadResource);
        gameText.addDebugMessage(textStream.str(), 0);
    }
    else {
        std::stringstream textStream;
        textStream << "hasToUnloadResource: " << boolAsText(hasToUnloadResource);
        gameText.addDebugMessage(textStream.str(), 1);
    }
//bool isWalkingToUnloadingPoint
    if (isWalkingToUnloadingPoint) {
        std::stringstream textStream;
        textStream << "isWalkingToUnloadingPoint: " << boolAsText(isWalkingToUnloadingPoint);
        gameText.addDebugMessage(textStream.str(), 0);
    }
    else {
        std::stringstream textStream;
        textStream << "isWalkingToUnloadingPoint: " << boolAsText(isWalkingToUnloadingPoint);
        gameText.addDebugMessage(textStream.str(), 1);
    }
//bool reachedUnloadingPoint
    if (reachedUnloadingPoint) {
        std::stringstream textStream;
        textStream << "reachedUnloadingPoint: " << boolAsText(reachedUnloadingPoint);
        gameText.addDebugMessage(textStream.str(), 0);
    }
    else {
        std::stringstream textStream;
        textStream << "reachedUnloadingPoint: " << boolAsText(reachedUnloadingPoint);
        gameText.addDebugMessage(textStream.str(), 1);
    }
//bool hasUnloaded
    if (hasUnloaded) {
        std::stringstream textStream;
        textStream << "hasUnloaded: " << boolAsText(hasUnloaded);
        gameText.addDebugMessage(textStream.str(), 0);
    }
    else {
        std::stringstream textStream;
        textStream << "hasUnloaded: " << boolAsText(hasUnloaded);
        gameText.addDebugMessage(textStream.str(), 1);
    }
//bool isFindingAlternative
    if (isFindingAlternative) {
        std::stringstream textStream;
        textStream << "isFindingAlternative: " << boolAsText(isFindingAlternative);
        gameText.addDebugMessage(textStream.str(), 0);
    }
    else {
        std::stringstream textStream;
        textStream << "isFindingAlternative: " << boolAsText(isFindingAlternative);
        gameText.addDebugMessage(textStream.str(), 1);
    }
//bool isBuilding
    if (isBuilding) {
        std::stringstream textStream;
        textStream << "isBuilding: " << boolAsText(isBuilding);
        gameText.addDebugMessage(textStream.str(), 0);
    }
    else {
        std::stringstream textStream;
        textStream << "isBuilding: " << boolAsText(isBuilding);
        gameText.addDebugMessage(textStream.str(), 1);
    }
//bool isMeleeAttacking
    if (isMeleeAttacking) {
        std::stringstream textStream;
        textStream << "isMeleeAttacking: " << boolAsText(isMeleeAttacking);
        gameText.addDebugMessage(textStream.str(), 0);
    }
    else {
        std::stringstream textStream;
        textStream << "isMeleeAttacking: " << boolAsText(isMeleeAttacking);
        gameText.addDebugMessage(textStream.str(), 1);
    }
//bool isRangedAttacking
    if (isRangedAttacking) {
        std::stringstream textStream;
        textStream << "isRangedAttacking: " << boolAsText(isRangedAttacking);
        gameText.addDebugMessage(textStream.str(), 0);
    }
    else {
        std::stringstream textStream;
        textStream << "isRangedAttacking: " << boolAsText(isRangedAttacking);
        gameText.addDebugMessage(textStream.str(), 1);
    }
    if (wasAttackMove) {
        std::stringstream textStream;
        textStream << "wasAttackMove: " << boolAsText(wasAttackMove);
        gameText.addDebugMessage(textStream.str(), 0);
    }
    else {
        std::stringstream textStream;
        textStream << "wasAttackMove: " << boolAsText(wasAttackMove);
        gameText.addDebugMessage(textStream.str(), 1);
    }
   //current cords
    std::stringstream textStream1;
    textStream1 << "Current cords x: " << actorCords.x << " y: " << actorCords.y;
    gameText.addDebugMessage(textStream1.str(), 0);
    //currentGoal
    textStream1.str(std::string());
    textStream1 << "Short term goal cords x: " << actorGoal.x << " y: " << actorGoal.y;
    gameText.addDebugMessage(textStream1.str(), 0);
    //realGoal
    textStream1.str(std::string());
    textStream1 << "Long term goal cords x: " << actorRealGoal.x << " y: " << actorRealGoal.y;
    gameText.addDebugMessage(textStream1.str(), 0);
    //actorCommandGoal
    textStream1.str(std::string());
    textStream1 << "actorCommandGoal cords x: " << actorCommandGoal.x << " y: " << actorCommandGoal.y;
    gameText.addDebugMessage(textStream1.str(), 0);
    //actionPreformedOnTile;
    textStream1.str(std::string());
    textStream1 << "actionPreformedOnTile x:" << actionPreformedOnTile.x << " y: " << actionPreformedOnTile.y;
    gameText.addDebugMessage(textStream1.str(), 0);
    //isWalkingBackXYDrawOverride
    if (isWalkingBackXYDrawOverride.isActive) {
        std::stringstream textStream;
        textStream << "isWalkingBackXYDrawOverride: " << boolAsText(isWalkingBackXYDrawOverride.isActive);
        gameText.addDebugMessage(textStream.str(), 0);
        textStream1.str(std::string());
        textStream1 << "isWalkingBackXYDrawOverride x:" << isWalkingBackXYDrawOverride.newXY.x << " y: " << isWalkingBackXYDrawOverride.newXY.y;
        gameText.addDebugMessage(textStream1.str(), 0);
    }
    else {
        std::stringstream textStream;
        textStream << "isWalkingBackXYDrawOverride: " << boolAsText(isWalkingBackXYDrawOverride.isActive);
        gameText.addDebugMessage(textStream.str(), 1);
    }
}

void Actors::makeSurePathIsOnListToGetCalculated()
{
    //Route should have a path? Check if route still needs to be calculated
    bool actorInListForPathfinding = false;
    if (!listOfActorsWhoNeedAPath.empty()) {
        for (int i = 0; i < listOfActorsWhoNeedAPath.size(); i++) {
            if (listOfActorsWhoNeedAPath[i] == this->actorId) {
                actorInListForPathfinding = true;
            }
        }
    }
    if (!actorInListForPathfinding) {
        this->retryWalkingOrChangeGoal();
    }
}

void Actors::update()
{
    if (this->actorAlive)
    {
        if (this->isIdle) {
            doNextStackedCommand();
            makeSureActorIsOnTheMap();
        }
        else if (this->isWalkingToMiddleOfSquare) {
            walkBackAfterAbortedCommand(); //centers actor on the square again
        }
        else {
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
                    this->makeSurePathIsOnListToGetCalculated();
                }
                this->houseKeeping(); //makes sure the actor is only occuping one tile!
            }
            else {
                this->searchAltetnative();
            }
        }
    }
}

void Actors::searchAltetnative() {

    if (!this->routeNeedsPath) {
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

std::string Actors::getResources() const {
    std::stringstream output;
    output << "Wood: " << this->amountOfWood << " Food: " << this->amountOfFood << " Stone: " << this->amountOfStone << " Gold: " << this->amountOfGold;
    return output.str();
}

void Actors::getNewBuildingTileForSameBuilding() {
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

void Actors::doTaskIfNotWalking()
{
    bool chasing = false;
    if (!this->busyWalking) {
        if (this->isMeleeAttacking || this->isRangedAttacking) {
            chasing = this->chaseTarget();
        }
        if (this->pathFound && !this->route.empty())
        {
            this->walkToNextSquare();
        }
        else if (this->isMeleeAttacking && !chasing) {
            this->doMeleeDamage();
        }
        else if (this->isRangedAttacking && !chasing) {
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
            if (!chasing) {
                this->isIdle = true;
            }
            else if (this->wasAttackMove) {
                this->selectAndAttackNextTarget(30);
            }
        }
        if (!this->isMeleeAttacking && !this->isRangedAttacking) {
            selectAndAttackNextTarget(3);
        }
    }
}


void Actors::selectAndAttackNextTarget(int range) {
    
    struct foundActors {
        int id;
        double distance;
    };

    std::vector<foundActors> potentialTargets;
    for (Actor& actor : listOfActors) {
        if (actor.getTeam() != this->actorTeam) {
            potentialTargets.push_back({ actor.getActorId() , distEuclidean(this->actorCords.x, this->actorCords.y, actor.getActorCords().x, actor.getActorCords().y) });
        }
    }
    
    int idOfTarget = -1;
    double distanceToTarget = 0;
    for (foundActors& target : potentialTargets) {
        if (target.distance < distanceToTarget || distanceToTarget == 0) {
            idOfTarget = target.id;
            distanceToTarget = target.distance;
        }
    }

    if (distanceToTarget < range && idOfTarget != -1) {
        bool attackMove = this->wasAttackMove;
        this->updateGoal(listOfActors[idOfTarget].getActorCords(), 0);
        this->setIsDoingAttack(false);
        this->wasAttackMove = attackMove;
    }

    this->wasAttackMove = false;
}

void Actors::shootProjectile()
{
    if (this->timeStartedAction + this->timeBetweenShots < currentGame.getTime()) {
        this->timeStartedAction = currentGame.getTime();
        if (currentGame.occupiedByActorList[this->actionPreformedOnTile.x][this->actionPreformedOnTile.y].empty()) {
            projectile newProjectile(this->actorCords.x, this->actorCords.y, this->actionPreformedOnTile.x, this->actionPreformedOnTile.y, this->projectileType, this->rangedDamage, this->splashDamage, this->actorId);
            listOfProjectiles.push_back(newProjectile);
        }
        else {
            this->isRangedAttacking = false;
        }
    }
}

bool Actors::getIsIdle() const
{
    return this->isIdle;
}

bool Actors::getIsAlive() const
{
    return this->actorAlive;
}

cords Actors::getActorCords() const
{
    return this->actorCords;
}

void Actors::setIsDoingAttack(bool chasing)
{
    this->isRangedAttacking = this->doesRangedDamage;
    this->isMeleeAttacking  = !this->doesRangedDamage;
    this->actionPreformedOnTile = this->actorGoal;
    if (!chasing) {
        if (!currentGame.occupiedByActorList[this->actorGoal.x][this->actorGoal.y].empty()) {
            this->idOfTarget = currentGame.occupiedByActorList[this->actorGoal.x][this->actorGoal.y].front();
        }
        else if (currentGame.occupiedByBuildingList[this->actorGoal.x][this->actorGoal.y] != -1) {
            this->idOfTarget = -currentGame.occupiedByBuildingList[this->actorGoal.x][this->actorGoal.y];
        }
        else {
            std::cout << "Stopping attack" << std::endl;
            this->isMeleeAttacking = false;
            this->isRangedAttacking = false;
            this->isIdle = true;
            this->clearRoute();
            this->routeNeedsPath = false;
            this->actorGoal = this->actorCords;
        }
    }
}

void Actors::doMeleeDamage()
{
    if (this->timeStartedAction == 0.0f)
    {
        gameText.addNewMessage("We have engaged the Enemy!", 1);
    }
    if (currentGame.elapsedTimeMS - this->timeStartedAction > this->rateOfFire)
    {
        this->timeStartedAction = currentGame.elapsedTimeMS;
        if (!currentGame.occupiedByActorList[this->actionPreformedOnTile.x][this->actionPreformedOnTile.y].empty()) {
            if (listOfActors[currentGame.occupiedByActorList[this->actionPreformedOnTile.x][this->actionPreformedOnTile.y].front()].getTeam() != this->actorTeam)
            {
                listOfActors[currentGame.occupiedByActorList[this->actionPreformedOnTile.x][this->actionPreformedOnTile.y].front()].takeDamage(this->meleeDamage, this->actorId);
            }
        }
        else if (currentGame.occupiedByBuildingList[this->actionPreformedOnTile.x][this->actionPreformedOnTile.y] != -1)
        {
            if (listOfBuildings[currentGame.occupiedByBuildingList[this->actionPreformedOnTile.x][this->actionPreformedOnTile.y]].getTeam() != this->actorTeam)
            {
                listOfBuildings[currentGame.occupiedByBuildingList[this->actionPreformedOnTile.x][this->actionPreformedOnTile.y]].takeDamage(this->meleeDamage);
            }
        }
        else if(this->wasAttackMove) {
            this->selectAndAttackNextTarget(30);
        } else  {
            this->isMeleeAttacking = false;
            this->timeStartedAction = 0;
        }
    }
}

const std::list<cords>& Actors::getRejectedTargetsList() const
{
    return listOfTargetsToRejectUntilSuccesfullMovement;
}

void Actors::takeDamage(int amountOfDamage, int idOfAttacker)
{
    this->actorHealth -= amountOfDamage;
    if (this->actorHealth <= 0) {
        this->killActor();
    }
    this->fightOrFlight(idOfAttacker);
}

void Actors::killActor() {
    this->actorAlive = false;
    for (auto& rows : currentGame.occupiedByActorList)
    {
        for (auto& elem : rows)
        {
            elem.erase(std::remove(elem.begin(), elem.end(), this->actorId), elem.end());
        }
    }
}

int Actors::getType() const
{
    return this->actorType;
}

sf::IntRect Actors::getLastIntRect() const
{
    return this->lastIntRect;
}

std::string Actors::getNameOfActor() const
{
    switch (this->actorType)
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

void Actors::updateGoal(cords location, int waitTime)
{
    if (location.x >= 0 && location.x < MAP_WIDTH && location.y >= 0 && location.y < MAP_HEIGHT &&
        !(location.x == this->actorCommandGoal.x && location.y == this->actorCommandGoal.y)) //no second update after double click
    {
        if (this->busyWalking) {
            if (!(this->actorGoal.x == this->actorCommandGoal.x && this->actorGoal.y == this->actorCommandGoal.y) ||
                route.empty())
            {
                if (
                    (this->actorCords.x - this->actorGoal.x == 1 || this->actorCords.x - this->actorGoal.x == 0 || this->actorCords.x - this->actorGoal.x == -1) &&
                    (this->actorCords.y - this->actorGoal.y == 1 || this->actorCords.y - this->actorGoal.y == 0 || this->actorCords.y - this->actorGoal.y == -1)
                    ) 
                {
                    this->isWalkingToMiddleOfSquare = true;
                    this->dataOnPositionAbortedWalk = {
                        currentGame.getTime() - this->timeLastUpdate,
                        currentGame.getTime(),
                        worldSpace({ this->actorCords.x,this->actorCords.y }),
                        worldSpace({ this->actorGoal.x, this->actorGoal.y }),
                        this->timeToCrossOneTile
                    };
                }
            }
        }
        if (this->buildingId != -1)
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
        this->timeStartedAction = 0;
        this->isFindingAlternative = false;
        this->isIdle = false;
        this->lastTile = false;
        this->wasAttackMove = false;
        this->walkingToResource = 0;
    }
}

void Actors::updateGoalPath()
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

void Actors::moveActorIfWalking()
{
    if (this->busyWalking && (currentGame.elapsedTimeMS - this->timeLastUpdate) > this->timeToCrossOneTile)
    {
        this->busyWalking = false;
        currentGame.occupiedByActorList[this->actorCords.x][this->actorCords.y].erase(std::remove(currentGame.occupiedByActorList[this->actorCords.x][this->actorCords.y].begin(), currentGame.occupiedByActorList[this->actorCords.x][this->actorCords.y].end(), this->actorId), currentGame.occupiedByActorList[this->actorCords.x][this->actorCords.y].end());
        this->actorCords = this->actorGoal;
    }
}

void Actors::clearRoute()
{
    this->pathFound = false;
    this->commonGoal = false;
    this->route.clear();
    if (this->isWalkingToUnloadingPoint)
    {
        this->reachedUnloadingPoint = true;
    }
}

void Actors::startWalking()
{
    this->hasMoved = true;
    this->retries = 0;
    this->busyWalking = true;
    this->timeLastUpdate = currentGame.elapsedTimeMS;
    this->actorGoal = this->route.back().position;

    currentGame.occupiedByActorList[this->route.back().position.x][this->route.back().position.y].push_back(this->actorId);

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

void Actors::retryWalkingOrChangeGoal() {
    if (this->timeLastRetry + 500 < currentGame.getTime()) {
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

void Actors::walkToNextSquare() {
    this->lastTile = false;
    // Deze actor heeft een doel, dit doel is nog niet bereikt en is klaar met het vorige stuk lopen!
    if (actorHelper::actorOrientation(this->actorCords.x, this->actorCords.y, this->route.back().position.x, this->route.back().position.y) == this->orientation)
    {
        if ((this->isGatheringRecources || this->isMeleeAttacking) && this->route.size() == 1) {
            //if (!this->isWalkingToUnloadingPoint) {
            this->clearRoute();
        }
        else if ((this->isGatheringRecources || this->isMeleeAttacking) && this->route.size() == 2) {
            if (!currentGame.occupiedByActorList[this->route.back().position.x][this->route.back().position.y].empty()) {
                this->lastTile = true;
                this->retryWalkingOrChangeGoal();
            }
            else {
                this->listOfTargetsToRejectUntilSuccesfullMovement.clear();
                this->startWalking();
            }
        }
        else if (this->isRangedAttacking && distEuclidean(this->actorCords.x, this->actorCords.y, this->actorGoal.x, this->actorGoal.y) <= this->range)
        {
            this->clearRoute();
        }
        else {
            //De actor staat met zijn neus de goede kant op en kan dus gaan lope
            if (this->route.size() == 1) {
                //De laatste tegel van een route moet wel vrij zijn
                if (!currentGame.occupiedByActorList[this->route.back().position.x][this->route.back().position.y].empty()) {
                    this->lastTile = true;
                    this->retryWalkingOrChangeGoal();
                }
                else {
                    this->listOfTargetsToRejectUntilSuccesfullMovement.clear();
                    this->startWalking();
                }
            }
            else {
                this->listOfTargetsToRejectUntilSuccesfullMovement.clear();
                this->startWalking();
            }
        }
    }
    else
    {
        //De actor moet eerst draaien voordat hij kan gaan lopen
        this->orientation = actorHelper::newOrientation(this->orientation, actorHelper::actorOrientation(this->actorCords.x, this->actorCords.y, this->route.back().position.x, this->route.back().position.y));
    }
}

void Actors::handleResourceGathering()
{
    if (this->hasToUnloadResource)
    {
        if (!this->isBackAtOwnSquare)
        {
            if (this->timeStartedWalkingToRecource == 0)
            {
                this->timeStartedWalkingToRecource = currentGame.elapsedTimeMS;
                this->walkingToResource = 2;
            }
            else if (currentGame.elapsedTimeMS - this->timeStartedWalkingToRecource < 500)
            {
                this->walkingToResource = 2;
            }
            else {
                this->walkingToResource = 0;
                this->isBackAtOwnSquare = true;
                this->isAtRecource = false;
                this->isBuilding = false;
                this->timeStartedWalkingToRecource = 0;
                this->timeStartedAction = currentGame.elapsedTimeMS;
                this->offSetX = 0;
                this->offSetY = 0;
            }
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
            else if (this->timeStartedWalkingToRecource == 0)
            {
                this->timeStartedWalkingToRecource = currentGame.elapsedTimeMS;
            }
            else if (currentGame.elapsedTimeMS - this->timeStartedWalkingToRecource < 500)
            {
                this->walkingToResource = 1;
            }
            else
            {
                this->walkingToResource = 0;
                this->startGatheringAnimation();
            }
        }
    }
}

void Actors::handleBuilding()
{
    if (this->realPath) {
        if (this->orientation == actorHelper::actorOrientation(this->actorCords.x, this->actorCords.y, this->actionPreformedOnTile.x, this->actionPreformedOnTile.y)) {
            //villager is aangekomen bij te bouwen gebouw en kan na verplaatst te zijn gaan bouwen!
            if (this->isAtRecource)
            {
                this->buildBuilding();
            }
            else if (this->timeStartedWalkingToRecource == 0)
            {
                this->timeStartedWalkingToRecource = currentGame.elapsedTimeMS;
            }
            else if (currentGame.elapsedTimeMS - this->timeStartedWalkingToRecource < 500)
            {
                this->walkingToResource = 1;
            }
            else
            {
                this->walkingToResource = 0;
                this->startGatheringAnimation();
            }
        }
        else {
            //eerst de actor in de juiste orientatie zetten
            this->orientation = actorHelper::newOrientation(this->orientation, actorHelper::actorOrientation(this->actorCords.x, this->actorCords.y, this->actionPreformedOnTile.x, this->actionPreformedOnTile.y));
        }
    }
    else {
        this->getNewBuildingTileForSameBuilding();
    }
}

void Actors::houseKeeping()
{
    if (!this->busyWalking && this->route.empty() && this->hasMoved)
    {
        this->cleanUp();
        this->hasMoved = false;
    }
}

void Actors::walkBackToOwnSquare()
{
    float northSouth;
    float eastWest;
    float diagonalX;
    float diagonalY;
    if (this->ResourceBeingGatherd == resourceTypes::resourceWood)
    {
        northSouth = 22.f;
        eastWest = 55.f;
        diagonalX = 21.f;
        diagonalY = 12.f;
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
    switch (this->orientation)
    {
    case 0:
        this->offSetX = 0;
        this->offSetY = -northSouth + ((static_cast<float>( currentGame.elapsedTimeMS) - static_cast<float>(this->timeStartedWalkingToRecource)) / 500.f) * northSouth;
        break;
    case 1:
        this->offSetX = diagonalX - ((static_cast<float>(currentGame.elapsedTimeMS) - static_cast<float>(this->timeStartedWalkingToRecource)) / 500.f) * diagonalX;
        this->offSetY = -diagonalY + ((static_cast<float>(currentGame.elapsedTimeMS) - static_cast<float>(this->timeStartedWalkingToRecource)) / 500.f) * diagonalY;
        break;
    case 2:
        this->offSetX = eastWest - ((static_cast<float>(currentGame.elapsedTimeMS) - static_cast<float>(this->timeStartedWalkingToRecource)) / 500.f) * eastWest;
        this->offSetY = 0;
        break;
    case 3:
        this->offSetX = diagonalX - ((static_cast<float>(currentGame.elapsedTimeMS) - static_cast<float>(this->timeStartedWalkingToRecource)) / 500.f) * diagonalX;
        this->offSetY = diagonalY - ((static_cast<float>(currentGame.elapsedTimeMS) - static_cast<float>(this->timeStartedWalkingToRecource)) / 500.f) * diagonalY;
        break;
    case 4:
        this->offSetX = 0;
        this->offSetY = northSouth - ((static_cast<float>(currentGame.elapsedTimeMS) - static_cast<float>(this->timeStartedWalkingToRecource)) / 500.f) * northSouth;
        break;
    case 5:
        this->offSetX = -diagonalX + ((static_cast<float>(currentGame.elapsedTimeMS) - static_cast<float>(this->timeStartedWalkingToRecource)) / 500.f) * diagonalX;
        this->offSetY = diagonalY - ((static_cast<float>(currentGame.elapsedTimeMS) - static_cast<float>(this->timeStartedWalkingToRecource)) / 500.f) * diagonalY;
        break;
    case 6:
        this->offSetX = -eastWest + ((static_cast<float>(currentGame.elapsedTimeMS) - static_cast<float>(this->timeStartedWalkingToRecource)) / 500.f) * eastWest;
        this->offSetY = 0;
        break;
    case 7:
        this->offSetX = -diagonalX + ((static_cast<float>(currentGame.elapsedTimeMS) - static_cast<float>(this->timeStartedWalkingToRecource)) / 500.f) * diagonalX;
        this->offSetY = -diagonalY + ((static_cast<float>(currentGame.elapsedTimeMS) - static_cast<float>(this->timeStartedWalkingToRecource)) / 500.f) * diagonalY;
        break;
    }
}

void Actors::startGatheringAnimation()
{
    int northSouth;
    int eastWest;
    int diagonalX;
    int diagonalY;
    if (this->ResourceBeingGatherd == resourceTypes::resourceWood)
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
    this->timeStartedWalkingToRecource = 0;
    this->timeStartedAction = currentGame.elapsedTimeMS;
    switch (this->orientation)
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

void Actors::animateWalkingToResource()
{
    float northSouth;
    float eastWest;
    float diagonalX;
    float diagonalY;
    if (this->ResourceBeingGatherd == resourceTypes::resourceWood)
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
    switch (this->orientation)
    {
    case 0:
        this->offSetX = 0;
        this->offSetY = ((static_cast<float>(currentGame.elapsedTimeMS) - static_cast<float>(this->timeStartedWalkingToRecource)) / 500.f) * northSouth * -1.f;
        break;
    case 1:
        this->offSetX = ((static_cast<float>(currentGame.elapsedTimeMS) - static_cast<float>(this->timeStartedWalkingToRecource)) / 500.f) * diagonalX;
        this->offSetY = ((static_cast<float>(currentGame.elapsedTimeMS) - static_cast<float>(this->timeStartedWalkingToRecource)) / 500.f) * diagonalY * -1.f;
        break;
    case 2:
        this->offSetX = ((static_cast<float>(currentGame.elapsedTimeMS) - static_cast<float>(this->timeStartedWalkingToRecource)) / 500.f) * eastWest;
        this->offSetY = 0;
        break;
    case 3:
        this->offSetX = ((static_cast<float>(currentGame.elapsedTimeMS) - static_cast<float>(this->timeStartedWalkingToRecource)) / 500.f) * diagonalX;
        this->offSetY = ((static_cast<float>(currentGame.elapsedTimeMS) - static_cast<float>(this->timeStartedWalkingToRecource)) / 500.f) * diagonalY;
        break;
    case 4:
        this->offSetX = 0;
        this->offSetY = ((static_cast<float>(currentGame.elapsedTimeMS) - static_cast<float>(this->timeStartedWalkingToRecource)) / 500.f) * northSouth;
        break;
    case 5:
        this->offSetX = ((static_cast<float>(currentGame.elapsedTimeMS) - static_cast<float>(this->timeStartedWalkingToRecource)) / 500.f) * diagonalX * -1.f;
        this->offSetY = ((static_cast<float>(currentGame.elapsedTimeMS) - static_cast<float>(this->timeStartedWalkingToRecource)) / 500.f) * diagonalY;
        break;
    case 6:
        this->offSetX = ((static_cast<float>(currentGame.elapsedTimeMS) - static_cast<float>(this->timeStartedWalkingToRecource)) / 500.f) * eastWest * -1.f;
        this->offSetY = 0;
        break;
    case 7:
        this->offSetX = ((static_cast<float>(currentGame.elapsedTimeMS) - static_cast<float>(this->timeStartedWalkingToRecource)) / 500.f) * diagonalX * -1.f;
        this->offSetY = ((static_cast<float>(currentGame.elapsedTimeMS) - static_cast<float>(this->timeStartedWalkingToRecource)) / 500.f) * diagonalY * -1.f;
        break;
    }
}

void Actors::gatherResource()
{
    if (actorHelper::isReallyNextToResource(this->actorCords.x, this->actorCords.y, this->actionPreformedOnTile.x, this->actionPreformedOnTile.y)) {
        if (currentGame.objectLocationList[this->actionPreformedOnTile.x][this->actionPreformedOnTile.y] != -1)
        {
            if (currentGame.elapsedTimeMS - this->timeStartedAction > 2000)
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
                    this->isBackAtOwnSquare = false;
                    this->isAtRecource = false;
                    this->realPath = false;
                }
                this->timeStartedAction = currentGame.elapsedTimeMS;
            }
        }
        else
        {
            //resource not here!
            std::stringstream error;
            error << "Resources no longer here! Going back to base.";
            gameText.addNewMessage(error.str(), 1);

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

void Actors::unloadAndReturnToGathering()
{
    switch (listOfBuildings[this->dropOffTile.buildingId].getRecievesWhichResources())
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
            this->timeStartedWalkingToRecource = 0;
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
            this->timeStartedWalkingToRecource = 0;
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
        this->timeStartedWalkingToRecource = 0;
        this->isFindingAlternative = true;
    }

}

void Actors::setGatheringRecource(bool flag)
{
    this->isGatheringRecources = flag;
    this->isBuilding = false;
    if (flag && currentGame.objectLocationList[this->actorGoal.x][this->actorGoal.y] != -1)
    {
        this->ResourceBeingGatherd = listOfObjects[currentGame.objectLocationList[this->actorGoal.x][this->actorGoal.y]].getTypeOfResource();
        this->actionPreformedOnTile = this->actorGoal;
    }
}

void Actors::setCommonGoalTrue()
{
    this->commonGoal = true;
}

void Actors::setDoAttackMoveTrue()
{
    this->wasAttackMove = true;
}

void Actors::findNearestSimilarResource()
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

void Actors::findNearestDropOffPoint()
{
    if (!this->routeNeedsPath) {
        //Bugfix for edge case where the actor is ocuping a tile adjacent to a dropoff building
        int adjacentBuildingAvailableId = actorHelper::adjacentTileIsCorrectDropOffPoint(this->actorCords, this->ResourceBeingGatherd, this->actorTeam);
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
                if (this->buildingId != -1) {
                    listOfBuildings[this->buildingId].removeActorFromBuildingTile(this->actorId);
                }
                this->buildingId = listOfDropOffLocations.front().buildingId;
                listOfBuildings[this->buildingId].claimFreeBuiildingTile(listOfDropOffLocations.front().tileId, this->actorId);
                listOfDropOffLocations.pop_front();
            }
        }
    }
}

int Actors::getTeam() const
{
    return this->actorTeam;
}

bool Actors::getIsInitialized() const
{
    return this->initialized;
}

void Actors::calculateRoute()
{
    if (this->routeNeedsPath)
    {
        this->route.clear();
        this->actorRealGoal = this->actorGoal;
        this->realPath = false;
        this->routeNeedsPath = false;
        this->pathAStar();
        if (!this->realPath && this->pathFound) {
            this->timeLastPathTry = currentGame.elapsedTimeMS;
        }
    }
    else {
        std::cout << "Route did not need a path?" << std::endl;
    }
}

void Actors::pathAStar()
{
    std::vector<Cells> cellsList = baseCellList;
    int startCell = (actorCords.x * MAP_HEIGHT) + actorCords.y; //eigen positie
    int endCell = (actorGoal.x * MAP_HEIGHT) + actorGoal.y; //doel positie
    updateCells(endCell, startCell, cellsList, this->lastTile);
    std::vector<Cells*> listToCheck;
    std::vector<Cells*> checkedList;
    bool endReached = false;

    //check of de doelcel niet 1 hokje weg is 
    if (((actorCords.x - actorGoal.x == 0) || (actorCords.x - actorGoal.x == -1) || (actorCords.x - actorGoal.x == 1)) && ((actorCords.y - actorGoal.y == 0) || (actorCords.y - actorGoal.y == -1) || (actorCords.y - actorGoal.y == 1)))
    {
        //en geen obstakel is of het hokje van de actor zelf is
        if (!cellsList[endCell].obstacle || (actorCords.x == actorGoal.x && actorCords.y == actorGoal.y))
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
        cellsList[startCell].parentCellId = -1;
        cellsList[startCell].costToGoal = dist(cellsList[startCell].position.x, cellsList[startCell].position.y, cellsList[endCell].position.x, cellsList[endCell].position.y);
        //De startcel in de lijst van te checken cellen zetten om te kunnen beginnen
        listToCheck.push_back(&cellsList[startCell]);
    }

    while (!listToCheck.empty() && startCell != endCell)
    {
        int cellId = 0;
        double lowestCostGuess = (*listToCheck.front()).totalCostGuess;
        int iterator = 0;
        for (Cells* cell : listToCheck) {
            if (cell->totalCostGuess < lowestCostGuess) {
                lowestCostGuess = cell->totalCostGuess;
                cellId = iterator;
            }
            iterator++;
        }

        //Check of de te checken cell het doel is. Als dat zo is zijn we klaar
        if ((*listToCheck[cellId]).cellId == endCell)
        {
            listToCheck.clear();
            this->pathFound = true;
        }
        else if (!listToCheck.empty())
        {
            //Loop door de lijst van "buurcellen van de te checken cell"
            for (std::vector<int>::const_iterator iterator = (*listToCheck[cellId]).neighbours.begin(), end = (*listToCheck[cellId]).neighbours.end(); iterator != end; ++iterator)
            {
                //Kijk of deze cell eerder bezocht is
                if (!cellsList[*iterator].visited)
                {
                    //Deze cell heeft geen parent is is dus nooit eerder gevonden! De buren moeten dus toegevoegd worden!
                    cellsList[*iterator].addNeighbours(cellsList);
                    //De cell waarvan we de neighbours onderzoeken is uiteraard tot nu toe de kortste route hiernaartoe
                    cellsList[*iterator].parentCellId = (*listToCheck[cellId]).cellId;
                    //Nu moeten de kosten voor de route hiernatoe uitgerekend worden (Dit zijn de kosten van naar de parent gaan +1)
                    cellsList[*iterator].cummulativeCost = (*listToCheck[cellId]).cummulativeCost + 1;
                    //Als laatste zetten we de cell in de lijst met cellen die gecheckt moet worden
                    listToCheck.push_back(&cellsList[*iterator]);
                    //En we schatten vanaf deze cell de kosten naar het doel
                    cellsList[*iterator].costToGoal = dist(cellsList[*iterator].position.x, cellsList[*iterator].position.y, cellsList[endCell].position.x, cellsList[endCell].position.y);
                    //Waardoor we dus de totale kosten kunnen schatten
                    cellsList[*iterator].totalCostGuess = cellsList[*iterator].costToGoal + cellsList[*iterator].cummulativeCost;
                    //We zijn voor nu klaar met deze tegel en laten een vlag achter dat de tegel al eens bekeken is
                    cellsList[*iterator].visited = true;
                }
                else
                {
                    //Deze tegel is al eerder gevonden, de buren staan dus al in de te checken cell lijst en hoeft niet opnieuw gechecked te worden
                    //We moeten wel weten of de route waarmee deze tegel nu is aangedaan niet korter is dan een eerder gevonden route
                    if ((*listToCheck[cellId]).cummulativeCost + 1 < cellsList[*iterator].cummulativeCost)
                    {
                        //Er is een kortere route naar deze cell! Pas de parent cell dus aan en geef een nieuwe cummulative Cost;
                        cellsList[*iterator].parentCellId = (*listToCheck[cellId]).cellId;
                        cellsList[*iterator].cummulativeCost = (*listToCheck[cellId]).cummulativeCost + 1;
                        //Uiteraard verranderd dit dan ook de geschatte totale kosten vanaf deze tegel
                        cellsList[*iterator].totalCostGuess = cellsList[*iterator].costToGoal + cellsList[*iterator].cummulativeCost;
                    }
                }
            }
            //zet de tegel op de bezochte tegellijst
            checkedList.push_back(&cellsList[(*listToCheck[cellId]).cellId]);
            //en haal hem van de te bezoeken tegellijst
            listToCheck.erase(std::next(listToCheck.begin(), cellId));
        }
    }
    //Alle tegels die te bereiken zijn zijn bekeken of er is een route gevonden.
    //Maak een route door de parent cells achter elkaar te zetten tot de begin cell
    this->route.clear();
    if (this->pathFound)
    {
        //Er is een pad naar het doel gevonden! Stippel de route maar uit!
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
                //Fix voor een bug waarin op een of andere manier géén parent cell ingevult staat en dus null zou zijn. (al zou dat niet moeten kunnen)
                if (this->route.back().parentCellId == 0) {
                    //We gaan kijken of het een logische buur kan zijn, gelukkig hoeven we maar drie posities te controleren:
                    if (this->route.back().position.x == 0 && this->route.back().position.x == 1) {
                        //Logische parent; doe niets!
                    }
                    else if (this->route.back().position.x == 0 && this->route.back().position.x == 1) {
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
        this->realPath = true; //Vlaggetje dat de route naar het eindpunt gaat en er in princiepe geen herbereking nodig is
    }
    else {
        //Check of de actor niet naar een opdracht moet lopen. Is dat het geval wil ik niet naar de dichtsbijzijnde tegel lopen maar de actor een nieuw doel laten kiezen
        if (!(this->isGatheringRecources || this->isBuilding)) {
            //We kunnen niet bij het doel komen en hebben geen bouw of verzamel opdracht, dan gaan we maar naar de geschatte dichtsbijzijnde cell waar we naartoe kunnen!
            if (!checkedList.empty()) {
                std::sort(checkedList.begin(), checkedList.end(), ([](const Cells* f, const Cells* s)
                    {
                        return f->costToGoal < s->costToGoal;
                    }));
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

void Actors::clearCommandStack()
{
    this->listOfOrders.clear();
}


void Actors::fightOrFlight(int idOfAttacker)
{
    auto moveOrAttackIfNotPossible = [&](cords moveAwayFrom) {
        int moveX = 0;
        int moveY = 0;
        if (this->actorCords.x < moveAwayFrom.x) {
            moveX = -1;
        }
        else if (this->actorCords.x > moveAwayFrom.x) {
            moveX = 1;
        }
        if (this->actorCords.x < moveAwayFrom.y) {
            moveY = -1;
        }
        else if (this->actorCords.x > moveAwayFrom.y) {
            moveY = 1;
        }

        if (currentGame.isPassable({ this->actorCords.x + moveX, this->actorCords.y + moveY })) {
            this->updateGoal({ this->actorCords.x + moveX, this->actorCords.y + moveY }, 0);
        }
        else if (currentGame.isPassable({ this->actorCords.x + moveX, this->actorCords.y })) {
            this->updateGoal({ this->actorCords.x + moveX, this->actorCords.y }, 0);
        }
        else if (currentGame.isPassable({ this->actorCords.x, this->actorCords.y + moveY })) {
            this->updateGoal({ this->actorCords.x, this->actorCords.y + moveY }, 0);
        }
        else if (currentGame.isPassable({ this->actorCords.x - moveX, this->actorCords.y + moveY })) {
            this->updateGoal({ this->actorCords.x - moveX, this->actorCords.y + moveY }, 0);
        }
        else if (currentGame.isPassable({ this->actorCords.x + moveX, this->actorCords.y - moveY })) {
            this->updateGoal({ this->actorCords.x + moveX, this->actorCords.y - moveY }, 0);
        }
        else {
            this->updateGoal(listOfActors[idOfAttacker].getActorCords(), 0);
            this->setIsDoingAttack(false);
        }
    };


    if (!this->isMeleeAttacking && !this->isRangedAttacking) {
        if (idOfAttacker >= 0) {
            int hitsUntilDead = 0;
            int hitsToWin = 0;
            if (listOfActors[idOfAttacker].getMeleeDMG() > listOfActors[idOfAttacker].getRangedDMG()) {
                hitsUntilDead = this->actorHealth / listOfActors[idOfAttacker].getMeleeDMG();
            }
            else {
                hitsUntilDead = this->actorHealth / listOfActors[idOfAttacker].getRangedDMG();
            }
            if (this->meleeDamage > this->rangedDamage) {
                hitsToWin = listOfActors[idOfAttacker].getHealth().first / this->meleeDamage;
            }
            else {
                hitsToWin = listOfActors[idOfAttacker].getHealth().first / this->rangedDamage;
            }
            if (hitsToWin < hitsUntilDead) {
                this->updateGoal(listOfActors[idOfAttacker].getActorCords(), 0);
                this->setIsDoingAttack(false);
            }
            else {
                moveOrAttackIfNotPossible(listOfActors[idOfAttacker].getActorCords());
            }
        }
        else {
            moveOrAttackIfNotPossible(listOfBuildings[-idOfAttacker].getLocation());
        }
    }
}

void Actors::doNextStackedCommand()
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
            this->setIsDoingAttack(false);
            break;
        }
        listOfOrders.pop_front();
    }
}

void Actors::stackOrder(cords goal, const stackOrderTypes orderType)
{
    this->listOfOrders.push_back({ goal, orderType });
}

void Actors::routing(std::vector<Cells>& cellsList, int endCell, int startCell, bool endReached)
{
    //Zet de tegel waarnaartoe gelopen wordt in de lijst
    
}

void Actors::cleanUp()
{
    for (int i = 0; i < MAP_WIDTH; i++)
    {
        for (int j = 0; j < MAP_HEIGHT; j++)
        {
            currentGame.occupiedByActorList[i][j].erase(std::remove(currentGame.occupiedByActorList[i][j].begin(), currentGame.occupiedByActorList[i][j].end(), this->actorId), currentGame.occupiedByActorList[i][j].end());
        }
    }
    currentGame.occupiedByActorList[this->actorCords.x][this->actorCords.y].push_back(this->actorId);
}

std::pair<int, int> Actors::getHealth() const
{
    return { this->actorHealth, this->hitPoints };
}

int Actors::getMeleeDMG() const
{
    return this->meleeDamage;
}

int Actors::getRangedDMG() const
{
    return this->rangedDamage;
}

void Actors::drawActor()
{
    int i = this->actorCords.x;
    int j = this->actorCords.y;
    int x = this->actorGoal.x;
    int y = this->actorGoal.y;
    cords position = worldSpace({ i,j });
    int spriteXoffset = 0;
    int spriteYoffset = 0;

   switch (this->walkingToResource) {
   case 0:
       break;
   case 1:
       this->animateWalkingToResource();
       break;
   case 2:
       this->walkBackToOwnSquare();
       break;
    }

    if (this->busyWalking || this->timeStartedWalkingToRecource > 0)
    {
        if (currentGame.elapsedTimeMS - this->timeLastOffsetChange > 200)
        {
            this->spriteYOffset = this->spriteYOffset + 32;
            if (this->spriteYOffset > 128)
            {
                this->spriteYOffset = 32;
            }
            this->timeLastOffsetChange = currentGame.elapsedTimeMS;
        }
        if (this->busyWalking)
        {
            cords nPosition = worldSpace({ x, y });
            int deltaX = position.x - nPosition.x;
            int deltaY = position.y - nPosition.y;
            float deltaTime = static_cast<float>(currentGame.elapsedTimeMS) - static_cast<float>(this->timeLastUpdate);
            float speedMultiplier = 1.f / this->timeToCrossOneTile;
            float deltaXCompleted = deltaX * (deltaTime * speedMultiplier);
            float deltaYCompleted = deltaY * (deltaTime * speedMultiplier);
            position = { position.x - static_cast<int>(deltaXCompleted),  position.y - static_cast<int>(deltaYCompleted) };
        }
    }
    else if (this->isAtRecource)
    {
        if (currentGame.elapsedTimeMS - this->timeLastOffsetChange > 200)
        {
            this->spriteYOffset = this->spriteYOffset + 32;
            if (this->spriteYOffset > 128)
            {
                this->spriteYOffset = 0;
            }
            this->timeLastOffsetChange = currentGame.elapsedTimeMS;
        }
    }
    else
    {
        this->spriteYOffset = 0;
    }
    switch (this->spriteYOffset)
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
    if (this->isAtRecource)
    {
        if (this->ResourceBeingGatherd == resourceTypes::resourceWood)
        {
            spriteOffset = 128;
        }
        else if (this->ResourceBeingGatherd == resourceTypes::resourceFood || this->ResourceBeingGatherd == resourceTypes::resourceStone)
        {
            spriteOffset = 256;
        }
        else if (this->ResourceBeingGatherd == resourceTypes::resourceGold)
        {
            spriteOffset = 384;
        }
    }
    switch (this->orientation)
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

    if (this->isWalkingBackXYDrawOverride.isActive) {
        anitmateWalkingBackAfterAbortedCommand();
        position = this->isWalkingBackXYDrawOverride.newXY;
    }

    bool drawHealth = false;

    //determine if actor is the only actor on the tile
    if (currentGame.occupiedByActorList[this->actorCords.x][this->actorCords.y].size() > 1) {
        //Tile is occupied by more then one actor!
        int id = 0;
        for (auto& actorId : currentGame.occupiedByActorList[this->actorCords.x][this->actorCords.y]) {
            if (actorId == this->actorId) {
                break;
            }
            id++;
        }
        //If the actor was occuping the cell before the other one let it step asside to let the other pass
        if (id == 0) {
            position.x = position.x - 7;
        }
    }

    if (currentGame.isInSelectedActors(this->actorId))
    {
        currentGame.spriteUnitSelectedTile.setPosition(static_cast<float>(position.x), static_cast<float>(position.y));
        window.draw(currentGame.spriteUnitSelectedTile);
        drawHealth = true;
        if (!this->listOfOrders.empty() && this->actorTeam == listOfPlayers[currentPlayerI].getTeam()) {
            for (const orderStack order : this->listOfOrders) {
                currentGame.listOfFlagsToDraw.push_back(order.goal);
            }
        }
    }
    else if (this->actorHealth < this->hitPoints) {
        drawHealth = true;
    }


    //Draw the actor
    listOfActorTemplates[this->actorType].setSpritePosition(position);
    listOfActorTemplates[this->actorType].getActorSprite().setTextureRect(sf::IntRect(spriteXoffset, spriteYoffset, 16, 32));
    window.draw(listOfActorTemplates[this->actorType].getActorSprite());

    //Set last intRect for selection handeling

    this->lastIntRect = static_cast<sf::IntRect>(listOfActorTemplates[this->actorType].getActorSprite().getGlobalBounds());


    if (drawHealth) {
        currentGame.healthBarBackground.setFillColor(sf::Color(255, 0, 0));
        currentGame.healthBarBackground.setSize(sf::Vector2f(32, 3));
        currentGame.healthBarGreenBar.setFillColor(sf::Color(0, 255, 0));
        currentGame.healthBarGreenBar.setSize(sf::Vector2f((static_cast<float>(this->actorHealth) / static_cast<float>(this->hitPoints)) * 32, 3));
        currentGame.healthBarBackground.setPosition(static_cast<float>(position.x + 16), static_cast<float>(position.y - 28));
        currentGame.healthBarGreenBar.setPosition(static_cast<float>(position.x + 16), static_cast<float>(position.y - 28));
        window.draw(currentGame.healthBarBackground);
        window.draw(currentGame.healthBarGreenBar);

    }
}

void Actors::buildBuilding()
{
    if (currentGame.occupiedByBuildingList[this->actionPreformedOnTile.x][this->actionPreformedOnTile.y] != -1)
    {
        if (!listOfBuildings[currentGame.occupiedByBuildingList[this->actionPreformedOnTile.x][this->actionPreformedOnTile.y]].getCompleted())
        {
            if (currentGame.elapsedTimeMS - this->timeStartedAction > 1000)
            {
                listOfBuildings[currentGame.occupiedByBuildingList[this->actionPreformedOnTile.x][this->actionPreformedOnTile.y]].addBuildingPoint();
                this->timeStartedAction = currentGame.elapsedTimeMS;
            }
        }
        else
        {
            if (this->timeStartedWalkingToRecource == 0) {
                this->timeStartedWalkingToRecource = currentGame.elapsedTimeMS;
                this->walkingToResource = 2;
                this->isBackAtOwnSquare = false;
            } else if (currentGame.elapsedTimeMS - this->timeStartedWalkingToRecource < 500)
            {
                this->walkingToResource = 2;
            }
            else {
                this->walkingToResource = 0;
                this->isBackAtOwnSquare = true;
                this->isAtRecource = false;
                this->isBuilding = false;
                this->timeStartedWalkingToRecource = 0;
                this->timeStartedAction = currentGame.elapsedTimeMS;
                this->offSetX = 0;
                this->offSetY = 0;
            }
            listOfBuildings[this->buildingId].removeActorFromBuildingTile(this->actorId);
        }
    }
    else
    {
        //het gebouw is er niet meer
        if (currentGame.elapsedTimeMS - this->timeStartedWalkingToRecource < 500)
        {
            this->walkingToResource = 2;
        }
        else {
            this->walkingToResource = 0;
            this->isBackAtOwnSquare = true;
            this->isAtRecource = false;
            this->isBuilding = false;
            this->timeStartedWalkingToRecource = 0;
            this->timeStartedAction = currentGame.elapsedTimeMS;
            this->offSetX = 0;
            this->offSetY = 0;
        }
        listOfBuildings[this->buildingId].removeActorFromBuildingTile(this->actorId);
    }
}

int Actors::getActorId()  const
{
    return this->actorId;
}

bool Actors::getIsGathering() const
{
    return this->isGatheringRecources;
}

bool Actors::getIsBuilding() const
{
    return this->isBuilding;
}

bool Actors::getHasRoute()
{
    return !this->route.empty();
}

cords Actors::getGoal() const
{
    return this->actorGoal;
}

cords Actors::getEndGoal()
{
    if (!this->isGatheringRecources) {
        return { this->actorRealGoal.x, this->actorRealGoal.y };
    }
    else if(!route.empty()){
        if (route.size() > 1) {
            std::list<routeCell>::iterator secondCell = std::next(route.begin());
            return secondCell->position;
        }
        else {
            return route.front().position;
        }
    }
    else {
        return this->actorGoal;
    }

}

int Actors::getBuildingId() const
{
    return this->buildingId;
}

resourceTypes Actors::getResourceGathered() const
{
    return this->ResourceBeingGatherd;
}

void Actors::setIsBuildingTrue(int buildingId, cords goal)
{
    this->isBuilding = true;
    this->buildingId = buildingId;
    this->isGatheringRecources = false;
    this->ResourceBeingGatherd = resourceTypes::resourceFood;
    this->actionPreformedOnTile = goal;
}

void Actors::renderPath()
{
    std::list<routeCell>::iterator it;
    for (it = route.begin(); it != route.end(); it++)
    {
        currentGame.spriteSelectedTileForPath.setPosition(static_cast<float>(worldSpace(it->position).x), static_cast<float>(worldSpace(it->position).y));
        window.draw(currentGame.spriteSelectedTileForPath);
    }
}
