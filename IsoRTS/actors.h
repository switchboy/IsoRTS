#ifndef ACTORS_H
#define ACTORS_H
#include <cmath>
#include <list>
#include <SFML/System.hpp>
#include "gamestate.h"
#include "globalfunctions.h"

class Cells
{
public:
    Cells() {
        positionX = 0;
        positionY = 0; 
        parentCellId = 0; 
        cummulativeCost = 0; 
        cellId = 0; 
        backParent = 0;
        costToGoal = 0;
        totalCostGuess = 0;
    }
    int positionX, positionY, parentCellId, cummulativeCost, cellId, backParent;
    double costToGoal, totalCostGuess;
    bool visited = false;
    bool visitedBack = false;
    bool obstacle = false;
    std::vector<int> neighbours;
    void addNeighbours(const std::vector<Cells> &cellsList);
};

struct nearestBuildingTile
{
    float deltaDistance;
    int locationX;
    int locationY;
    int actionLocationX;
    int actionLocationY;
    int buildingId;
    bool isSet;
    int tileId;
};

extern void updateCells(int goalId, int startId, std::vector<Cells> &cellsList);
extern nearestBuildingTile findNearestBuildingTile(int buildingId, int actorId);

struct islandCell
{
    int positionX;
    int positionY;
    int cellId;
    int cellScore;
    int parentId;
};

struct routeCell
{
    int positionX;
    int positionY;
    bool visited;
    int parentCellId;
    int backParent;
};

struct orderStack
{
    cords goal;
    stackOrderTypes orderType;
};

class actors
{
public:
    actors(int type, int actorX, int actorY, int actorTeam, int actorId);
    virtual ~actors();
    void animateWalkingToResource();
    void buildBuilding();
    void calculateRoute();
    void chaseTarget();
    void cleanUp();
    void clearCommandStack();
    void clearRoute();
    void doMeleeDamage();
    void doNextStackedCommand();
    void doTaskIfNotWalking();
    void drawActor();
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
    void pathAStar();
    void renderPath();
    void retryWalkingOrChangeGoal();
    void routing(std::vector<Cells>& cellsList, int& endCell, int& startCell, bool& endReached);
    void searchAltetnative();
    void setCommonGoalTrue();
    void setGatheringRecource(bool flag);
    void shootProjectile();
    void stackOrder(cords Goal, stackOrderTypes orderType);
    void startGatheringAnimation();
    void startWalking();
    void takeDamage(int amountOfDamage, int idOfAttacker);
    void unloadAndReturnToGathering();
    void update();
    void updateGoal(int i, int j, int waitTime);
    void updateGoalPath();
    void walkBackToOwnSquare();
    void walkToNextSquare();

    bool getIsBuilding() const;
    bool idle() const;
    bool isAlive() const;
    bool isGathering() const;
    bool isInitialized() const;
    int getActorId() const;
    int getBuildingId() const;
    int getMeleeDMG() const;
    int getRangedDMG() const;
    int getTeam() const;
    int getType() const;
    cords getGoal() const;
    resourceTypes getResourceGathered() const;
    cords getActorCords() const;
    const std::list<cords>& getRejectedTargetsList() const;
    std::pair<int, int> getHealth() const;
    std::string nameOfActor() const;
    std::string getResources() const;

    void setIsBuildingTrue(int buildingId, int& goalX, int& goalY);
    void setIsDoingAttack();

private:
    bool actorAlive;
    bool doesRangedDamage;
    bool busyWalking;
    bool movedMoreThanHalf;
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
    bool hasStartedSearchingForAlternatives;
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
    int actorCords[2];
    int actorGoal[2];
    int actorRealGoal[2];
    int actorCommandGoal[2];
    int actionPreformedOnTile[2];
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
    float timeStartedGatheringRecource;
    float timeLastOffsetChange;
    float timeLastUpdate;
    float timeLastPathTry;
    float timeStartedWalkingToRecource;
    float timeLastAttempt;
    float offSetX;
    float offSetY;
    float timeBetweenShots;
    float timeLastRetry;
    nearestBuildingTile dropOffTile;
    std::list<cords> listOfTargetsToRejectUntilSuccesfullMovement;
    std::list<routeCell> route;
    std::list <nearestBuildingTile> listOfDropOffLocations;
    std::list <nearestBuildingTile> listOfResourceLocations;
    std::list<orderStack> listOfOrders;
};

extern std::vector<actors> listOfActors;
extern std::vector<int> listOfActorsWhoNeedAPath;

#endif // ACTORS_H
