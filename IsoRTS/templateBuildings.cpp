#include "templateBuildings.h"


std::vector<templateBuildings> listOfBuildingTemplates;

templateBuildings::templateBuildings(bool canDoRangedDamage, bool recievesWood, bool recievesStone, bool recievesGold, bool recievesFood, buildingNames idOfBuilding, int hitPointsTotal, int amountOfRangedDamage, int range, int buildingPointsNeeded, int supportsPopulationOf, int offSetYStore, int amountOfAnimationSprites, actorOrBuildingPrice priceOfBuilding, footprintOfBuilding buildingFootprint, cords buildingSprite, std::string buildingTexture, cords origin, std::string realBuildingName)
{
    this->canDoRangedDamage = canDoRangedDamage;
    this->recievesWood = recievesWood;
    this->recievesStone = recievesStone;
    this->recievesGold = recievesGold;
    this->recievesFood = recievesFood;
    this->idOfBuilding = idOfBuilding;
    this->hitPointsTotal = hitPointsTotal;
    this->amountOfRangedDamage = amountOfRangedDamage;
    this->range = range;
    this->buildingPointsNeeded = buildingPointsNeeded;
    this->supportsPopulationOf = supportsPopulationOf;
    this->offSetYStore = offSetYStore;
    this->amountOfAnimationSprites = amountOfAnimationSprites;
    this->priceOfBuilding = priceOfBuilding;
    this->buildingFootprint = buildingFootprint;
    this->textureRect = buildingSprite;
    this->origin = origin;
    if (!this->buildingTexture.loadFromFile(buildingTexture))
    {
        std::cout << "Error loading texture: house.png \n" << std::endl;
    }
}

void templateBuildings::setSpriteTexture() {
    //function to work around texture the location in memory change while the vector is being constructed
    this->buildingSprite.setTexture(this->buildingTexture);
    this->buildingSprite.setTextureRect(sf::IntRect(0, 0, this->textureRect.x, this->textureRect.y));
    this->buildingSprite.setOrigin(static_cast<float>(this->origin.x), static_cast<float>(this->origin.y));
}

void templateBuildings::setSpritePosition(cords position){
    this->buildingSprite.setPosition(static_cast<float>(position.x), static_cast<float>(position.y));
}

void templateBuildings::setSpriteTextureRect(sf::IntRect value) {
    this->buildingSprite.setTextureRect(value);
}

void templateBuildings::setSpriteColor(sf::Color color)
{
    this->buildingSprite.setColor(color);
}

bool templateBuildings::getCanDoRangedDamage() const
{
    return this->canDoRangedDamage;
}

bool templateBuildings::getRecievesWood() const
{
    return this->recievesWood;
}

bool templateBuildings::getRecievesStone() const
{
    return this->recievesStone;
}

bool templateBuildings::getRecievesGold() const
{
    return this->recievesGold;
}

bool templateBuildings::getRecievesFood() const
{
    return this->recievesFood;
}

buildingNames templateBuildings::getIdOfBuilding() const
{
    return this->idOfBuilding;
}

int templateBuildings::getHitPointsTotal() const
{
    return this->hitPointsTotal;
}

int templateBuildings::getAmountOfRangedDamage() const
{
    return this->amountOfRangedDamage;
}

int templateBuildings::getRange() const
{
    return this->range;
}

int templateBuildings::getBuildingPointsNeeded() const
{
    return this->buildingPointsNeeded;
}

int templateBuildings::getSupportsPopulationOf() const
{
    return this->supportsPopulationOf;
}

int templateBuildings::getOffSetYStore() const
{
    return this->offSetYStore;
}

int templateBuildings::getAmountOfAnimationSprites() const
{
    return this->amountOfAnimationSprites;
}

actorOrBuildingPrice templateBuildings::getPriceOfBuilding() const
{
    return this->priceOfBuilding;
}

footprintOfBuilding templateBuildings::getBuildingFootprint() const
{
    return this->buildingFootprint;
}

sf::Sprite templateBuildings::getBuildingSprite()
{
    return this->buildingSprite;
}

sf::Texture templateBuildings::getBuildingTexture()
{
    return this->buildingTexture;
}

std::string templateBuildings::getBuildingName() const
{
    return this->realBuildingName;
}

void loadBuildings()
{
    //load house
    listOfBuildingTemplates.push_back({
        false,                      //bool                    canDoRangedDamage,
        false,                      //bool                    recievesWood,
        false,                      //bool                    recievesStone,
        false,                      //bool                    recievesGold,
        false,                      //bool                    recievesFood,
        house,                      //buildingNames           idOfBuilding,
        450,                        //int                     hitPointsTotal,
        0,                          //int                     amountOfRangedDamage,
        0,                          //int                     range,
        25,                         //int                     buildingPointsNeeded,
        5,                          //int                     supportsPopulationOf,
        1,                          //int                     offSetYStore,
        0,                          //int                     amountOfAnimationSprites,
        {0,25,0,0},                 //actorOrBuildingPrice    priceOfBuilding, food, wood, stone, gold
        {2,2},                      //footprintOfBuilding     buildingFootprint,
        {128,128},                  //cords                   buildingSprite,
        "textures/house.png",       //std::string             buildingTexture,
        {32,96},                    //cords                   origin
        "Town house"                //std::string             realBuildingName;
    });

    //load towncenter
    listOfBuildingTemplates.push_back({
        true,                       //bool                    canDoRangedDamage,
        true,                       //bool                    recievesWood,
        true,                       //bool                    recievesStone,
        true,                       //bool                    recievesGold,
        true,                       //bool                    recievesFood,
        towncenter,                 //buildingNames           idOfBuilding,
        5000,                       //int                     hitPointsTotal,
        10,                         //int                     amountOfRangedDamage,
        4,                          //int                     range,
        150,                        //int                     buildingPointsNeeded,
        5,                          //int                     supportsPopulationOf,
        1,                          //int                     offSetYStore,
        0,                          //int                     amountOfAnimationSprites,
        { 0,300,100,0 },            //actorOrBuildingPrice    priceOfBuilding, food, wood, stone, gold
        { 4,4 },                    //footprintOfBuilding     buildingFootprint,
        {256,256},                  //cords                   buildingSprite,
        "textures/townCenter.png",  //std::string             buildingTexture,
        {96,224},                   //cords                   origin
         "Towncenter"               //std::string             realBuildingName;
        });

    //load mill
    listOfBuildingTemplates.push_back({
        false,                      //bool                    canDoRangedDamage,
        false,                      //bool                    recievesWood,
        false,                      //bool                    recievesStone,
        false,                      //bool                    recievesGold,
        true,                       //bool                    recievesFood,
        mill,                       //buildingNames           idOfBuilding,
        350,                        //int                     hitPointsTotal,
        0,                          //int                     amountOfRangedDamage,
        0,                          //int                     range,
        25,                         //int                     buildingPointsNeeded,
        0,                          //int                     supportsPopulationOf,
        1,                          //int                     offSetYStore,
        8,                          //int                     amountOfAnimationSprites,
        { 0,100,0,0 },              //actorOrBuildingPrice    priceOfBuilding, food, wood, stone, gold
        { 3,3 },                    //footprintOfBuilding     buildingFootprint,
        {192,192},                  //cords                   buildingSprite,
        "textures/mill.png",        //std::string             buildingTexture,
        {64,160},                   //cords                   origin
         "Mill"                     //std::string             realBuildingName;
    });

    //lumbercamp,
    listOfBuildingTemplates.push_back({
        false,                      //bool                    canDoRangedDamage,
        true,                       //bool                    recievesWood,
        false,                      //bool                    recievesStone,
        false,                      //bool                    recievesGold,
        false,                      //bool                    recievesFood,
        lumbercamp,                 //buildingNames           idOfBuilding,
        250,                        //int                     hitPointsTotal,
        0,                          //int                     amountOfRangedDamage,
        0,                          //int                     range,
        25,                         //int                     buildingPointsNeeded,
        0,                          //int                     supportsPopulationOf,
        1,                          //int                     offSetYStore,
        8,                          //int                     amountOfAnimationSprites,
        { 0,100,0,0 },              //actorOrBuildingPrice    priceOfBuilding, food, wood, stone, gold
        { 3,3 },                    //footprintOfBuilding     buildingFootprint,
        {192,192},                  //cords                   buildingSprite,
        "textures/lumbercamp.png",  //std::string             buildingTexture,
        {64,160},                   //cords                   origin
        "Lumbercamp"                //std::string             realBuildingName;
        });

    //barracks 4
    listOfBuildingTemplates.push_back({
        false,                      //bool                    canDoRangedDamage,
        true,                       //bool                    recievesWood,
        false,                      //bool                    recievesStone,
        false,                      //bool                    recievesGold,
        false,                      //bool                    recievesFood,
        barracks,                   //buildingNames           idOfBuilding,
        250,                        //int                     hitPointsTotal,
        0,                          //int                     amountOfRangedDamage,
        0,                          //int                     range,
        25,                         //int                     buildingPointsNeeded,
        0,                          //int                     supportsPopulationOf,
        1,                          //int                     offSetYStore,
        0,                          //int                     amountOfAnimationSprites,
        { 0,175,0,0 },              //actorOrBuildingPrice    priceOfBuilding, food, wood, stone, gold
        { 3,3 },                    //footprintOfBuilding     buildingFootprint,
        {192,192},                  //cords                   buildingSprite,
        "textures/barracks.png",    //std::string             buildingTexture,
        {64,160},                    //cords                   origin
        "Barracks"                //std::string             realBuildingName;
        });
    
    //Mining camp stone
    listOfBuildingTemplates.push_back({
        false,                      //bool                    canDoRangedDamage,
        false,                      //bool                    recievesWood,
        true,                       //bool                    recievesStone,
        true,                       //bool                    recievesGold,
        false,                      //bool                    recievesFood,
        miningcampstone,            //buildingNames           idOfBuilding,
        250,                        //int                     hitPointsTotal,
        0,                          //int                     amountOfRangedDamage,
        0,                          //int                     range,
        25,                         //int                     buildingPointsNeeded,
        0,                          //int                     supportsPopulationOf,
        1,                          //int                     offSetYStore,
        8,                          //int                     amountOfAnimationSprites,
        { 0,100,0,0 },              //actorOrBuildingPrice    priceOfBuilding, food, wood, stone, gold
        { 3,3 },                    //footprintOfBuilding     buildingFootprint,
        {192,192},                  //cords                   buildingSprite,
        "textures/miningCamp.png",  //std::string             buildingTexture,
        {64,160} ,                  //cords                   origin
        "Miningcamp"                //std::string             realBuildingName;
        });


    //Mining camp gold
    listOfBuildingTemplates.push_back({
        false,                      //bool                    canDoRangedDamage,
        false,                      //bool                    recievesWood,
        true,                       //bool                    recievesStone,
        true,                       //bool                    recievesGold,
        false,                      //bool                    recievesFood,
        miningcampgold,             //buildingNames           idOfBuilding,
        250,                        //int                     hitPointsTotal,
        0,                          //int                     amountOfRangedDamage,
        0,                          //int                     range,
        25,                         //int                     buildingPointsNeeded,
        0,                          //int                     supportsPopulationOf,
        1,                          //int                     offSetYStore,
        8,                          //int                     amountOfAnimationSprites,
        { 0,100,0,0 },              //actorOrBuildingPrice    priceOfBuilding, food, wood, stone, gold
        { 3,3 },                    //footprintOfBuilding     buildingFootprint,
        {192,192},                  //cords                   buildingSprite,
        "textures/miningCamp.png",  //std::string             buildingTexture,
        {64,160},                  //cords                   origin
        "Miningcamp"                //std::string             realBuildingName;
        });


    for (int i = 0; i < listOfBuildingTemplates.size(); i++) {
        listOfBuildingTemplates[i].setSpriteTexture();
    }

}
