#ifndef BUILDINGS_H
#define BUILDINGS_H
#include <vector>
#include <list>
#include <string>
#include "actors.h"

struct footprintOfBuilding
{
    int amountOfXFootprint;
    int amountOfYFootprint;
};

extern std::vector<footprintOfBuilding> footprintOfBuildings;
extern bool noNewBuildings;

struct buildingQueue
{
    bool isResearch;
    int idOfUnitOrResearch;
    int productionPointsGained;
    int productionPointsNeeded;
    float lastTimeUpdate;
};

struct orderContainer {
    cords goal;
    stackOrderTypes orderType;
    bool isSet;
};

class buildings
{

public:
    buildings() { 
        this->exists = false; 
        this->buildingType = 0;
        this->startXlocation = 0;
        this->startYLocation = 0;
        this->endXlocation = 0;
        this->endYLocation = 0;
        this->buildingId = 0;
        this->ownedByPlayer = 0;
        this->buildingCompleted = false;
        this->exists = true;
        this->lastShotFired = 0.0f;
        this->rallyPoint = { {0,0}, stackOrderTypes::stackActionMove, false }; //set dummy values for the rally point
        this->lastFrameUpdate = 0.0f;
        hitPointsTotal = 0;
        hitPointsLeft = 0;
        canDoRangedDamage = false;
        amountOfRangedDamage = 0;
        range = 0;
        recievesWood = false;
        recievesStone = false;
        recievesGold = false;
        recievesFood = false;
        this->hasDisplayedError = false;
        buildingPointsNeeded = 0;
        buildingPointsRecieved = 0;
        supportsPopulationOf = 0;
        this->offSetYStore = 0;
        this->amountOfAnimationSprites = 0;
    }
    buildings(int type, int startXlocation, int startYLocation, int buildingId, int team);
    void                        update();
    void                        drawBuildingFootprint(int type, int mouseWorldX, int mouseWorldY);
    void                        drawBuilding(int i, int j, int type, bool typeOverride);
    resourceTypes               getRecievesWhichResources();
    int                         getType();
    int                         getLocationX();
    int                         getLocationY();
    int                         getBuildingId();
    int                         getTeam();
    bool                        getCompleted();
    void                        setCompleted();
    int                         getRangedDMG();
    void                        setRallyPoint(cords goal, stackOrderTypes orderType);
    bool                        hasTask();
    void                        removeBuilding();
    void                        fillAdjacentTiles();
    void                        getTask(bool isResearch, int idOfUnitOrResearch);
    void                        takeDamage(int amountOfDamage);
    void                        doProduction();
    void                        spawnProduce();
    void                        checkOnEnemyAndShoot();
    std::vector<buildingQueue>  productionQueue;
    std::pair<int, int>         getBuildingPoints();
    std::string                 getName();
    std::pair<int, int>         getHealth();
    void                        addBuildingPoint();
    std::vector<adjacentTile>   getDropOffTiles();
    std::vector<adjacentTile>   getFreeBuildingTile();
    bool                        claimFreeBuiildingTile(int id, int actorId);
    void                        removeActorFromBuildingTile(int actorId);
    std::list<cords>   getFootprintOfBuilding();
    

private:
    int                         buildingId;
    int                         buildingType;
    int                         ownedByPlayer;
    int                         hitPointsTotal;
    int                         hitPointsLeft;
    int                         amountOfAnimationSprites;

    int                         offSetYStore;
    bool                        buildingCompleted;
    bool                        canDoRangedDamage;
    bool                        recievesWood;
    bool                        recievesStone;
    bool                        recievesGold;
    bool                        recievesFood;
    bool                        exists;
    int                         buildingPointsNeeded;
    int                         buildingPointsRecieved;
    int                         startXlocation;
    int                         endXlocation;
    int                         startYLocation;
    int                         endYLocation;
    int                         amountOfRangedDamage;
    int                         supportsPopulationOf;
    bool                        hasDisplayedError;
    int                         range;
    float                       lastShotFired;
    float                       lastFrameUpdate;
    std::vector<adjacentTile>   adjacentTiles;
    orderContainer              rallyPoint;
};


extern std::vector<buildings> listOfBuildings;

#endif // BUILDINGS_H
