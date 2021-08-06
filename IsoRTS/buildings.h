#ifndef BUILDINGS_H
#define BUILDINGS_H
#include <list>
#include <string>
#include <vector>
//#include "humanReadableNames.h"
#include "gamestate.h"

struct footprintOfBuilding
{
    int amountOfXFootprint;
    int amountOfYFootprint;
};

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
    bool                        claimFreeBuiildingTile(int id, int actorId);
    bool                        hasTask() const;
    void                        addBuildingPoint();
    void                        checkOnEnemyAndShoot();
    void                        doProduction();
    void                        drawBuilding(int i, int j, int type, bool typeOverride);
    void                        drawBuildingFootprint(int type, int mouseWorldX, int mouseWorldY);
    void                        fillAdjacentTiles();
    void                        getTask(bool isResearch, int idOfUnitOrResearch);
    void                        removeActorFromBuildingTile(int actorId);
    void                        removeBuilding();
    void                        spawnProduce();
    void                        takeDamage(int amountOfDamage);
    void                        update();
    std::vector<buildingQueue>  productionQueue;

    int                         getBuildingId() const;
    bool                        getCompleted() const;
    int                         getLocationX() const;
    int                         getLocationY() const;
    int                         getRangedDMG() const;
    int                         getTeam() const;
    int                         getType() const;
    resourceTypes               getRecievesWhichResources() const;
    std::list<cords>            getFootprintOfBuilding() const;
    std::pair<int, int>         getBuildingPoints() const;
    std::pair<int, int>         getHealth() const;
    std::string                 getName() const;
    std::vector<adjacentTile>   getDropOffTiles() const;
    std::vector<adjacentTile>   getFreeBuildingTile() const;

    void                        setCompleted();
    void                        setRallyPoint(cords goal, stackOrderTypes orderType);

private:
    bool                        buildingCompleted;
    bool                        canDoRangedDamage;
    bool                        exists;
    bool                        hasDisplayedError;
    bool                        recievesFood;
    bool                        recievesGold;
    bool                        recievesStone;
    bool                        recievesWood;
    float                       lastFrameUpdate;
    float                       lastShotFired;
    int                         amountOfAnimationSprites;
    int                         amountOfRangedDamage;
    int                         buildingId;
    int                         buildingPointsNeeded;
    int                         buildingPointsRecieved;
    int                         buildingType;
    int                         endXlocation;
    int                         endYLocation;
    int                         hitPointsLeft;
    int                         hitPointsTotal;
    int                         offSetYStore;
    int                         ownedByPlayer;
    int                         range;
    int                         startXlocation;
    int                         startYLocation;
    int                         supportsPopulationOf;
    std::vector<adjacentTile>   adjacentTiles;
    orderContainer              rallyPoint;
};

extern std::vector<buildings>           listOfBuildings;
extern std::vector<footprintOfBuilding> footprintOfBuildings;
extern bool                             noNewBuildings;

#endif // BUILDINGS_H
