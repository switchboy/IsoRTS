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
    int buildingId;
    bool isSet;
};

extern void updateCells(int goalId, int startId, std::vector<Cells> &cellsList);

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
    int visited;
    int parentCellId;
    int backParent;
};

struct cords{
    int x;
    int y;
};

class actors
{
public:
    actors(int type, int actorX, int actorY, int actorTeam, int actorId);
    virtual ~actors();
    void update();
    void calculateRoute();
    void drawActor();
    std::string nameOfActor();
    void updateGoal(int i, int j, int waitTime);
    void renderPath();
    void setCommonGoalTrue();
    void setGatheringRecource(bool flag);
    void setIsBuildingTrue(int buildingId);
    bool canTargetBeReached();
    nearestBuildingTile findNearestDropOffPoint(int Resource);
    void pathAStar();
    void pathAStarBiDi();
    bool isInitialized();
    int getTeam();
    cords getLocation();
    int getType();
    int getActorId();
    std::pair<int, int> getHealth();
    bool findNearestSimilairResource();
    void walkBackToOwnSquare();
    void startGatheringAnimation();
    void animateWalkingToResource();
    void gatherResource();
    void buildBuilding();
    void unloadAndReturnToGathering();
    void cleanUp();
    int getMeleeDMG();
    int getRangedDMG();


private:
    int actorType;
    int actorTeam;
    int actorHealth;
    bool actorAlive;
    int buildingId;
    int actorId;
    int hitPoints;
    int totalHitPoints;
    int meleeDamage;
    bool doesRangedDamage;
    int rangedDamage;
    int range;
    int actorCords[2];
    int actorGoal[2];
    int gatheringResourcesAt[2];
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
    int ResourceBeingGatherd;
    bool carriesRecources;
    int amountOfGold;
    int amountOfWood;
    int amountOfStone;
    int amountOfFood;
    bool pathFound;
    int orientation;
    int currentFrame;
    int spriteYOffset;
    float timeStartedGatheringRecource;
    float timeLastOffsetChange;
    float timeLastUpdate;
    float timeStartedWalkingToRecource;
    int retries;
    float timeLastAttempt;
    int waitForAmountOfFrames;
    bool goalNeedsUpdate;
    bool isBackAtOwnSquare;
    float offSetX;
    float offSetY;
    bool noPathPossible;
    bool routeNeedsPath;
    bool initialized;
    bool isBuilding;
    nearestBuildingTile dropOffTile;
    std::list<routeCell> route;
    bool hasMoved;
    std::list <nearestBuildingTile> listOfDropOffLocations;



};

extern std::vector<actors> listOfActors;
extern std::list<int> listOfActorsWhoNeedAPath;

#endif // ACTORS_H
