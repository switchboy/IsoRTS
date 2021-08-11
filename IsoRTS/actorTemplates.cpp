#include "actorTemplates.h"

actorTemplates::actorTemplates(actorNames actorId, bool doesRangedDamage, float timeBetweenShots, float timeToCrossOneTile, int actorHealth, int hitPoints, int meleeDamage, int projectileType, int range, int rangedDamage, int rateOfFire, int splashDamage, cords textureRect, actorOrBuildingPrice priceOfActor, std::string actorTexture, std::string realActorName)
{
    this->actorId = actorId;
    this->doesRangedDamage = doesRangedDamage;
    this->timeBetweenShots = timeBetweenShots;
    this->timeToCrossOneTile = timeToCrossOneTile;
    this->actorHealth = actorHealth;
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

int actorTemplates::getActorHealth() const
{
    return this->actorHealth;
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
}
