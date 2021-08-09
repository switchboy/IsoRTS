#pragma once
#include "globalfunctions.h"
#include <vector>
#include <SFML/Graphics.hpp>

enum buildingNames {
    house,
    towncenter,
    mill,
    lumbercamp,
    barracks,
    miningcampstone,
    miningcampgold,
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
    std::string             realBuildingName
    );

    //Getters
    bool                    getCanDoRangedDamage() const;
    bool                    getRecievesWood() const;
    bool                    getRecievesStone() const;
    bool                    getRecievesGold() const;
    bool                    getRecievesFood() const;
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
    sf::Sprite              getBuildingSprite();
    sf::Texture             getBuildingTexture();
    std::string             getBuildingName() const; 

    //setters
    void                    setSpriteTextureRect(sf::IntRect value);
    void                    setSpriteColor(sf::Color color);
    void                    setSpritePosition(cords position);
    void                    setSpriteTexture();

private:
    bool                    canDoRangedDamage;
    bool                    recievesWood;
    bool                    recievesStone;
    bool                    recievesGold;
    bool                    recievesFood;
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
};

extern std::vector<templateBuildings>            listOfBuildingTemplates;
extern void loadBuildings();