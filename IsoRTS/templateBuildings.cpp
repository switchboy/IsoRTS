#include "templateBuildings.h"
#include "buildings.h"
#include "player.h"

std::vector<templateBuildings> listOfBuildingTemplates;

templateBuildings::templateBuildings(bool canDoRangedDamage, bool recievesWood, bool recievesStone, bool recievesGold, bool recievesFood, buildingNames idOfBuilding,
    int hitPointsTotal, int amountOfRangedDamage, int range, int buildingPointsNeeded, int supportsPopulationOf, int offSetYStore, int amountOfAnimationSprites, 
    actorOrBuildingPrice priceOfBuilding, footprintOfBuilding buildingFootprint, cords buildingSprite, std::string buildingTexture, cords origin, std::string realBuildingName, 
    int bigSpriteYOffset, bool isWall, std::list<buttonVariables> listOfBuildingButtons)
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
    this->realBuildingName = realBuildingName;
    this->bigSpriteYOffset = bigSpriteYOffset;
    this->isWall = isWall;
    this->listOfBuildingButtons = listOfBuildingButtons;
    if (!Collision::CreateTextureAndBitmask(this->buildingTexture, buildingTexture))
    {
        std::cout << "Error loading texture: "<< buildingTexture << std::endl;
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

sf::Sprite& templateBuildings::getBuildingSprite()
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
\

int templateBuildings::getBigSpriteYOffset() const
{
    return this->bigSpriteYOffset;
}

bool templateBuildings::getIsWall() const
{
    return this->isWall;
}


void templateBuildings::createBuildingButtons(int startX, int startY, int buildingId, int incrementalXOffset, bool& buttonsAreThere)
{
    if (!buttonsAreThere) {
        if (listOfBuildings[buildingId].getCompleted()) {
            int startXOr = startX;
            int buttonCounter = 0;
            for (buttonVariables currentButton : this->listOfBuildingButtons) {
                buttonCounter++;
                if (buttonCounter <= 5) {
                    if (buttons::requirementForButtonIsMet(currentButton.action, buildingId, listOfBuildings[buildingId].getTeam())) {
                        listOfButtons.push_back({ startX, startY, currentButton.sprite, currentButton.action, buildingId, static_cast<int>(listOfButtons.size()), 0 });
                    }
                    startX += incrementalXOffset;
                }
                else {
                    buttonCounter = 1;
                    startX = startXOr;
                    startY += incrementalXOffset;
                    if (buttons::requirementForButtonIsMet(currentButton.action, buildingId, listOfBuildings[buildingId].getTeam())) {
                        listOfButtons.push_back({ startX, startY, currentButton.sprite, currentButton.action, buildingId, static_cast<int>(listOfButtons.size()), 0 });
                    }
                    startX += incrementalXOffset;
                }
            }
        }
        buttonsAreThere = true;
    }
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
        buildingNames::house,       //buildingNames           idOfBuilding,
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
        "Town house",                //std::string             realBuildingName;
        128,                         //int                     bigSpriteYoffset
        false,                       //bool                   isWall
        {                            //                       Begin list of building buttons
         
        }                           //                        end list of building buttons
    });

    //load towncenter
    listOfBuildingTemplates.push_back({
        true,                       //bool                    canDoRangedDamage,
        true,                       //bool                    recievesWood,
        true,                       //bool                    recievesStone,
        true,                       //bool                    recievesGold,
        true,                       //bool                    recievesFood,
        buildingNames::towncenter,  //buildingNames           idOfBuilding,
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
         "Towncenter",               //std::string             realBuildingName;
         0,                         //int                     bigSpriteYoffset
        false,                       //bool                   isWall
        {                            //                       Begin list of building buttons
         {spriteTypes::spriteVillager, actionTypes::actionMakeVillager}
        }                           //                        end list of building buttons
        });

    //load mill
    listOfBuildingTemplates.push_back({
        false,                      //bool                    canDoRangedDamage,
        false,                      //bool                    recievesWood,
        false,                      //bool                    recievesStone,
        false,                      //bool                    recievesGold,
        true,                       //bool                    recievesFood,
        buildingNames::mill,        //buildingNames           idOfBuilding,
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
         "Mill",                     //std::string             realBuildingName;
         384,                         //int                     bigSpriteYoffset
        false,                       //bool                   isWall
        {                            //                       Begin list of building buttons

        }                           //                        end list of building buttons
    });

    //lumbercamp,
    listOfBuildingTemplates.push_back({
        false,                      //bool                    canDoRangedDamage,
        true,                       //bool                    recievesWood,
        false,                      //bool                    recievesStone,
        false,                      //bool                    recievesGold,
        false,                      //bool                    recievesFood,
        buildingNames::lumbercamp,  //buildingNames           idOfBuilding,
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
        "Lumbercamp",                //std::string            realBuildingName;
        256 ,                         //int                   bigSpriteYoffset
        false,                       //bool                   isWall
        {                            //                       Begin list of building buttons

        }                           //                        end list of building buttons
        });

    //barracks 4
    listOfBuildingTemplates.push_back({
        false,                      //bool                    canDoRangedDamage,
        true,                       //bool                    recievesWood,
        false,                      //bool                    recievesStone,
        false,                      //bool                    recievesGold,
        false,                      //bool                    recievesFood,
        buildingNames::barracks,    //buildingNames           idOfBuilding,
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
        {64,160},                   //cords                  origin
        "Barracks",                  //std::string             realBuildingName;
        512,                         //int                     bigSpriteYoffset
        false,                       //bool                   isWall
        {                            //                       Begin list of building buttons
            {spriteTypes::spriteSwordsman, actionTypes::actionMakeSwordsman}
        }                           //                        end list of building buttons
        });
    
    //Mining camp stone
    listOfBuildingTemplates.push_back({
        false,                              //bool                    canDoRangedDamage,
        false,                              //bool                    recievesWood,
        true,                               //bool                    recievesStone,
        true,                               //bool                    recievesGold,
        false,                              //bool                    recievesFood,
        buildingNames::miningcampstone,     //buildingNames           idOfBuilding,
        250,                                //int                     hitPointsTotal,
        0,                                  //int                     amountOfRangedDamage,
        0,                                  //int                     range,
        25,                                 //int                     buildingPointsNeeded,
        0,                                  //int                     supportsPopulationOf,
        1,                                  //int                     offSetYStore,
        8,                                  //int                     amountOfAnimationSprites,
        { 0,100,0,0 },                      //actorOrBuildingPrice    priceOfBuilding, food, wood, stone, gold
        { 3,3 },                            //footprintOfBuilding     buildingFootprint,
        {192,192},                          //cords                   buildingSprite,
        "textures/miningCamp.png",          //std::string             buildingTexture,
        {64,160} ,                          //cords                   origin
        "Miningcamp",                       //std::string             realBuildingName;
        640,                                //int                     bigSpriteYoffset
        false,                              //bool                   isWall
        {                                   //                       Begin list of building buttons

        }                                   //                       End list of building buttons
        });


    //Mining camp gold
    listOfBuildingTemplates.push_back({
        false,                                  //bool                    canDoRangedDamage,
        false,                                  //bool                    recievesWood,
        true,                                   //bool                    recievesStone,
        true,                                   //bool                    recievesGold,
        false,                                  //bool                    recievesFood,
        buildingNames::miningcampgold,          //buildingNames           idOfBuilding,
        250,                                    //int                     hitPointsTotal,
        0,                                      //int                     amountOfRangedDamage,
        0,                                      //int                     range,
        25,                                     //int                     buildingPointsNeeded,
        0,                                      //int                     supportsPopulationOf,
        1,                                      //int                     offSetYStore,
        8,                                      //int                     amountOfAnimationSprites,
        { 0,100,0,0 },                          //actorOrBuildingPrice    priceOfBuilding, food, wood, stone, gold
        { 3,3 },                                //footprintOfBuilding     buildingFootprint,
        {192,192},                              //cords                   buildingSprite,
        "textures/miningCamp.png",              //std::string             buildingTexture,
        {64,160},                               //cords                   origin
        "Miningcamp",                           //std::string             realBuildingName;
        640 ,                                   //int                     bigSpriteYoffset
        false,                                  //bool                   isWall
        {                                       //                       Begin list of building buttons

        }                                       //                       End list of building buttons
        });

    //Wall
    listOfBuildingTemplates.push_back({
        false,                                  //bool                    canDoRangedDamage,
        false,                                  //bool                    recievesWood,
        false,                                  //bool                    recievesStone,
        false,                                  //bool                    recievesGold,
        false,                                  //bool                    recievesFood,
        buildingNames::wall,                    //buildingNames           idOfBuilding,
        750,                                    //int                     hitPointsTotal,
        0,                                      //int                     amountOfRangedDamage,
        0,                                      //int                     range,
        10,                                     //int                     buildingPointsNeeded,
        0,                                      //int                     supportsPopulationOf,
        1,                                      //int                     offSetYStore,
        0,                                      //int                     amountOfAnimationSprites,
        { 0,0,5,0 },                            //actorOrBuildingPrice    priceOfBuilding, food, wood, stone, gold
        { 1,1 },                                //footprintOfBuilding     buildingFootprint,
        {64,96},                                //cords                   buildingSprite,
        "textures/wall.png",                    //std::string             buildingTexture,
        {0,64},                                //cords                   origin
        "Wall",                                //std::string             realBuildingName;
        640,                                   //int                     bigSpriteYoffset
        true,                                  //bool                   isWall
        {                                      //                       Begin list of building buttons
            {spriteTypes::spriteCancel, actionTypes::actionMakeGate},                                                   //TODO: sprite!
            {spriteTypes::spriteLumberCamp, actionTypes::actionOpenOrCloseGate}                                         //TODO: sprite!
        }                                      //                       End list of building buttons
        });


    for (int i = 0; i < listOfBuildingTemplates.size(); i++) {
        listOfBuildingTemplates[i].setSpriteTexture();
    }

}

