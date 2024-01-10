#include "actorTemplates.h"

actorTemplates::actorTemplates(actorNames actorId, bool doesRangedDamage, int timeBetweenShots, int timeToCrossOneTile, int hitPoints, int meleeDamage, int projectileType, int range, int rangedDamage,
    int rateOfFire, int splashDamage, cords textureRect, actorOrBuildingPrice priceOfActor, std::string actorTexture, std::string realActorName, cords spriteOrigin, int bigSpriteYOffset, std::vector<actorButtonVariables> actorButtons)
{
    this->actorId = actorId;
    this->doesRangedDamage = doesRangedDamage;
    this->timeBetweenShots = timeBetweenShots;
    this->timeToCrossOneTile = timeToCrossOneTile;
    this->hitPoints = hitPoints;
    this->meleeDamage = meleeDamage;
    this->projectileType = projectileType;
    this->range = range;
    this->rangedDamage = rangedDamage;
    this->rateOfFire = rateOfFire;
    this->splashDamage = splashDamage;
    this->textureRect = textureRect;
    this->spriteOrigin = spriteOrigin;
    this->priceOfActor = priceOfActor;
    this->realActorName = realActorName;
    this->bigSpriteYOffset = bigSpriteYOffset;
    this->actorButtons = actorButtons;

    if (!Collision::CreateTextureAndBitmask(this->actorTexture, actorTexture))
    {
        std::cout << "Error loading texture: " << actorTexture << std::endl;
    }
}

actorNames actorTemplates::getActorId() const
{
    return this->actorId;
}

int actorTemplates::getBigSpriteYOffset() const
{
    return this->bigSpriteYOffset;
}

bool actorTemplates::getDoesRangedDamage() const
{
    return this->doesRangedDamage;
}

int actorTemplates::getTimeBetweenShots() const
{
    return this->timeBetweenShots;
}

int actorTemplates::getTimeToCrossOneTile() const
{
    return this->timeToCrossOneTile;
}

int actorTemplates::getHitPoints() const
{
    return this->hitPoints;
}

int actorTemplates::getMeleeDamage() const
{
    return this->meleeDamage;
}

int actorTemplates::getProjectileType() const
{
    return this->projectileType;
}

int actorTemplates::getRange() const
{
    return this->range;
}

int actorTemplates::getRangedDamage() const
{
    return this->rangedDamage;
}

int actorTemplates::getRateOfFire() const
{
    return this->rateOfFire;
}

int actorTemplates::getSplashDamage() const
{
    return this->splashDamage;
}

cords actorTemplates::getTextureRect() const
{
    return this->textureRect;
}

actorOrBuildingPrice actorTemplates::getPriceOfActor() const
{
    return this->priceOfActor;
}

sf::Sprite& actorTemplates::getActorSprite()
{
    return this->actorSprite;
}

sf::Texture actorTemplates::getActorTexture()
{
    return this->actorTexture;
}

std::string actorTemplates::getRealActorName() const
{
    return this->realActorName;
}

std::list<actorButtonVariables> actorTemplates::getActorButtonsOfMenu(int menu)
{
    std::list<actorButtonVariables> tempButtonList;
    for (actorButtonVariables& actorButton : actorButtons) {
        if (actorButton.subMenu == menu) {
            tempButtonList.push_back(actorButton);
        }
    }
    return tempButtonList;
}

void actorTemplates::setActorTexture()
{
    this->actorSprite.setTexture(this->actorTexture);
    this->actorSprite.setTextureRect(sf::IntRect(0, 0, this->textureRect.x, this->textureRect.y));
    this->actorSprite.setOrigin(static_cast<float>(this->spriteOrigin.x), static_cast<float>(this->spriteOrigin.y));
}

void actorTemplates::setSpritePosition(cords position) {
    this->actorSprite.setPosition(static_cast<float>(position.x), static_cast<float>(position.y));
}

void loadActors()
{

    //villager 
    listOfActorTemplates.push_back({
        actorNames::villager,                           //actorNames              actorId,
        false,                              //bool                    doesRangedDamage,
        2000,                               //int                   timeBetweenShots,
        1000,                               //int                  timeToCrossOneTile,
        25,                                 //int                     hitPoints,
        3,                                  //int                     meleeDamage,
        0,                                  //int                     projectileType,
        3,                                  //int                     range,
        1,                                  //int                     rangedDamage,
        2000,                                //int                     rateOfFire,
        0,                                  //int                     splashDamage,
        { 16, 32 },                         //cords                   textureRect,
        { 50, 0, 0, 0, 25 },                //actorOrBuildingPrice    priceOfActor, (food, wood, stone, gold, production points)
        "textures/testVillagerSprite.png",  //std::string             actorTexture,
        "Villager",                         //std::string             realActorName
        {-24, 12 },                         //cords                   spriteOrigin
        0,                                  //int                     bigSpriteYOffset
        {                                   //std::vector<actorButtonVariables> actorButtons
            //0
            {spriteTypes::spriteCivilBuilding, actionTypes::actionShowCivilBuildings, 0},
            {spriteTypes::spriteMilitairyBuilding, actionTypes::actionShowMilitairyBuildings, 0},
            {spriteTypes::spriteAttackMove, actionTypes::actionAttackMove, 0},
            //1
            {spriteTypes::spriteTownCenter, actionTypes::actionBuildTownCenter, 1},
            {spriteTypes::spriteHouse, actionTypes::actionBuildHouse, 1},
            {spriteTypes::spriteMill, actionTypes::actionBuildMill, 1},
            {spriteTypes::spriteLumberCamp, actionTypes::actionBuildLumberCamp, 1},
            {spriteTypes::spriteMiningCamp, actionTypes::actionBuildMiningCamp, 1},
            {spriteTypes::spriteGoBack, actionTypes::actionShowBack, 1},
            //2
            {spriteTypes::spriteBarracks, actionTypes::actionBuildBarracks, 2},
            {spriteTypes::spriteWall, actionTypes::actionBuildWall, 2},
            {spriteTypes::spriteGoBack, actionTypes::actionShowBack, 2},
        }
    });

    //priceOfActor.push_back({ 60,0,0,20,25 });  Swordsman
    listOfActorTemplates.push_back({
        actorNames::swordsman,                           //actorNames              actorId,
        false,                              //bool                    doesRangedDamage,
        2000,                               //int                   timeBetweenShots,
        800,                               //int                 timeToCrossOneTile,
        60,                                 //int                     hitPoints,
        12,                                  //int                    meleeDamage,
        0,                                  //int                     projectileType,
        3,                                  //int                     range,
        0,                                  //int                     rangedDamage,
        2000,                                  //int                     rateOfFire,
        0,                                  //int                     splashDamage,
        { 16, 32 },                         //cords                   textureRect,
        { 50, 0, 0, 0, 25 },                //actorOrBuildingPrice    priceOfActor, (food, wood, stone, gold, production points)
        "textures/swordsman.png",           //std::string             actorTexture,
        "Swordsman",                        //std::string             realActorName
        {-24, 12},                          //cords                   spriteOrigin
        128,                                //bigSpriteYOffset
        {                                   //std::vector<actorButtonVariables> actorButtons
            {spriteTypes::spriteAttackMove, actionTypes::actionAttackMove, 0},
        }
    });

    for (int i = 0; i < listOfActorTemplates.size(); i++) {
        listOfActorTemplates[i].setActorTexture();
    }
}
std::vector<actorTemplates> listOfActorTemplates;