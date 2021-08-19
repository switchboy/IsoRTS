#pragma once
#include "globalfunctions.h"
#include <vector>
#include <SFML/Graphics.hpp>
#include "collision.h"
#include "humanReadableNames.h"
#include "button.h"
//#include "buildings.h"

enum class buildingNames : uint32_t {
    house,
    towncenter,
    mill,
    lumbercamp,
    barracks,
    miningcampstone,
    miningcampgold,
    wall
};

struct buttonVariables {
    spriteTypes sprite;
    actionTypes action;
};

class templateBuildings
{
public:
    templateBuildings(
    bool                    canDoRangedDamage,
    bool                    recievesWood,
    bool                    recievesStone,
    bool                    recievesGold,
    bool                    recievesFood,
    buildingNames           idOfBuilding,
    int                     hitPointsTotal,
    int                     amountOfRangedDamage,
    int                     range,
    int                     buildingPointsNeeded,
    int                     supportsPopulationOf,
    int                     offSetYStore,
    int                     amountOfAnimationSprites,
    actorOrBuildingPrice    priceOfBuilding,
    footprintOfBuilding     buildingFootprint,
    cords                   buildingSprite, 
    std::string             buildingTexture, 
    cords                   origin,
    std::string             realBuildingName,
    int                     bigSpriteYOffset,
    bool                    isWall,
    std::list<buttonVariables>         listOfBuildingButtons
    );

    //Getters
    bool                    getCanDoRangedDamage() const;
    bool                    getRecievesWood() const;
    bool                    getRecievesStone() const;
    bool                    getRecievesGold() const;
    bool                    getRecievesFood() const;
    bool                    getIsWall() const;
    buildingNames           getIdOfBuilding() const;
    int                     getHitPointsTotal() const;
    int                     getAmountOfRangedDamage() const;
    int                     getRange() const;
    int                     getBuildingPointsNeeded() const;
    int                     getSupportsPopulationOf() const;
    int                     getOffSetYStore() const;
    int                     getAmountOfAnimationSprites() const;
    actorOrBuildingPrice    getPriceOfBuilding() const;
    footprintOfBuilding     getBuildingFootprint() const;
    sf::Sprite&             getBuildingSprite();
    sf::Texture             getBuildingTexture();//TODO: should this return an sf::Texture& instead? - If I keep it probably, but I haven't yet foud a use for it so it might get scrapped
    std::string             getBuildingName() const; 
    int                     getBigSpriteYOffset() const;
    void                    createBuildingButtons(int startX, int startY, int buildingId, int incrementalXOffset, bool& buttonsAreThere);

    //setters
    void                    setSpritePosition(cords position); 
    void                    setSpriteTexture(); //still needed since texture needs to set on sprite AFTER it is in its 'permanent' momory position (once the listOfBuildingTemplates is finished)

private:
    bool                    canDoRangedDamage;
    bool                    recievesWood;
    bool                    recievesStone;
    bool                    recievesGold;
    bool                    recievesFood;
    bool                    isWall;
    buildingNames           idOfBuilding;
    int                     hitPointsTotal;
    int                     amountOfRangedDamage;
    int                     range;
    int                     buildingPointsNeeded;
    int                     supportsPopulationOf;
    int                     offSetYStore;
    int                     amountOfAnimationSprites; 
    cords                   textureRect;
    cords                   origin;
    actorOrBuildingPrice    priceOfBuilding;
    footprintOfBuilding     buildingFootprint;
    sf::Sprite              buildingSprite;
    sf::Texture             buildingTexture;
    std::string             realBuildingName;
    int                     bigSpriteYOffset;
    std::list<buttonVariables>         listOfBuildingButtons;
};

extern std::vector<templateBuildings>            listOfBuildingTemplates;
extern void loadBuildings();