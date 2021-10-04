#ifndef BUILDINGS_H
#define BUILDINGS_H
#include <list>
#include <string>
#include <vector>
//#include "humanReadableNames.h"
#include "gamestate.h"
#include "globalfunctions.h"

struct buildingQueue
{
    bool isResearch;
    int idOfUnitOrResearch;
    int productionPointsGained;
    int productionPointsNeeded;
    int lastTimeUpdate;
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
        this->startLocation = { 0, 0};
        this->endLocation = { 0, 0 };
        this->buildingId = 0;
        this->ownedByPlayer = -1;
        this->buildingCompleted = false;
        this->exists = true;
        this->lastShotFired = 0;
        this->rallyPoint = { {0,0}, stackOrderTypes::stackActionMove, false }; //set dummy values for the rally point
        this->lastFrameUpdate = 0;
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

    buildings(int type, cords startLocation, int buildingId, int team);
    bool                        claimFreeBuiildingTile(int id, int actorId);
    bool                        hasTask() const;
    void                        addBuildingPoint();
    void                        checkOnEnemyAndShoot();
    void                        doProduction();
    void                        drawBuilding(int i, int j, int type, bool typeOverride);
    void                        drawBuildingFootprint(int type, cords mouseWorld);
    void                        fillAdjacentTiles();
    void                        getTask(bool isResearch, int idOfUnitOrResearch);
    void                        removeActorFromBuildingTile(int actorId);
    void                        removeBuilding();
    void                        spawnProduce();
    void                        takeDamage(int amountOfDamage);
    void                        update();
    
    int                         getBuildingId() const;
    bool                        getExists() const;
    bool                        getCompleted() const;
    bool                        getGateIsOpen() const;
    bool                        getIsGate() const;
    cords                       getLocation() const;
    int                         getRangedDMG() const;
    int                         getTeam() const;
    int                         getType() const;
    bool                        canBeGate() const;
    std::list<buildingQueue>    getProductionQueue() const;
    resourceTypes               getRecievesWhichResources() const;
    std::list<cords>            getFootprintOfBuilding() const;
    std::pair<int, int>         getBuildingPoints() const;
    std::pair<int, int>         getHealth() const;
    std::string                 getName() const;
    std::vector<adjacentTile>   getDropOffTiles() const;
    std::vector<adjacentTile>   getFreeBuildingTile() const;
    void                        setCompleted();
    void                        setIsGate();
    void                        setGateOpen(bool state);
    void                        setRallyPoint(cords goal, stackOrderTypes orderType);
    void                        removeTask(int id);

private:
    std::list<buildingQueue>    productionQueue;
    bool                        buildingCompleted;
    bool                        gateIsOpen = false;
    bool                        canDoRangedDamage;
    bool                        exists;
    bool                        hasDisplayedError;
    bool                        recievesFood;
    bool                        recievesGold;
    bool                        recievesStone;
    bool                        recievesWood;
    bool                        isGate = false;
    int                         lastFrameUpdate;
    int                         lastShotFired;
    int                         amountOfAnimationSprites;
    int                         amountOfRangedDamage;
    int                         buildingId;
    int                         buildingPointsNeeded;
    int                         buildingPointsRecieved;
    int                         buildingType;
    cords                       endLocation;
    int                         hitPointsLeft;
    int                         hitPointsTotal;
    int                         offSetYStore;
    int                         ownedByPlayer;
    int                         range;
    cords                       startLocation;
    int                         supportsPopulationOf;
    std::vector<adjacentTile>   adjacentTiles;
    orderContainer              rallyPoint;
    std::string                 buildingName;
};

extern std::vector<buildings>           listOfBuildings;
extern bool                             noNewBuildings;
#endif // BUILDINGS_H
