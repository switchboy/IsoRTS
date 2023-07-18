#ifndef ACTORS_H
#define ACTORS_H
#include <cmath>
#include <list>
#include <SFML/System.hpp>
#include "gamestate.h"
#include "globalfunctions.h"
#include "actorTemplates.h"


struct Cells
{
public:
    Cells() {
        position = { 0,0 };
        parentCellId = 0; 
        cummulativeCost = 0; 
        cellId = 0; 
        backParent = 0;
        costToGoal = 0;
        totalCostGuess = 0;
    }
    Cells(cords cellPosition, int cellId);
    Cells(cords cellPosition, int cellId, bool obstacle);
    cords position;
    int parentCellId, cummulativeCost, cellId, backParent;
    double costToGoal, totalCostGuess;
    bool visited = false;
    bool visitedBack = false;
    bool obstacle = false;
    std::vector<int> neighbours;
    void addNeighbours(const std::vector<Cells> &cellsList);
};

extern std::vector<Cells> baseCellList;

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
    float deltaXCompleted = static_cast<float>(startDeltaX) * ((static_cast<float>(this->timePassedSinceChangingOffset) / 1000.f ) * (static_cast<float>(this->speedMultiplier) / 1000.f));
    float deltaYCompleted = static_cast<float>(startDeltaY) * ((static_cast<float>(this->timePassedSinceChangingOffset) / 1000.f ) * (static_cast<float>(this->speedMultiplier) / 1000.f));
};

struct drawXYOverride {
    bool isActive;
    cords newXY;
};

class actors
{
public:
    actors(int type, cords location, int actorTeam, int actorId);
    
    void drawActor();
    void renderPath();

    void update();
   
    void buildBuilding();
    void calculateRoute();
    bool chaseTarget();
    void cleanUp();
    void clearCommandStack();
    void clearRoute();
    void doMeleeDamage();
    void doNextStackedCommand();
    void doTaskIfNotWalking();
    void fightOrFlight(int idOfAttacker);
    void findNearestDropOffPoint();
    void findNearestSimilarResource();
    void gatherResource();
    void getNewBuildingTileForSameBuilding();
    void handleBuilding();
    void handleResourceGathering();
    void houseKeeping();
    void killActor();
    void moveActorIfWalking();
    void makeSureActorIsOnTheMap();
    void printActorDebugText();
    void pathAStar();
    void selectAndAttackNextTarget(int range);
    void retryWalkingOrChangeGoal();
    void routing(std::vector<Cells>& cellsList, int endCell, int startCell, bool endReached);
    void searchAltetnative();
    void setCommonGoalTrue();
    void setDoAttackMoveTrue();
    void setGatheringRecource(bool flag);
    void shootProjectile();
    void stackOrder(cords Goal, stackOrderTypes orderType);
    void startGatheringAnimation();
    void startWalking();
    void takeDamage(int amountOfDamage, int idOfAttacker);
    void unloadAndReturnToGathering();
    void makeSurePathIsOnListToGetCalculated();
    void updateGoal(cords goal, int waitTime);
    void updateGoalPath();
    void walkBackToOwnSquare();
    void walkToNextSquare();
    void walkBackAfterAbortedCommand();
    void animateWalkingToResource();
    void anitmateWalkingBackAfterAbortedCommand();
    
    //getters
    bool getIsBuilding() const;
    bool getHasRoute();
    bool getIsIdle() const;
    bool getIsAlive() const;
    bool getIsGathering() const;
    bool getIsInitialized() const;
    int getActorId() const;
    int getBuildingId() const;
    int getMeleeDMG() const;
    int getRangedDMG() const;
    int getTeam() const;
    int getType() const;
    sf::IntRect getLastIntRect() const;
    cords getGoal() const;
    cords getEndGoal();
    resourceTypes getResourceGathered() const;
    cords getActorCords() const;
    const std::list<cords>& getRejectedTargetsList() const;
    std::pair<int, int> getHealth() const;
    std::string getNameOfActor() const;
    std::string getResources() const;

    //setters
    void setIsBuildingTrue(int buildingId, cords goal);
    void setIsDoingAttack(bool chasing);

private:
    bool isWalkingToMiddleOfSquare;
    bool lastTile;
    bool actorAlive;
    bool doesRangedDamage;
    bool busyWalking;
    bool commonGoal;
    bool isGatheringRecources;
    bool isAtRecource;
    bool isAtCarryCapacity;
    bool hasToUnloadResource;
    bool isWalkingToUnloadingPoint;
    bool reachedUnloadingPoint;
    bool hasUnloaded;
    bool carriesRecources;
    bool pathFound;
    bool goalNeedsUpdate;
    bool isBackAtOwnSquare;
    int walkingToResource = 0;
    bool noPathPossible;
    bool routeNeedsPath;
    bool initialized;
    bool isBuilding;
    bool hasMoved;
    bool isMeleeAttacking;
    bool isRangedAttacking;
    bool isFindingAlternative;
    bool isIdle;
    bool realPath;
    bool wasAttackMove;
    cords actorCords;
    cords actorGoal;
    cords actorRealGoal;
    cords actorCommandGoal;
    cords actionPreformedOnTile;
    int idOfTarget;
    int actorType;
    int actorTeam;
    int actorHealth;
    int buildingId;
    int actorId;
    int hitPoints;
    int meleeDamage;
    int rangedDamage;
    int range;
    resourceTypes ResourceBeingGatherd;
    int amountOfGold;
    int amountOfWood;
    int amountOfStone;
    int amountOfFood;
    int orientation;
    int currentFrame;
    int spriteYOffset;
    int retries;
    int splashDamage;
    int projectileType;
    int waitForAmountOfFrames;
    int rateOfFire;
    float offSetX;
    float offSetY;


    int lastChaseTime = 0;
    int timePassedSinceChangingOffset;
    int timeStartedGatheringRecource;
    int timeLastOffsetChange;
    int timeLastUpdate;
    int timeLastPathTry;
    int timeStartedWalkingToRecource;
    int timeLastAttempt;

    int timeBetweenShots;
    int timeLastRetry;
    int timeToCrossOneTile;
    drawXYOverride isWalkingBackXYDrawOverride;
    unfinischedWalking dataOnPositionAbortedWalk;
    nearestBuildingTile dropOffTile;
    std::list<cords> listOfTargetsToRejectUntilSuccesfullMovement;
    std::list<routeCell> route;
    std::list <nearestBuildingTile> listOfDropOffLocations;
    std::list <nearestBuildingTile> listOfResourceLocations;
    std::list<orderStack> listOfOrders;
    sf::IntRect lastIntRect;
};

extern std::vector<actors> listOfActors;
extern std::vector<int>    listOfActorsWhoNeedAPath;

extern void                updateCells(int goalId, int startId, std::vector<Cells>& cellsList, bool cantPassActors);
extern nearestBuildingTile findNearestBuildingTile(int buildingId, int actorId);

#endif // ACTORS_H
