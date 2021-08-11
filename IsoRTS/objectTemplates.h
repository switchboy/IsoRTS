#pragma once
#include "globalFunctions.h"
#include "humanReadableNames.h"
#include "objects.h"
#include <SFML/Graphics.hpp>



class objectTemplates
{
public:
    objectTemplates(
        objectTypes             objectId,
        resourceTypes           typeOfResource,
        int                     startAmountOfResources,
        std::string             realName,
        cords                   spriteOrigin,
        cords                   textureRect,
        std::string             texture,
        int                     objectBigSpriteYOffset
    );

    objectTypes             getObjectId() const;
    resourceTypes           getTypeOfResource() const;
    int                     getStartAmountOfResources() const;
    std::string             getRealName() const;
    cords                   getSpriteOrigin() const;
    cords                   getTextureRect() const;
    sf::Sprite&             getSprite();
    int                     getObjectBigSpriteYOffset() const;
    sf::Texture             getTexture();
    void                    setTexture();

private:
    objectTypes             objectId;
    resourceTypes           typeOfResource;
    int                     startAmountOfResources;
    std::string             realName;
    cords                   spriteOrigin;
    cords                   textureRect;
    sf::Sprite              sprite;
    sf::Texture             texture;
    int                     objectBigSpriteYOffset;
};

extern void loadObjects();
extern std::vector<objectTemplates> listOfObjectTemplates;
