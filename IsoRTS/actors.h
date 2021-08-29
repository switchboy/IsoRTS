#ifndef ACTORS_H
#define ACTORS_H
#include <cmath>
#include <list>
#include <SFML/System.hpp>
#include "gamestate.h"
#include "globalfunctions.h"
#include "actorTemplates.h"


class Cells
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
    cords position;
    int parentCellId, cummulativeCost, cellId, backParent;
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
    float timePassedSinceChangingOffset;
    float timeWalkingBackStarted;
    cords position;
    cords nPosition;
    float speedMultiplier;

    //calculations
    int startDeltaX = position.x - nPosition.x;
    int startDeltaY = position.y - nPosition.y;
    float deltaXCompleted = startDeltaX * (this->timePassedSinceChangingOffset * this->speedMultiplier);
    float deltaYCompleted = startDeltaY * (this->timePassedSinceChangingOffset * this->speedMultiplier);
};

struct drawXYOverride {
    bool isActive;
    cords newXY;
};

class actors
{
public:
    actors(int type, cords location, int actorTeam, int actorId);
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
    void routing(std::vector<Cells>& cellsList, int endCell, int startCell, bool endReached);
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
    void updateGoal(cords goal, int waitTime);
    void updateGoalPath();
    void walkBackToOwnSquare();
    void walkToNextSquare();
    void walkBackAfterAbortedCommand();
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
    sf::IntRect getLastIntRect() const;
    cords getGoal() const;
    resourceTypes getResourceGathered() const;
    cords getActorCords() const;
    const std::list<cords>& getRejectedTargetsList() const;
    std::pair<int, int> getHealth() const;
    std::string nameOfActor() const;
    std::string getResources() const;

    void setIsBuildingTrue(int buildingId, cords goal);
    void setIsDoingAttack();

private:
    bool isWalkingToMiddleOfSquare;
    bool lastTile;
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
    float timePassedSinceChangingOffset;
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
    float timeToCrossOneTile;
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
