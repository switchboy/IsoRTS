#ifndef ACTORS_H
#define ACTORS_H
#include <cmath>
#include <list>
#include "gamestate.h"
#include <SFML/System.hpp>
#include <thread>


class Cells
{
public:
    int positionX, positionY, parentCellId, cummulativeCost, cellId, backParent;
    double costToGoal, totalCostGuess;
    bool visited = false;
    bool visitedBack = false;
    bool obstacle = false;
    std::vector<int> neighbours;
    void addNeighbours(std::vector<Cells> &cellsList);
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



class actors
{
public:
    actors(int type, int actorX, int actorY, int actorTeam, int actorId);
    virtual ~actors();
    void update();
    void searchAltetnative();
    void calculateRoute();
    void drawActor();
    std::string nameOfActor();
    std::string getRecources();
    void updateGoal(int i, int j, int waitTime);
    void renderPath();
    void setCommonGoalTrue();
    void setGatheringRecource(bool flag);
    void findNearestDropOffPoint();
    void findNearestSimilairResource();
    void getNewBuildingTileForSameBuilding();
    void pathAStar();
    void routing(std::vector<Cells>& cellsList, int& endCell, int& startCell, bool& endReached);
    bool isInitialized();
    int getTeam();
    cords getLocation();
    int getType();
    int getActorId();
    void setIsBuildingTrue(int buildingId, int& goalX, int& goalY);
    std::pair<int, int> getHealth();
    void walkBackToOwnSquare();
    void startGatheringAnimation();
    void animateWalkingToResource();
    void gatherResource();
    void buildBuilding();
    void unloadAndReturnToGathering();
    void cleanUp();
    void killActor();
    void doMeleeDamage();
    void takeDamage(int amountOfDamage);
    int getMeleeDMG();
    int getRangedDMG();
    void updateGoalPath();
    void moveActorIfWalking();
    void walkToNextSquare();
    void clearRoute();
    void startWalking();
    void retryWalkingOrChangeGoal();
    void handleResourceGathering();
    void handleBuilding();
    void houseKeeping();
    void setIsDoingAttack();
    void doTaskIfNotWalking();
    void shootProjectile();

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
    bool realPath;
    int actorType;
    int actorTeam;
    int actorHealth;
    int buildingId;
    int actorId;
    int hitPoints;
    int totalHitPoints;
    int meleeDamage;
    int rangedDamage;
    int range;
    int actorCords[2];
    int actorGoal[2];
    int actorRealGoal[2];
    int actorCommandGoal[2];
    int actionPreformedOnTile[2];
    int ResourceBeingGatherd;
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
    std::list<routeCell> route;
    std::list <nearestBuildingTile> listOfDropOffLocations;
    std::list <nearestBuildingTile> listOfResourceLocations;
};

extern std::vector<actors> listOfActors;
extern std::vector<int> listOfActorsWhoNeedAPath;

#endif // ACTORS_H
