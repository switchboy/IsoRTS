#pragma once
#include "globalfunctions.h"
#include <vector>
#include <SFML/Graphics.hpp>


enum actorNames {
    villager,
    swordsman
};

class actorTemplates
{
public:
    actorTemplates(
        actorNames              actorId,
        bool                    doesRangedDamage,
        float                   timeBetweenShots,
        float                   timeToCrossOneTile,
        int                     actorHealth,
        int                     hitPoints,
        int                     meleeDamage,
        int                     projectileType,
        int                     range,
        int                     rangedDamage,
        int                     rateOfFire,
        int                     splashDamage,
        cords                   textureRect,
        actorOrBuildingPrice    priceOfActor,
        sf::Sprite              actorSprite,
        sf::Texture             actorTexture,
        std::string             realActorName
    );

    //getters
    actorNames              getActorId;
    bool                    getDoesRangedDamage;
    float                   getTimeBetweenShots;
    float                   getTimeToCrossOneTile;
    int                     getActorHealth;
    int                     getHitPoints;
    int                     getMeleeDamage;
    int                     getProjectileType;
    int                     getRange;
    int                     getRangedDamage;
    int                     getRateOfFire;
    int                     getSplashDamage;
    cords                   getTextureRect;
    actorOrBuildingPrice    getPriceOfActor;
    sf::Sprite&             getActorSprite;
    sf::Texture             getActorTexture;
    std::string             getRealActorName;


private:
    actorNames              actorId;
    bool                    doesRangedDamage;
    float                   timeBetweenShots;
    float                   timeToCrossOneTile;
    int                     actorHealth;
    int                     hitPoints;
    int                     meleeDamage;
    int                     projectileType;
    int                     range;
    int                     rangedDamage;
    int                     rateOfFire;
    int                     splashDamage;
    cords                   textureRect;
    actorOrBuildingPrice    priceOfActor;
    sf::Sprite              actorSprite;
    sf::Texture             actorTexture;
    std::string             realActorName;
};


extern std::vector<actorTemplates>            listOfActorTemplates;
extern void loadActors();