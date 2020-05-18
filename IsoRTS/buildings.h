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


class buildings
{

public:
    buildings() {}
    buildings(int type, int startXlocation, int startYLocation, int buildingId, int team);
    void                        update();
    void                        drawBuildingFootprint(int type, int mouseWorldX, int mouseWorldY);
    void                        drawBuilding(int i, int j, int type, bool typeOverride);
    int                         getRecievesWhichResources();
    int                         getType();
    int                         getLocationX();
    int                         getLocationY();
    int                         getBuildingId();
    int                         getTeam();
    bool                        getCompleted();
    void                        setCompleted();
    int                         getRangedDMG();
    bool                        hasTask();
    void                        removeBuilding();
    void                        fillAdjacentTiles();
    void                        getTask(bool isResearch, int idOfUnitOrResearch, int productionPointsNeeded);
    std::vector<buildingQueue>  productionQueue;
    std::pair<int, int>         getBuildingPoints();
    std::string                 getName();
    std::pair<int, int>         getHealth();
    void                        addBuildingPoint();
    std::vector<adjacentTile>   getDropOffTiles();
    std::vector<adjacentTile>   getFreeBuildingTile();
    void                        claimFreeBuiildingTile(int id, int actorId);
    void                        removeActorFromBuildingTile(int actorId);

private:
    int                         buildingId;
    int                         buildingType;
    int                         ownedByPlayer;
    int                         hitPointsTotal;
    int                         hitPointsLeft;
    int                         amountOfAnimationSprites;
    int                         spriteHeight;
    int                         spriteWidth;
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
    std::vector<adjacentTile>   adjacentTiles;
};

extern std::vector<buildings> listOfBuildings;

#endif // BUILDINGS_H
