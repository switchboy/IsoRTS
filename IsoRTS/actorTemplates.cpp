#include "actorTemplates.h"

actorTemplates::actorTemplates(actorNames actorId, bool doesRangedDamage, float timeBetweenShots, float timeToCrossOneTile, int hitPoints, int meleeDamage, int projectileType, int range, int rangedDamage, int rateOfFire, int splashDamage, cords textureRect, actorOrBuildingPrice priceOfActor, std::string actorTexture, std::string realActorName, cords spriteOrigin)
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
    this->priceOfActor = priceOfActor;
    this->realActorName = realActorName;
    if (!this->actorTexture.loadFromFile(actorTexture))
    {
        std::cout << "Error loading texture: " << actorTexture << std::endl;
    }
}

actorNames actorTemplates::getActorId() const
{
    return this->actorId;
}

bool actorTemplates::getDoesRangedDamage() const
{
    return this->doesRangedDamage;
}

float actorTemplates::getTimeBetweenShots() const
{
    return this->timeBetweenShots;
}

float actorTemplates::getTimeToCrossOneTile() const
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
    this->actorSprite;
}

sf::Texture actorTemplates::getActorTexture()
{
    return this->actorTexture;
}

std::string actorTemplates::getRealActorName() const
{
    return this->realActorName;
}

void actorTemplates::setActorTexture()
{
    this->actorSprite.setTexture(this->actorTexture);
    this->actorSprite.setTextureRect(sf::IntRect(0, 0, this->textureRect.x, this->textureRect.y));
    this->actorSprite.setOrigin(static_cast<float>(this->spriteOrigin.x), static_cast<float>(this->spriteOrigin.y));
}

void loadActors()
{

    //villager 
    listOfActorTemplates.push_back({
        villager,                           //actorNames              actorId,
        false,                              //bool                    doesRangedDamage,
        2.0f,                               //float                   timeBetweenShots,
        1.0f,                               //float                   timeToCrossOneTile,
        25,                                 //int                     hitPoints,
        3,                                  //int                     meleeDamage,
        0,                                  //int                     projectileType,
        3,                                  //int                     range,
        1,                                  //int                     rangedDamage,
        2,                                  //int                     rateOfFire,
        0,                                  //int                     splashDamage,
        { 16, 32 },                         //cords                   textureRect,
        { 50, 0, 0, 0, 25 },                //actorOrBuildingPrice    priceOfActor, (food, wood, stone, gold, production points)
        "textures/testVillagerSprite.png",  //std::string             actorTexture,
        "Villager",                         //std::string             realActorName
        {-24,12}
    });

    //priceOfActor.push_back({ 60,0,0,20,25 });  Swordsman
    listOfActorTemplates.push_back({
        swordsman,                           //actorNames              actorId,
        false,                              //bool                    doesRangedDamage,
        2.0f,                               //float                   timeBetweenShots,
        0.8f,                               //float                   timeToCrossOneTile,
        60,                                 //int                     hitPoints,
        12,                                  //int                    meleeDamage,
        0,                                  //int                     projectileType,
        3,                                  //int                     range,
        0,                                  //int                     rangedDamage,
        3,                                  //int                     rateOfFire,
        0,                                  //int                     splashDamage,
        { 16, 32 },                         //cords                   textureRect,
        { 50, 0, 0, 0, 25 },                //actorOrBuildingPrice    priceOfActor, (food, wood, stone, gold, production points)
        "textures/swordsman.png",           //std::string             actorTexture,
        "Swordsman",                        //std::string             realActorName
        {-24,12}
    });
}