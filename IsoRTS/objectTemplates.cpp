#include "objectTemplates.h"

objectTemplates::objectTemplates(objectTypes objectId, resourceTypes typeOfResource, int startAmountOfResources, std::string realName, cords spriteOrigin, cords textureRect, std::string texture, int objectBigSpriteYOffset)
{
    this->objectId = objectId;
    this->typeOfResource = typeOfResource;
    this->startAmountOfResources = startAmountOfResources;
    this->realName = realName;
    this->spriteOrigin = spriteOrigin;
    this->textureRect = textureRect;
    this->objectBigSpriteYOffset = objectBigSpriteYOffset;
    if (!this->texture.loadFromFile(texture))
    {
        std::cout << "Error loading texture: " << texture << std::endl;
    }
}

objectTypes objectTemplates::getObjectId() const
{
    return this->objectId;
}

resourceTypes objectTemplates::getTypeOfResource() const
{
    return this->typeOfResource;
}

int objectTemplates::getStartAmountOfResources() const
{
    return this->startAmountOfResources;
}

std::string objectTemplates::getRealName() const
{
    return this->realName;
}

cords objectTemplates::getSpriteOrigin() const
{
    return this->spriteOrigin;
}

cords objectTemplates::getTextureRect() const
{
    return this->textureRect;
}

sf::Sprite& objectTemplates::getSprite()
{
    return this->sprite;
}

int objectTemplates::getObjectBigSpriteYOffset() const
{
    return this->objectBigSpriteYOffset;
}

sf::Texture objectTemplates::getTexture()
{
    return this->texture;
}

void objectTemplates::setTexture()
{
    this->sprite.setTexture(this->texture);
    this->sprite.setTextureRect(sf::IntRect(0, 0, this->textureRect.x, this->textureRect.y));
    this->sprite.setOrigin(static_cast<float>(this->spriteOrigin.x), static_cast<float>(this->spriteOrigin.y));
}

int getObjectBigSpriteYOffset(int objectId)
{
    switch (listOfObjects[objectId].getType())
    {
    case objectTypes::cactus:
        return 0;
        break;
    case objectTypes::cypress:
        return 128;
        break;
    case objectTypes::maple:
        return 256;
        break;
    case objectTypes::pine:
        return 384;
        break;
    case objectTypes::stone:
        return 512;
        break;
    case objectTypes::gold:
        return 640;
        break;
    case objectTypes::berry:
        return 768;
        break;
    default:
        return 0;
        break;
    }
}

void loadObjects()
{
    //cactus
    listOfObjectTemplates.push_back({
        objectTypes::cactus,          //objectTypes             objectId,
        resourceTypes::resourceWood,        //resourceTypes           typeOfResource,
        200,                                //int                     startAmountOfResources,
        "Cactus",                           //std::string             realName,
        {-23 , 16},                           //cords                   spriteOrigin,
        {20, 33},                           //cords                   textureRect,
        "textures/cactus.png",              //std::string             texture
        0                                   //int                     ObjectBigSpriteYOffset
        });

    //cypress
    listOfObjectTemplates.push_back({
        objectTypes::cypress,         //objectTypes             objectId,
        resourceTypes::resourceWood,        //resourceTypes           typeOfResource,
        200,                                //int                     startAmountOfResources,
        "Cypress",                          //std::string             realName,
        {-20 , 37},                           //cords                   spriteOrigin,
        {25, 54},                           //cords                   textureRect,
        "textures/cypressTree.png",          //std::string             texture
        128                                 //int                     ObjectBigSpriteYOffset
        });

    //Maple
    listOfObjectTemplates.push_back({
        objectTypes::maple,           //objectTypes             objectId,
        resourceTypes::resourceWood,        //resourceTypes           typeOfResource,
        200,                                //int                     startAmountOfResources,
        "Maple",                            //std::string             realName,
        {-19 , 34},                           //cords                   spriteOrigin,
        {29, 52},                           //cords                   textureRect,
        "textures/normalTree.png",           //std::string             texture
        256                                 //int                     ObjectBigSpriteYOffset
        });

    //Pine
    listOfObjectTemplates.push_back({
        objectTypes::pine,            //objectTypes             objectId,
        resourceTypes::resourceWood,        //resourceTypes           typeOfResource,
        200,                                //int                     startAmountOfResources,
        "Pine",                             //std::string             realName,
        {-19 , 37},                           //cords                   spriteOrigin,
        {30, 54},                           //cords                   textureRect,
        "textures/pineTree.png",             //std::string             texture
        384                                 //int                     ObjectBigSpriteYOffset
        });

    //stone
    listOfObjectTemplates.push_back({
        objectTypes::stone,           //objectTypes             objectId,
        resourceTypes::resourceStone,       //resourceTypes           typeOfResource,
        2000,                               //int                     startAmountOfResources,
        "Stone",                            //std::string             realName,
        {0 , 2},                            //cords                   spriteOrigin,
        {64, 32},                           //cords                   textureRect,
        "textures/stone.png",                //std::string             texture
        512                                 //int                     ObjectBigSpriteYOffset
        });

    //gold
    listOfObjectTemplates.push_back({
        objectTypes::gold,            //objectTypes             objectId,
        resourceTypes::resourceGold,        //resourceTypes           typeOfResource,
        1500,                               //int                     startAmountOfResources,
        "Gold",                             //std::string             realName,
        {0 , 2},                            //cords                   spriteOrigin,
        {64, 32},                           //cords                   textureRect,
        "textures/gold.png",                 //std::string             texture
        640                                 //int                     ObjectBigSpriteYOffset
        });

    //Berrybush
    listOfObjectTemplates.push_back({
        objectTypes::berry,          //objectTypes             objectId,
        resourceTypes::resourceFood,       //resourceTypes           typeOfResource,
        5000,                              //int                     startAmountOfResources,
        "Berrybush",                       //std::string             realName,
        {0 , 2},                           //cords                   spriteOrigin,
        {64, 32},                          //cords                   textureRect,
        "textures/berrybush.png",           //std::string             texture
        768                                 //int                     ObjectBigSpriteYOffset
        });

    for (int i = 0; i < listOfObjectTemplates.size(); i++) {
        listOfObjectTemplates[i].setTexture();
    }
}

std::vector<objectTemplates> listOfObjectTemplates;


